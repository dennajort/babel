#ifndef	__OPUS_HPP__
#define	__OPUS_HPP__

#include "opus/opus.h"
#include "opus/opus_defines.h"
#include "opus/opus_types.h"
#include "IEncoder.hpp"
#include "BabelException.hpp"

class	Opus : public IEncoder
{
public:
  Opus(const int samplingRate, const int frameSize);
  ~Opus();
  void	encode(const float *pcm);
  void 	decode(const unsigned char *data, float *decodedData);
  unsigned char *getEncodedData() const;
  int getEncodedSize() const;
private:
  int			_samplingRate;
  int			_maxDataBytes;
  int			_frameSize;
  unsigned char		*_compressedData;
  OpusEncoder		*_encoder;
  OpusDecoder		*_decoder;
  static const int	nbChannel = 1;
  static const int	decodeFec = 0;
};

#endif
