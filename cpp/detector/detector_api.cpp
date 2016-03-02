#include "detector/detector_api.h"

namespace cobalt
{
    DetectorApi DetectorApi::sDetectorApi;

    void DetectorApi::Api_NewModel(CobaltString modelIdC, CobaltString pathC)
    {
        const string modelId(modelIdC);
        const string modelPath(pathC);

        std::stringstream ss;
        ss << "Api_NewModel, creating model with id " << modelId << ", path " << modelPath;

        logMessage(LOGLEVEL_INFO, ss.str());
        {
            LockGuard lock(mMutex);
            if(mModels.find(modelId) != mModels.end())
            {
                throw std::runtime_error(string("Api_NewModel: model id ") + modelId + " is in use.");
            }
        }
        VadModel::Ptr model = boost::make_shared<VadModel>(modelPath);

        {
            LockGuard lock(mMutex);
            // check this again, another thread might have created the model while we were busy.
            if(mModels.find(modelId) != mModels.end())
            {
                throw std::runtime_error(string("Api_NewModel: model id ") + modelId + " is in use.");
            }
            mModels[modelId] = model;
        }
        ss.str(string());
        ss << "Api_NewModel, successfully created model " << modelId;
        logMessage(LOGLEVEL_INFO, ss.str());
    }

    void DetectorApi::Api_DeleteModel(CobaltString modelIdC)
    {
        const string modelId(modelIdC);
        std::stringstream ss;
        ss << "Api_DeleteModel, deleting model with id " << modelId;
        logMessage(LOGLEVEL_INFO, ss.str());
        {
            LockGuard lock(mMutex);
            map<string, VadModel::Ptr>::iterator it = mModels.find(modelId);
            if(it == mModels.end())
            {
                throw std::runtime_error(string("Api_DeleteModel: model id ") + modelId + " not found.");
            }
            mModels.erase(it);
        }
        ss.str(string());
        ss << "Api_DeleteModel, successfully deleted model " << modelId;
        logMessage(LOGLEVEL_INFO, ss.str());
    }


    void DetectorApi::Api_NewDetector(CobaltString detectorIdC, CobaltString modelIdC)
    {
        const string detectorId(detectorIdC);
        const string modelId(modelIdC);
        std::stringstream ss;
        ss << "Api_NewDetector, creating ";
        ss << " detector with id " << detectorId << " using model id " << modelId;
        logMessage(LOGLEVEL_INFO, ss.str());
        VadDetector::Ptr detector = createDetector(detectorId, modelId);
        {
            LockGuard lock(mMutex);
            mDetectors[detectorId] = detector;
        }
        ss.str(string());
        ss << "Api_NewDetector, successfully created detector " << detectorId;
        logMessage(LOGLEVEL_INFO, ss.str());
    }

    VadDetector::Ptr DetectorApi::createDetector(const string& detectorId, const string& modelId)
    {
        VadModel::Ptr model;
        {
            LockGuard lock(mMutex);
            if(mDetectors.find(detectorId) != mDetectors.end())
            {
                throw std::runtime_error(string("Create Detector: detector id ") + detectorId + " is in use.");
            }
            // number of detector in the map, plus the current detector being created.
            map<string, VadModel::Ptr>::iterator it = mModels.find(modelId);
            if(it == mModels.end())
            {
                throw std::runtime_error(string("Create Detector: model id ") + modelId + " not found.");
            }
            model = it->second;
        }
        // let go of the lock.
        VadDetector::Ptr detector = boost::make_shared<VadDetector>(model);
        return detector;
    }

    void DetectorApi::Api_DeleteDetector(CobaltString detectorIdC)
    {
        const string detectorId(detectorIdC);
        std::stringstream ss;
        ss << "Api_DeleteDetector, deleting detector with id " << detectorId;
        logMessage(LOGLEVEL_INFO, ss.str());
        VadDetector::Ptr detector;
        {
            LockGuard lock(mMutex);
            map<string, VadDetector::Ptr>::iterator it = mDetectors.find(detectorId);
            if(it == mDetectors.end())
            {
                throw std::runtime_error(string("Api_DeleteDetector: detector id ") + detectorId + " not found.");
            }
            detector = it->second;
            mDetectors.erase(it);
        }
        detector->shutdownDetector();
        ss.str(string());
        ss << "Api_DeleteDetector, successfully deleted detector " << detectorId;
        logMessage(LOGLEVEL_INFO, ss.str());
    }

    void DetectorApi::Detector_PushEvent(CobaltString detectorIdC, AudioEvent* event, string& jsonResults)
    {
        const string detectorId(detectorIdC);
        if (event == NULL)
        {
            throw std::runtime_error(string("Detector_PushEvent: pushing NULL RecognitionEvent to ") + detectorId);
        }
        VadDetector::Ptr detector;

        {
            LockGuard lock(mMutex);
            map<string, VadDetector::Ptr>::iterator it = mDetectors.find(detectorId);
            if(it == mDetectors.end())
            {
                throw std::runtime_error(string("Detector_PushEvent: detector id ") + detectorId + " not found.");
            }
            detector = it->second;
        }

        VadEvents events;
        detector->pushEvents(event, events);
        JsonValuePtr jsonEvents = serializeVadEvents(events);
        Json::FastWriter fastWriter;
        fastWriter.omitEndingLineFeed();
        jsonResults = fastWriter.write(*jsonEvents);

    }

    DetectorApi::DetectorApi()
    {
        mLogger = boost::make_shared<ApiLogger>(&(mLoggingCallback));

    }
    void DetectorApi::Api_RegisterLoggingCallback(Logging_Callback callback)
    {
        setLogger(mLogger);
        mLoggingCallback = callback;
    }

    DetectorApi::~DetectorApi()
    {
        std::stringstream ss;
        // we are destruction, meaning the process is shutting down, log to stderr instead of logging handlers.
        if (mModels.size() != 0)
        {
            ss << "Active models remain upon API destruction.";
            std::cout << ss.str() << endl;
        }
        if (mDetectors.size() != 0)
        {
            ss.str(string());
            ss << "Active detectors remain upon API destruction.";
            std::cout << ss.str() << endl;
        }
    }
}
