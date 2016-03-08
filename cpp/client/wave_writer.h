#ifndef COBALT_GUARD_WAVE_WRITER_H_
#define COBALT_GUARD_WAVE_WRITER_H_
#include <fstream>
#include <string>
#include <vector>
#include "util/io.h"
namespace vad_client
{
    // a limited class that writes very specific wav files: 16
    class WaveWriter
    {
    public:
        WaveWriter(const std::string& outputFilePath)
        : mOutputFilePath(outputFilePath)
        {}

        void writeWav(const std::vector<short>& data, int sampleRate);

        void writeWav(const std::vector<short>& data, size_t beginSample, size_t endSample, int sampleRate);
    private:
        const std::string mOutputFilePath;
        void writeHeader(int numSamples, int sampleRate, cobalt::OfstreamPtr outfile);
    };
}

#endif
