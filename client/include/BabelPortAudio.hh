#ifndef	_BABELPORTAUDIO_HH_
#define	_BABELPORTAUDIO_HH_

#include "portaudio.h"

#define SAMPLE_RATE (44100)

class	BabelPortAudio
{
public:
  BabelPortAudio();
  ~BabelPortAudio();
  int	myCallback(const void * input, void * output,
		   unsigned long frameCount,
		   const PaStreamCallbackTimeInfo * timeInfo,
		   PaStreamCallbackFlags statusFlags);

  static int	myPaCallback(const void * input, void * output,
			     unsigned long frameCount,
			     const PaStreamCallbackTimeInfo * timeInfo,
			     PaStreamCallbackFlags statusFlags,
			     void * userData)
  {
    return ((BabelPortAudio*)userData)->
      myCallback(input, output, frameCount, timeInfo, statusFlags);
  }

  void	record();

private:
  PaStream	*_stream;
};

#endif
