#include "client/client_utils.h"
#include "api/api.h"
#include "util/io.h"

namespace vad_client
{
    MakeDetectorName MakeDetectorName::sMakeDetectorName;

    string MakeDetectorName::makeDetectorName(const string& id)
    {
        string idToInsert = id;
        const string wavEnd = ".wav";
        if(cobalt::endsWith(idToInsert, wavEnd))
        {
            idToInsert = idToInsert.substr(0, idToInsert.size() - wavEnd.size());
        }
        LockGuard_t lock(mMutex);
        while (mRecognizerIds.find(idToInsert) != mRecognizerIds.end())
        {
            idToInsert = idToInsert + "_";
        }
        mRecognizerIds.insert(idToInsert);
        return idToInsert;
    }

    void MakeDetectorName::removeDetectorName(const string& id)
    {
        LockGuard_t lock(mMutex);
        if (mRecognizerIds.find(id) == mRecognizerIds.end())
        {
            throw std::runtime_error(string("removing an unknown recognizer name: ") + id);
        }
        mRecognizerIds.erase(id);
    }


    void checkApiReturn(const ApiReturn& ret)
    {
        // 0 means success
        if (ret.error == 0)
        {
            return;
        }
        // copy the string and use api call to release memory
        const string error(ret.errorMessage);
        Api_Clear(ret.errorMessage);
        throw std::runtime_error(error);
    }

    void checkDetectorReturn(const DetectorReturn& ret, string& jsonVadEvents)
    {
        // 0 means success
        if (ret.error == 0)
        {
            jsonVadEvents = ret.resultsJson;
            // copy the string and use api call to release memory
            Api_Clear(ret.resultsJson);
            return;
        }
        // copy the string and use api call to release memory
        const string error(ret.errorMessage);
        Api_Clear(ret.errorMessage);
        throw std::runtime_error(error);
    }
}


