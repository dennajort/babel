#include "BabelPortAudio.hh"

BabelPortAudio::BabelPortAudio()
{
  PaError	err;

  err = Pa_Initialize();
  //if (err != paNoError)
  //throw BabelException(Pa_GetErrorText(err));
}

BabelPortAudio::~BabelPortAudio()
{
  PaError	err;

  err = Pa_Terminate();
  //if (err != paNoError)
  //throw BabelException(Pa_GetErrorText(err));
}

void	BabelPortAudio::record()
{
  PaError	err;

  err = Pa_OpenDefaultStream(&_stream,
			     2, 2,
			     paFloat32,
			     SAMPLE_RATE,
			     256,
			     myPaCallback,
			     this);
  //if (err != paNoError)
  //throw BabelException(Pa_GetErrorText(err));
  Pa_StartStream(_stream);
  Pa_Sleep(10000);
  Pa_StopStream(_stream);
  Pa_CloseStream(_stream);
}

int	BabelPortAudio::myCallback(const void * input, void * output,
		   unsigned long frameCount,
		   const PaStreamCallbackTimeInfo * timeInfo,
		   PaStreamCallbackFlags statusFlags)
{
  const float * in = (const float *)input;
  float * out = (float *)output;
  unsigned int	i;
  float	leftInput, rightInput;

  // encode speed
  for (i = 0; i < frameCount; i++)
    {
      leftInput = *in++;
      rightInput = *in++;
      *out++ = leftInput * rightInput; // left out (leftInput * rightInput)??
      *out++ = 0.5f * (leftInput * rightInput); // right out (0.5f * (leftInput * rightInput))??
    }

  return (paContinue);
}
