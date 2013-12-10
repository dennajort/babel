#include <exception>
#include <iostream>
#include "Opus.hpp"

Opus::Opus(const int samplingRate, const int frameSize)
  : _samplingRate(samplingRate),
    _maxDataBytes(samplingRate * 1024),
    _frameSize(frameSize)
{
  int	encoderError;
  int	decoderError;

  _encoder = opus_encoder_create(_samplingRate, nbChannel, OPUS_APPLICATION_VOIP, &encoderError);
  _decoder = opus_decoder_create(_samplingRate, nbChannel, &decoderError);
  if (encoderError != OPUS_OK || decoderError != OPUS_OK)
    {
      std::cout << "encoder = " << encoderError << std::endl;
      std::cout << "decoder = " << decoderError << std::endl;
      throw BabelException("Opus creation error");
    }
  opus_encoder_ctl(_encoder, OPUS_GET_BANDWIDTH(&_maxDataBytes));
  _compressedData = new unsigned char[_maxDataBytes];
}

Opus::~Opus()
{
  opus_encoder_destroy(_encoder);
  opus_decoder_destroy(_decoder);
  delete _compressedData;
}

void Opus::encode(const float *pcm)
{
  int error;

  if ((error = opus_encode_float(_encoder, pcm, _frameSize, _compressedData, _maxDataBytes)) < 0)
    {
      std::cout << "encode error = " << error << std::endl;
      throw BabelException("Opus encode error");
    }
}

void Opus::decode(const unsigned char *data, float *decodedData)
{
  int error;

  if ((error = opus_decode_float(_decoder, data, _maxDataBytes, decodedData, _frameSize, decodeFec)) < 0)
    {
      std::cout << "decode error = " << error << std::endl;
      throw BabelException("Opus decode error");
    }
}

unsigned char *Opus::getEncodedData() const
{
  return (_compressedData);
}
