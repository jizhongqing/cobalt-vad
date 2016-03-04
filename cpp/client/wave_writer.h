#ifndef COBALT_GUARD_WAVE_WRITER_H_
#define COBALT_GUARD_WAVE_WRITER_H_
#include <fstream>
#include <string>
#include <vector>
namespace vad_client
{
    // a limited class that writes very specific wav files: 16
    class WaveWriter
    {
    public:
        WaveWriter(const std::string& outputFilePath)
        : mOutputFilePath(outputFilePath)
        {}

        void writeWav(const std::vector<short>& data, int sampleRate, short channels);
    private:
        const std::string mOutputFilePath;
    };
}

#endif
