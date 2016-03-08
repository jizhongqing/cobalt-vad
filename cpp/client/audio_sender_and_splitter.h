#ifndef COBALT_GUARD_AUDIO_SENDER_AND_SPLITTER_H_
#define COBALT_GUARD_AUDIO_SENDER_AND_SPLITTER_H_
#include <vector>
#include <string>
#include "util/vad_events.h"
#include "client/audio_chunk_getter.h"

namespace vad_client
{
    // a class that sends audio and splits it into speech chunks.
    class AudioSenderAndSplitter
    {
    public:
        AudioSenderAndSplitter(const std::string& audioFilePath,
                               const std::string& modelId,
                               const std::string& outputDir,
                               int chunkSizeMsec);
        void sendAndSplit();

    private:
        const std::string mAudioFilePath;
        const std::string mModelId;
        const std::string mOutputDir;
        const int mChunkSizeMsec;
        int mNumFilesSplit;
        void splitAudio(const cobalt::VadEvents& allEvent,
                const AudioChunkGetter& audioChunkGetter, const std::string& detectorId);
        void splitChunk(const std::vector<short>& samples,
                size_t sosSample, size_t eosSample, int samplingFreq,
                const std::string& detectorId);
    };
}



#endif
