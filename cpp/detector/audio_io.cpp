/*
 * audio_io.cpp
 *
 *  Created on: Jul 16, 2015
 *      Author: kevinyang
 */
#include "detector/audio_io.h"

namespace cobalt
{
	void wavToAudioEvent(kaldi::WaveData& wavData, AudioEvent& event)
	{
		kaldi::SubVector<float> data(wavData.Data(), 0);
		event.audio = new short[data.Dim()];
		for(int i = 0; i < data.Dim(); ++i)
		{
			event.audio[i] = data(i);
		}
		event.size = data.Dim();
	}

	// meant to be used after calls to wavToAudioEvent.
    void clearAudioEvent(AudioEvent* event)
    {
        if (event->audio)
        {
            delete event->audio;
        }
    }
}
