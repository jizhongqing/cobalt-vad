#include "client/audio_sender_and_splitter.h"
#include "detector/audio_io.h"
#include "client/client_utils.h"
#include "client/audio_chunk_getter.h"
#include "client/wave_writer.h"
#include "api/api.h"
#include "client/client_detector.h"
#include "util/io.h"
#include <boost/lexical_cast.hpp>

namespace vad_client
{
    AudioSenderAndSplitter::AudioSenderAndSplitter(const std::string &audioFilePath,
                           const std::string& modelId,
                           const std::string& outputDir,
                           int chunkSizeMsec)
    : mAudioFilePath(audioFilePath), mModelId(modelId),
      mOutputDir(outputDir), mChunkSizeMsec(chunkSizeMsec), mNumFilesSplit(0)
    {}

    void AudioSenderAndSplitter::splitChunk(const std::vector<short>& samples,
            size_t sosSample, size_t eosSample, int samplingFreq, const string& detectorId)
    {
        if (eosSample < sosSample)
        {
            throw std::runtime_error("end of speech sample smaller than sos sample.");
        }
        if (eosSample > samples.size()-1 )
        {
            eosSample = samples.size()-1;
        }
        const string leaf = cobalt::splitString(detectorId, "/").second;
        const string outputFilePath = mOutputDir + "/" + leaf + "-" + boost::lexical_cast<string>(mNumFilesSplit++) + ".wav";

        // TODO: Implement verbose mode
        //cout << "creating new file " << outputFilePath << endl;
        WaveWriter waveWriter(outputFilePath);
        waveWriter.writeWav(samples, sosSample, eosSample, samplingFreq);
    }

    void AudioSenderAndSplitter::splitAudio(const cobalt::VadEvents& allEvent,
            const AudioChunkGetter& audioChunkGetter,
            const string& detectorId)
    {
        const int samplingFreq = audioChunkGetter.getSampFreq();
        const int samplesPerMsec = samplingFreq / 1000;
        // indicates whether we are in speech, starts as EOS because at the beginning of file we are not in speech
        cobalt::VadEventType previousEvent = cobalt::EOS;
        int lastSosMsec = 0;
        printVadEvents(allEvent);

        for (size_t i = 0; i < allEvent.size(); ++i)
        {
            // a switch of event state happened
            if (allEvent[i].vadEventType != previousEvent)
            {
                // SOS to EOS, we need to split out this chunk
                if (previousEvent == cobalt::SOS)
                {
                    const size_t eosSample = allEvent[i].eventOccurenceMsec * samplesPerMsec;
                    const size_t sosSample = lastSosMsec * samplesPerMsec;
                    // cout << "Splitting chunks on event " << i << endl;
                    splitChunk(audioChunkGetter.getSamples(), sosSample, eosSample, samplingFreq, detectorId);
                }
                // EOS to SOS, we record a new lastSosMsec
                else if (previousEvent == cobalt::EOS)
                {
                    lastSosMsec = allEvent[i].eventOccurenceMsec;
                }
                previousEvent = allEvent[i].vadEventType;
            }
        }
    }

    void AudioSenderAndSplitter::sendAndSplit()
    {
        ClientDetector detector(mAudioFilePath, mModelId);
        cobalt::VadEvents allEvent;

        AudioChunkGetter audioChunkGetter(mAudioFilePath);

        // we're going to push this chunk by chunk.
        while (!audioChunkGetter.fileDone())
        {
            std::vector<short> sampleChunk;
            audioChunkGetter.getNextChunk(sampleChunk, mChunkSizeMsec);
            cobalt::VadEvents eventThisPush;
            detector.pushAudio(sampleChunk, eventThisPush);
            allEvent.insert( allEvent.end(), eventThisPush.begin(), eventThisPush.end() );
        }
        const string detectorId = detector.getDetectorId();
        detector.shutdownDetector();
        splitAudio(allEvent, audioChunkGetter, detectorId);
    }
}

