/* complete linux synthesizer using RtAudio and RtMidi */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>

#include "LightFluidSynth.hpp"

#include "RtAudio.h"

//#define AUDIO_FORMAT RTAUDIO_FLOAT32
#define AUDIO_FORMAT RTAUDIO_SINT16

#define FRAME_SIZE 128

using namespace std;

{
}

int audioCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                   double streamTime, RtAudioStreamStatus status, void *userData )
{
  LightFluidSynth *synth = (LightFluidSynth *)userData;

  switch (AUDIO_FORMAT) {
  case RTAUDIO_FLOAT32:
  {
    float *buf = (float *)outputBuffer;
    synth->writeStereoFloat(buf, nBufferFrames);

  }
  break;
  case RTAUDIO_SINT16:
  {
    int16_t *buf = (int16_t *)outputBuffer;
    synth->writeStereoS16((short *)buf, nBufferFrames);
  }
  break;
  case RTAUDIO_SINT32:
  {
    int32_t *buf = (int32_t *)outputBuffer;
    synth->writeStereoS32((int *)buf, nBufferFrames);

  }
  break;
  }

  return 0;
}

int main(int argc, char** argv)
{

  if (argc != 2) {
    printf("Usage: synth file.sf2\n");
    exit(0);
  }

  LightFluidSynth *usynth;

  usynth = new LightFluidSynth();

  usynth->loadSF2(argv[1]);
//  usynth->loadSF2("tim.sf2");

//   RtAudio dac(RtAudio::LINUX_PULSE);
  RtAudio dac;
  RtAudio::StreamParameters rtParams;

  // Determine the number of devices available
  unsigned int devices = dac.getDeviceCount();
  // Scan through devices for various capabilities
  RtAudio::DeviceInfo info;
  for ( unsigned int i = 0; i < devices; i++ ) {
    info = dac.getDeviceInfo( i );
    if ( info.probed == true ) {
      std::cout << "device " << " = " << info.name;
      std::cout << ": maximum output channels = " << info.outputChannels << "\n";
    }
  }
//  rtParams.deviceId = 3;
  rtParams.deviceId = dac.getDefaultOutputDevice();
  rtParams.nChannels = 2;
  unsigned int bufferFrames = FRAME_SIZE;

  RtAudio::StreamOptions options;
  options.flags = RTAUDIO_SCHEDULE_REALTIME;

  dac.openStream( &rtParams, NULL, AUDIO_FORMAT, SAMPLE_RATE, &bufferFrames, &audioCallback, (void *)usynth, &options );
  dac.startStream();

  printf("\n\nPress Enter to stop\n\n");
  cin.get();
  dac.stopStream();

  delete(usynth);
  return 0;
}