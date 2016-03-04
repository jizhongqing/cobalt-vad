#ifndef COBALT_GUARD_AUDIO_CHUNK_GETTER_H_
#define COBALT_GUARD_AUDIO_CHUNK_GETTER_H_
#include "feat/wave-reader.h"
#include "util/usingstl.h"

namespace vad_client
{
	class AudioChunkGetter
	{
	public:
		AudioChunkGetter(const string& filename);

		// get the next chunk of audio, at most of size mChunkSizeSamples
		void getNextChunk(std::vector<short> &samples, int chunkSizeMsec);

		bool fileDone() const;

		const kaldi::WaveData& getWavData();

		const std::vector<short>& getSamples() const
		{
		    return mSamples;
		}

		const float getSampFreq() const
		{
		    return mWavData.SampFreq();
		}
	private:
		std::vector<short> mSamples;
		size_t mCurrentSampleIndex;
		kaldi::WaveData mWavData;

	private:
		// methods
		int getNumSamplesFromMsec(int sizeInMsec) const;
	};

}

#endif /* COBALT_GUARD_AUDIO_CHUNK_GETTER_H_ */
