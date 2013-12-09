#ifndef	__OPUS_HPP__
#define	__OPUS_HPP__

#include "opus/opus.h"
#include "opus/opus_defines.h"
#include "opus/opus_types.h"

class	Opus
{
public:
  Opus(const int samplingRate);
  ~Opus();
  unsigned char	*encode(const short *pcm, const int frameSize);
  unsigned char	*encodeFloat(const float *pcm, const int frameSize);
  short		*decode(const unsigned char *data, const int frameSize0);
  float		*decodeFloat(const unsigned char *data, const int frameSize0);
private:
  int			_samplingRate;
  int			_maxDataBytes;
  OpusEncoder		*_encoder;
  OpusDecoder		*_decoder;
  static const int	nbChannel = 1;
  static const int	decodeFec = 0;
};

#endif
