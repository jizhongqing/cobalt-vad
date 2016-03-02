#ifndef COBALT_GUARD_COBALT_API_H_
#define COBALT_GUARD_COBALT_API_H_

typedef const char* CobaltString;

#include <iostream>
#include <map>
#include <boost/thread.hpp>
#include "api/api_types.h"
#include "detector/vad_model.h"
#include "detector/detector.h"
#include <boost/shared_ptr.hpp>
#include "util/logger.h"
#include "detector/api_logger.h"

using std::map;


namespace cobalt
{
        class CobaltApi
        {
        public:
            // the only instance of the cobalt api.
            static CobaltApi sCobaltApi;
        private:
            // singleton
            CobaltApi();
            ~CobaltApi();
        public:
            // returns if successful, throws if not.
            void Api_NewModel(CobaltString modelIdC, CobaltString modelPathC);
    
            void Api_DeleteModel(CobaltString modelIdC);
    
            void Api_NewDetector(CobaltString detectorIdC, CobaltString modelIdC);
    
            void Api_DeleteDetector(CobaltString detectorIdC);
    
            void Detector_PushEvent(CobaltString detectorIdC, AudioEvent* event, string& jsonResults);
    
            // callback function registration. These functions are not thread safe.
            // Best practice is to register callbacks once, before other API calls, and leave them
            // alone for the lifetime of the process.
    
            // Logging callback.
            void Api_RegisterLoggingCallback(Logging_Callback callback);

    private:
        // members
        boost::mutex mMutex;
        typedef boost::lock_guard<boost::mutex> LockGuard;

        map<string, VadModel::Ptr> mModels;
        map<string, VadDetector::Ptr> mDetectors;

        Logger::Ptr mLogger;
        Logging_Callback mLoggingCallback;
        

    private:
        // methods
        VadDetector::Ptr createDetector(const string& detectorId, const string& modelIdC);

        VadDetector::Ptr newDetector(CobaltString detectorIdC, CobaltString modelIdC);

    };



}

#endif /* COBALT_GUARD_COBALT_API_H_ */
