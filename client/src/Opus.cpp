#include <exception>
#include "Opus.hpp"

Opus::Opus(const int samplingRate)
  : _samplingRate(samplingRate),
    _maxDataBytes(samplingRate * 1024)
{
  int	encoderError;
  int	decoderError;

  _encoder = opus_encoder_create(_samplingRate, nbChannel, OPUS_APPLICATION_VOIP, &encoderError);
  _decoder = opus_decoder_create(_samplingRate, nbChannel, &decoderError);
  if (encoderError != OPUS_OK || decoderError != OPUS_OK)
    throw std::exception();
  opus_encoder_ctl(_encoder, OPUS_GET_BANDWIDTH(&_maxDataBytes));
}

Opus::~Opus()
{
  opus_encoder_destroy(_encoder);
  opus_decoder_destroy(_decoder);
}

unsigned char *Opus::encode(const short *pcm, const int frameSize)
{
  unsigned char	*compressedData = new unsigned char[_maxDataBytes];

  if (opus_encode(_encoder, pcm, frameSize, compressedData, _maxDataBytes) < 1)
    throw std::exception();
  return (compressedData);
}

unsigned char *Opus::encodeFloat(const float *pcm, const int frameSize)
{
  unsigned char	*compressedData = new unsigned char[_maxDataBytes];

  if (opus_encode_float(_encoder, pcm, frameSize, compressedData, _maxDataBytes) < 1)
    throw std::exception();
  return (compressedData);
}

short *Opus::decode(const unsigned char *data, const int frameSize)
{
  short *decodedData = new short[frameSize * opus_packet_get_nb_channels(data)];

  if (opus_decode(_decoder, data, _maxDataBytes, decodedData, frameSize, decodeFec) < 1)
    throw std::exception();
  return (decodedData);
}

float *Opus::decodeFloat(const unsigned char *data, const int frameSize)
{
  float *decodedData = new float[frameSize * opus_packet_get_nb_channels(data)];

  if (opus_decode_float(_decoder, data, _maxDataBytes, decodedData, frameSize, decodeFec) < 1)
    throw std::exception();
  return (decodedData);
}
