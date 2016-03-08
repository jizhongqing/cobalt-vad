#ifndef CPP_COBALT_CLIENT_CLIENT_UTILS_H_
#define CPP_COBALT_CLIENT_CLIENT_UTILS_H_
#include "util/usingstl.h"
#include "util/vad_events.h"
#include "api/api_types.h"

#include <set>
#include <boost/thread.hpp>

namespace vad_client
{
    /**
     * Helper function for checking the return struct of API calls.
     * Throws with the struct's error message if API call failed.
     */
	void checkApiReturn(const ApiReturn& ret);

    /**
     * Helper function for checking the return struct of API calls.
     * Throws with the struct's error message if API call failed.
     * copies message and clears memory of return.
     */
	void checkDetectorReturn(const DetectorReturn& ret, string& jsonVadEvents);

    /**
     * class that helps make a detector name, with the condition that no two detectors share the same name.
     */
    class MakeDetectorName
    {
    public:
        // the only instance of MakeDetectorName .
        static MakeDetectorName sMakeDetectorName;

        /** Create a detector name,
         * if there is a naming collision, meaning id is active,
         * we will append to the id.
         */
        string makeDetectorName(const string& id);

        /**
         * Remove a detector name, call this with the ids created by makeDetectorName(),
         * once you are done with the detector name.
         */
        void removeDetectorName(const string& id);

    private:
        MakeDetectorName(){}

        std::set<string> mRecognizerIds;
        typedef boost::lock_guard<boost::mutex> LockGuard_t;
        boost::mutex mMutex;
    };

    void printVadEvents(const cobalt::VadEvents& events);
}

#endif /* CPP_COBALT_CLIENT_CLIENT_UTILS_H_ */
