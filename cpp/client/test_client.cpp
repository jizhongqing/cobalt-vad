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
    WaveWriter writer("out_woman1_wb.wav");
    writer.writeWav(samples, waveData.SampFreq(), 1);

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
