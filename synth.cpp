/* complete linux synthesizer using RtAudio and RtMidi */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>

#include "lfluidsynth.h"

#include "rtaudio/RtAudio.h"

//#define AUDIO_FORMAT 	RTAUDIO_FLOAT32
#define AUDIO_FORMAT 	RTAUDIO_SINT16

#define POLYPHONY 	64
#define SAMPLE_RATE 	44100

#define FRAME_SIZE 	32

using namespace std;

int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData)
{
	fluid_synth_t *synth = (fluid_synth_t*)userData;

	switch (AUDIO_FORMAT) {
	case RTAUDIO_FLOAT32:
		{
			float *buf = (float*)outputBuffer;
			fluid_synth_write_float(synth, nBufferFrames, buf, 0, 2, buf, 1, 2);

		}
		break;
	case RTAUDIO_SINT16:
		{
			int16_t *buf = (int16_t*)outputBuffer;
			fluid_synth_write_s16(synth, nBufferFrames, (int16_t *)buf, 0, 2, (int16_t *)buf, 1, 2);

		}
		break;
	case RTAUDIO_SINT32:
		{
			int32_t *buf = (int32_t*)outputBuffer;
			fluid_synth_write_s32(synth, nBufferFrames, (int32_t *)buf, 0, 2, (int32_t *)buf, 1, 2);

		}
		break;
	}

	return 0;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
    		printf("Usage: synth file.sf2\n");
    		exit(0);
	}

	fluid_synth_t *synth;

	fluid_settings_t settings;
	fluid_synth_settings(&settings);
	settings.sample_rate = SAMPLE_RATE;
	settings.reverb      = 0;
	settings.chorus      = 0;
	settings.polyphony   = POLYPHONY;
	settings.verbose     = 0;

printf("new_fluid_synth\n");
	synth = new_fluid_synth(&settings);

//	fluid_synth_set_interp_method(synth, -1, FLUID_INTERP_LINEAR);
printf("fluid_synth_set_interp_method\n");
	fluid_synth_set_interp_method(synth, -1, FLUID_INTERP_NONE);

printf("fluid_synth_sfload\n");
	int sfont_id = fluid_synth_sfload(synth, argv[1], 1);

#define AUDIO

#ifdef AUDIO
	RtAudio dac;
	RtAudio::StreamParameters rtParams;

	// Determine the number of devices available
	unsigned int devices = dac.getDeviceCount();
	// Scan through devices for various capabilities
	RtAudio::DeviceInfo info;
	for (unsigned int i = 0; i < devices; i++) {
		info = dac.getDeviceInfo(i);
		if (info.probed == true) {
			std::cout << "device " << " = " << info.name;
			std::cout << ": maximum output channels = " << info.outputChannels << "\n";
		}
	}

	rtParams.deviceId = dac.getDefaultOutputDevice();
	rtParams.nChannels = 2;
	unsigned int bufferFrames = FRAME_SIZE;

	RtAudio::StreamOptions options;
	options.flags = RTAUDIO_SCHEDULE_REALTIME;

	dac.openStream(&rtParams, NULL, AUDIO_FORMAT, SAMPLE_RATE, &bufferFrames, &audioCallback, (void *)synth, &options);
	dac.startStream();
#endif
	int note = 0;
	int chan = 0;
	int prog = 1;
	int cnt  = 1;
//goto SKIP;

printf("PC\n");
	fluid_synth_program_change(synth, chan, prog);
	
	while (cnt--) {
		int n = 40; // + (note++ % 16);
		printf("prog %3d  note %3d\n", prog, n);

printf("ON\n");
		fluid_synth_noteon(synth, chan, n, 127);
		usleep(900000);
printf("OFF\n");
		fluid_synth_noteoff(synth, chan, n);
		usleep(50000);

		prog = (prog + 1) % 128;
	}

SKIP:
#ifdef AUDIO
	dac.stopStream();
#endif
	fluid_synth_sfunload(synth,sfont_id,1);
    	delete_fluid_synth(synth);

	return 0;
}
