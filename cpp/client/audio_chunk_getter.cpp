#include "client/audio_chunk_getter.h"
#include "util/io.h"
namespace vad_client
{

	AudioChunkGetter::AudioChunkGetter(const string& filename)
	: mCurrentSampleIndex(0)
	{
		cobalt::IfstreamPtr fstream = cobalt::readfile(filename, std::ios::in | std::ios::binary);
		mWavData.Read(*fstream);
		kaldi::SubVector<float> data(mWavData.Data(), 0);
		for (int i = 0; i < data.Dim(); ++i)
		{
			mSamples.push_back(static_cast<short>(data(i)));
		}
	}

	const kaldi::WaveData& AudioChunkGetter::getWavData()
	{
	    return mWavData;
	}

	// get the next chunk of audio, at most of size mChunkSizeSamples
	void AudioChunkGetter::getNextChunk(std::vector<short> &samples, int chunkSizeMsec)
	{
		if (fileDone())
		{
			// throwing here, because client code should use fileDone() function to prevent this.
			throw std::runtime_error("No samples to get, call fileDone before calling this function.");
		}
		const size_t requestedChunkSizeSamples = getNumSamplesFromMsec(chunkSizeMsec);

		// suppose 500 samples in file, mCurrentSampleIndex = 400, then there are 100 left (499-400+1)
		const size_t numSamplesRemainingInFile = mSamples.size() - mCurrentSampleIndex;
		const size_t numSamplesToGet = requestedChunkSizeSamples < numSamplesRemainingInFile ? requestedChunkSizeSamples : numSamplesRemainingInFile;

		samples.resize(numSamplesToGet);
		for(size_t i = mCurrentSampleIndex; i < mCurrentSampleIndex + numSamplesToGet; ++i)
		{
			samples[i-mCurrentSampleIndex] = mSamples[i];
		}
		mCurrentSampleIndex += numSamplesToGet;
	}

	bool AudioChunkGetter::fileDone() const
	{
		return mCurrentSampleIndex >= mSamples.size();
	}

	int AudioChunkGetter::getNumSamplesFromMsec(int sizeInMsec) const
	{
		// SampFreq is HZ, so suppose 8000, that means 8 samples per msec,
		// so divide by 1000 and multiply by chunkSizeMsec
		return mWavData.SampFreq() * sizeInMsec / 1000;
	}

}
