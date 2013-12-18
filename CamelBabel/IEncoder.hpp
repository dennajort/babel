#ifndef	__IENCODER_HPP__
#define	__IENCODER_HPP__

class	IEncoder
{
public:
  virtual ~IEncoder() {}
  virtual void encode(const float *pcm) = 0;
  virtual void decode(const unsigned char *data, float *decodedData) = 0;
  virtual unsigned char *getEncodedData() const = 0;
  virtual int getEncodedSize() const = 0;
};

#endif
