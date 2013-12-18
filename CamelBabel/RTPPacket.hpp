#ifndef RTPPACKET_HPP
#define RTPPACKET_HPP

#include <QByteArray>
#include <QtEndian>
#include "rtp.hpp"

class RTPPacket
{
public:
  RTPPacket(QByteArray *datagram, size_t rtpHeaderLength)
    : _datagram(datagram),
      _rtp(reinterpret_cast<t_rtp *>(datagram->data())),
      _payload(reinterpret_cast<unsigned char *>(&datagram->data()[rtpHeaderLength]))
  {
  }

  ~RTPPacket()
  {
    delete _datagram;
  }

  const unsigned char *getPayload() const {return (_payload);}
  quint16 getSequence() const {return (qFromBigEndian(_rtp->rtp_ts));}

private:
  QByteArray      *_datagram;
  t_rtp           *_rtp;
  unsigned char   *_payload;
};

#endif // RTPPACKET_HPP
