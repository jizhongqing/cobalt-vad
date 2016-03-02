#ifndef COBALT_GUARD_AUDIO_IO_H_
#define COBALT_GUARD_AUDIO_IO_H_

#include "feat/wave-reader.h"
#include "api/api_types.h"
#include "util/io.h"

namespace cobalt
{
	void wavToAudioEvent(kaldi::WaveData& wavData, AudioEvent& event);

	void clearAudioEvent(AudioEvent* event);
}

#endif /* COBALT_GUARD_AUDIO_IO_H_ */
