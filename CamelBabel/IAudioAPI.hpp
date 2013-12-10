#ifndef IAUDIOAPI_HPP
#define IAUDIOAPI_HPP

class IAudioAPI
{
public:
  virtual       ~IAudioAPI() {}
  virtual void  start() = 0;
  virtual void  stop() = 0;
};

#endif // IAUDIOAPI_HPP
