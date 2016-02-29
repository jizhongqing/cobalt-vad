#ifndef COBALT_GUARD_COBALTVAD_H
#define COBALT_GUARD_COBALTVAD_H
extern "C"
{
    // C API header file.

    /**
    * @file cobalt_vad.h
    * This is a thread-safe Voice Activity Detection (VAD) API. The objective of
    * this api is to convert audio to speech start and speech end timestamps.
    *
    * There are two main types of 'objects' in this API: Models and Recognizers
    * 
     * A Model is an encapsulation for machine-learned artifacts that were trained in
     * offline, supervised fashion. Models are the 'brain' of our system. Models own the
     * the accuracy or recognition.
     *
     * A Recognizer is created from a model and contains the logic for performing speech recognition.
     * Recognizers own the latency and stability of recognition.
     *
     * Models are typically heavy weight objects that should be created once and used often. Recognizers
     * are light weight objects should be created per audio stream (unless otherwise advised).
     *
     * This API provides two main classes of functions: API level functions, prefixed by API_, are responsible
     * for creating and deleting objects, such as models and recognizers. Object specific functions,
     * prefixed by object names such as Recognizer_, are responsible for controlling specific functionality of each
     * object, such as pushing audio to a recognizer.
     *
     * Each API call returns success of failure. If the call is successful, the object created for the API call may be used;
     * if the call failed, the reason for failure is documented in the return struct.
     *
     * The API is stateful, and uses string (char*) based IDs to maintain state; e.g. clients create a Models and
     * control the ID of the model by setting modelIdC, that model may then be used by calling other API functions
     * with the same ID.
     *
     * Memory management is crucial to the success of the API, the client must make sure each pointer object
     * passed to an API call remains in scope until the call returns. Pointers returned by the API will remain
     * in scope until the client calls the "clear" function. Best practice is to copy the contents each pointer, and call
     * clear() immediately, to avoid memory usage growth.
     *
     * The API delivers objects via callbacks, e.g. logging, metrics, and results.
     *
     * Callbacks are registered as function pointers. The registration calls are not thread-safe. Best practice
     * is to registered each callback once at the start of the process. Callbacks should be registered prior to any other
     * API calls.
     *
     * The API delivers per-recognition results in asynchronous fashion. To allow the client to map a result to a recognizer,
     * the result (and metrics) callbacks contain a field for the id of the recognizer. Each recognizer object delivers
     * (0+) results per lifetime, depending on the underlying model and Recognizer_ API calls. 
    #include "api_types.h"
    **/

/*
    Given a  model id and a path to a config file, instantiates VAD configuration
*/
COBALTVAD ApiReturn Api_NewModel(CobaltString modelIdC, CobaltString modelPathC);
COBALTVAD ApiReturn Api_DeleteModel(CobaltString modelIdC);
COBALTVAD ApiReturn Api_NewRecognizer(CobaltString recognizerIdC, CobaltString modelIdC)
COBALTVAD ApiReturn Api_DeleteRecgonizer(CobaltString recognizerIdC);

COBALTVAD ApiReturn Api_PushEvent(CobaltString recognizerIdC, RecognitionEvent* event);
COBALTVAD ApiReturn Api_RegisterLoggingCallback(Logging_Callback callback);/

}
#endif /* COBALT_GUARD_COBALTVAD_H
