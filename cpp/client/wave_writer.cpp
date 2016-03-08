#include "client/wave_writer.h"
#include "util/io.h"

namespace vad_client
{

    template <typename T>
    void write(std::ofstream& stream, const T& t) {
        stream.write((const char*)&t, sizeof(T));
    }


    void WaveWriter::writeHeader(int numSamples, int sampleRate, cobalt::OfstreamPtr outfile)
    {
        // size in bytes.
        const int dataSize = numSamples * 2;
        /* Header */
        outfile->write("RIFF", 4);                                        // "RIFF"
        write<int>(*outfile, 36 + dataSize);                               // dwFileLength
        outfile->write("WAVE", 4);                                        // "WAVE"

        /* Format Chunk */
        outfile->write("fmt ", 4);                                        // "fmt "
        write<int>(*outfile, 16);                                         // Chunk size (of Format Chunk)
        write<short>(*outfile, 1);                                        // 1 for PCM)
        write<short>(*outfile, 1);                                 // number of channels
        write<int>(*outfile, sampleRate);                                 // sampling Rate
        write<int>(*outfile, sampleRate * sizeof(short)); // byte rate
        write<short>(*outfile, sizeof(short));            // frame size
        write<short>(*outfile, 8 * sizeof(short));                   // bits per sample, always 16

        /* Data Chunk */
        outfile->write("data", 4);                                        // "data"
        outfile->write((const char*)&dataSize, 4);                         // size of data.
    }

    void WaveWriter::writeWav(const std::vector<short>& data, size_t beginSample, size_t endSample, int sampleRate)
    {
        const int numSamplesToWrite = endSample - beginSample + 1;
        if (data.size() < numSamplesToWrite)
        {
            throw std::runtime_error("Requested writing more samples than were in the data.");
        }
        cobalt::OfstreamPtr outfile = cobalt::writefile(mOutputFilePath, std::ios::binary);
        writeHeader(numSamplesToWrite, sampleRate, outfile);
        outfile->write((const char*)(&data[beginSample]), numSamplesToWrite * 2);
    }
    void WaveWriter::writeWav(const std::vector<short>& data, int sampleRate)
    {
        cobalt::OfstreamPtr outfile = cobalt::writefile(mOutputFilePath, std::ios::binary);
        writeHeader(data.size(), sampleRate, outfile);
        outfile->write((const char*)(&data[0]), data.size() * 2);
    }

}

