#ifndef	BABELPORTAUDIO_HPP
#define	BABELPORTAUDIO_HPP

#include <portaudio.h>
#include "IAudioAPI.hpp"
#include "BabelException.hpp"

template <typename T>
class PortAudio : public IAudioAPI
{
public:
  PortAudio(double sampleRate, unsigned long framesPerBuffer, T t)
    : _stream(nullptr),
      _t(t)
  {
    PaError	err;
    err = Pa_Initialize();
    if (err != paNoError)
      throw BabelException(Pa_GetErrorText(err));
    err = Pa_OpenDefaultStream(&_stream, 1, 1, paFloat32, sampleRate,
                               framesPerBuffer, &PortAudio::callback, this);
    if (err != paNoError)
      throw BabelException(Pa_GetErrorText(err));
  }

  PortAudio(const PortAudio &pa);

  ~PortAudio()
  {
    PaError	err;

    if (_stream != nullptr)
      Pa_CloseStream(_stream);
    err = Pa_Terminate();
    if (err != paNoError)
      throw BabelException(Pa_GetErrorText(err));
  }

  void start()
  {
    if (_stream == nullptr)
      throw BabelException("No stream open");
    Pa_StartStream(_stream);
  }

  void stop()
  {
    if (_stream == nullptr)
      throw BabelException("No stream open");
    Pa_StopStream(_stream);
  }

private:
  static int callback(const void *input, void *output,
		      unsigned long frameCount,
		      const PaStreamCallbackTimeInfo *timeInfo,
		      PaStreamCallbackFlags statusFlags,
		      void *userData)
  {
    PortAudio	*pThis = reinterpret_cast<PortAudio*>(userData);

    (void)statusFlags;
    pThis->_t(reinterpret_cast<const float *>(input),
	      reinterpret_cast<float *>(output),
	      frameCount, timeInfo->currentTime);
    return (paContinue);
  }

  PaStream      *_stream;
  T             _t;
};

template <typename T>
PortAudio<T> *createPortAudio(double sampleRate, unsigned long framesPerBuffer, const T &t)
{
  return (new PortAudio<T>(sampleRate, framesPerBuffer, t));
}

#endif // PORTAUDIO_HPP
