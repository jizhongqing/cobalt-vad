#ifndef COBALT_GUARD_COBALTVAD_H
#define COBALT_GUARD_COBALTVAD_H
extern "C"
{
    // C API header file.

    /**
    * @file cobalt_vad.h
    *
    * This is a thread-safe Voice Activity Detection (VAD) API. The objective of
    * this api is to convert audio to "speech_start" and "speech_end" timestamps.
    *
    * There are two main types of 'objects' in this API: Models and Detectors
    * 
    * A Model is an encapsulation for machine-learned artifacts that were 
    * trained offline. Models are the 'brain' of our system. Models own the
    * accuracy of detection. 
    *
    * A Detector is created from a model and contains the logic for performing
    * voice activity detection. Detectors own the latency and stability of 
    * detection.
    *
    * Models are typically heavy weight objects that should be created once and
    * used often. Detectors are light weight objects should be created per audio
    * stream (unless otherwise advised).
    *
    * This API provides two main classes of functions: API level functions, 
    * prefixed by API_, are responsible for creating and deleting objects, such
    * as models and detectors. Object specific functions, prefixed by object
    * names such as Detector_, are responsible for controlling specific
    * functionality of each object, such as pushing audio to a detector.
    *
    * Each API call returns success of failure. If the call is successful, the
    * object created for the API call may be used; if the call failed, the
    * reason for failure is documented in the return struct.
    *
    * The API is stateful, and uses string (char*) based IDs to maintain state;
    * e.g. clients create a Models and control the ID of the model by setting
    * modelIdC, that model may then be used by calling other API functions
    * with the same ID.
    *
    * Memory management is crucial to the success of the API, the client must
    * make sure each pointer object passed to an API call remains in scope until
    * the call returns. Pointers returned by the API will remain in scope until
    * the client calls the "clear" function. Best practice is to copy the
    * contents each pointer, and call clear() immediately, to avoid memory
    * usage growth.
    *
    *
    * The API delivers results synchronously, as a return from the API call.
    * The results returned by PushEvent will contain timestamps of detected
    * "speech_start" and "speech_end" events. It is NOT guaranteed that the
    * speech events will be in the audio received by that PushEvent.
    *
    * For example, if the speech starts in the last frame of the DetectionEvent,
    * the "speech_start" may not be returned until the next PushEvent, because
    * the VAD library requires more frames of context before rendering a
    * decision.
    */

    #include "api_types.h"
    /**
    * Entry point for creating a new model
    * \param[in] modelIdC 
    *
    *   The ID of the successfully created model,
    *   if call is successful, clients may use the model by sending the ID
    *   to other API calls.
    *
    *   The ID must be unique. The API call will fail if clients attempt to
    *   create a model with a ID identical to an existing model.
    *
    * \param[in] modelPathC
    *   The on-disk filepath of the model.
    */
    ApiReturn Api_NewModel(CobaltString modelIdC, CobaltString modelPathC);

    /**
    * Entry point for deleting an existing model.
    *
    * \param[in] modelIdC
    *   The ID of the model to be deleted.
    */
    ApiReturn Api_DeleteModel(CobaltString modelIdC);

    /**
    * Entry point for creating a new detector.
    * \param[in] detectorIdC
    *   The ID of the successfully created detector. If this API call is
    *   successful, clients may use the detector by sending the ID to other
    *   API calls.
    *
    *   The ID must be unique. The API call will fail if clients attempt to
    *   create a detector with an ID identical to an existing detector.
    *
    * \param[in] modelIdC
    *   The ID of the model to be used by the detector.
    * 
    */
    ApiReturn Api_NewDetector(CobaltString detectorIdC, CobaltString modelIdC);

    /**
    * Entry point for deleting a detector.
    *
    * \param[in] detectorIdC
    *   The ID of the detector to be deleted.
    */
    ApiReturn Api_DeleteDetector(CobaltString detectorIdC);

    /**
    * Entry point for pushing audio to a detector.
    * \param[in] detectorIdC
    *   The ID of the detector. The detector must alreadt exist, through the
    *   NewDetector call.
    *
    * \param[in] event
    *   The detection event. Examples of event types are audio events and
    *   end-of-session events.
    */
    DetectorReturn Api_PushEvent(CobaltString detectorIdC, DetectionEvent* event);

    /**
    * Entry point for registering a logging callback
    *
    * \param[in] callback          Function Pointer to make callbacks to.
    *
    */
    ApiReturn Api_RegisterLoggingCallback(Logging_Callback callback);

    /**
    * Entry point for deleting memory.
    * The library will return objects that are active on the heap, e.g. error messages.
    * The client should use this function to clear that memory, after the client no longer needs the memory.
    *
    * \param[in] clear             The Pointer for the memory to be cleared.
    *
    */
    void Api_Clear(Memory clear);
} // extern "C"
#endif /* COBALT_GUARD_COBALTVAD_H */
