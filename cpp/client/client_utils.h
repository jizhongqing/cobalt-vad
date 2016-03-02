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
     * class that helps make a recognizer name, with the condition that no two recognizers share the same name.
     */
    class MakeRecognizerName
    {
    public:
        // the only instance of MakeRecognizerName .
        static MakeRecognizerName sMakeRecognizerName;

        /** Create a recognizer name,
         * if there is a naming collision, meaning id is active,
         * we will append to the id.
         */
        string makeRecognizerName(const string& id);

        /**
         * Remove a recognizer name, call this with the ids created by makeRecognizerName(),
         * once you are done with the recognizer name.
         */
        void removeRecognizerName(const string& id);

    private:
        MakeRecognizerName(){}

        std::set<string> mRecognizerIds;
        typedef boost::lock_guard<boost::mutex> LockGuard_t;
        boost::mutex mMutex;
    };
}

#endif /* CPP_COBALT_CLIENT_CLIENT_UTILS_H_ */
