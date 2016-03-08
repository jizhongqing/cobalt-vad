#include "client/wave_writer.h"
#include "client/audio_chunk_getter.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <boost/filesystem.hpp>

using namespace std;
using namespace vad_client;

void test_wave_writer()
{
    cout << endl << endl << "Begin test_wave_writer()" << endl;
    // first get some audio.
    const string audioFile = "data/audio/woman1_wb.wav";
    AudioChunkGetter getter(audioFile);
    std::vector<short> samples;
    // use a large chunk size, should get most of the file.
    const int chunkSizeMsec = 10000;
    // get the next chunk of audio, at most of size mChunkSizeSamples
    getter.getNextChunk(samples, chunkSizeMsec);

    // now write the audio.
    const kaldi::WaveData& waveData = getter.getWavData();

    const string outfile = "out_woman1_wb.wav";
    WaveWriter writer(outfile);
    writer.writeWav(samples, waveData.SampFreq());

    AudioChunkGetter getter2(outfile);
    std::vector<short> samples2;
    // get the next chunk of audio, at most of size mChunkSizeSamples
    getter2.getNextChunk(samples2, chunkSizeMsec);

    if (samples != samples2)
    {
        throw std::runtime_error("Expected equal samples.");
    }
    cout << "bytes read first and then re-written are equal." << endl;
    // remove the temp file if the test succeeds.
    boost::filesystem::remove(outfile);

    cout << "Test writing chunks of file" <<endl;
    const size_t beginSample = 10;
    const size_t endSample = 500;
    writer.writeWav(samples, beginSample, endSample, waveData.SampFreq());

    AudioChunkGetter getter3(outfile);
    std::vector<short> samples3;
    // get the next chunk of audio, at most of size mChunkSizeSamples
    getter3.getNextChunk(samples3, chunkSizeMsec);

    for (size_t i = beginSample; i < endSample; ++i)
    {
        if (samples[i] != samples3[i-beginSample])
        {
            throw std::runtime_error("Samples not equal");
        }
    }
    cout << endl << "bytes read first and then re-written are equal, with offset and endsample." << endl;
    cout << endl << endl << "Begin test_wave_writer()" << endl;
}
int main()
{
    try
    {
        test_wave_writer();
    }
    catch (std::runtime_error &e)
    {
        cout << "Caught C++ runtime error: " << e.what() << endl;
        return 1;
    }
    catch ( ... )
    {
        cout << "Caught unknown error!!";
        return 2;
    }
    return 0;
}
