#include <exception>
#include <iostream>
#include "Opus.hpp"

Opus::Opus(const int samplingRate, const int frameSize)
  : _samplingRate(samplingRate),
    _maxDataBytes(samplingRate * 1024),
    _frameSize(frameSize),
    _compressedSize(0)
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
  if ((_compressedSize = opus_encode_float(_encoder, pcm, _frameSize, _compressedData, _maxDataBytes)) < 0)
    throw BabelException(opus_strerror(_compressedSize));
}

void Opus::decode(const unsigned char *data, int len, float *decodedData)
{
  int error;

  if ((error = opus_decode_float(_decoder, data, len, decodedData, _frameSize, decodeFec)) < 0)
    throw BabelException(opus_strerror(error));
}

unsigned char *Opus::getEncodedData() const
{
  return (_compressedData);
}

int Opus::getEncodedSize() const
{
  return (_compressedSize);
}
