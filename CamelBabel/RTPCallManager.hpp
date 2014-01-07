#ifndef RTPCALLMANAGER_HPP
#define RTPCALLMANAGER_HPP

#include <QObject>
#include <QUdpSocket>
#include <QMutex>
#include <queue>
#include "RTPPacket.hpp"
#include "IAudioAPI.hpp"
#include "IEncoder.hpp"

typedef std::priority_queue<RTPPacket*, std::vector<RTPPacket*>, CompareRTPPacket> RTPPacketQueue;

class RTPCallManager : public QObject
{
  Q_OBJECT

public:
  explicit  RTPCallManager(QObject *parent = 0);
  virtual   ~RTPCallManager();

  void setRtpPort(quint16 rtpPort);

public slots:
  void call(const QString &ip, quint16 port);
  void finishCall();

private slots:
  void readPendingDatagrams();

signals:
  void criticalError(const QString &error);

private:
  void initSocket();
  void processRTPDatagram(QByteArray *datagram);
  void createRTPFromAudio();
  void sendDatagram(const QByteArray &datagram);
  void handleAudio(const float *input, float *output,
                   unsigned long frameCount, double currentTime);

  QUdpSocket          *_udpSocket;

  QHostAddress        _contact;
  quint16             _contactPort;

  quint16             _packetSequence;

  RTPPacketQueue      _packetQueue;
  QMutex              _packetQueueMutex;

  IAudioAPI           *_audioAPI;
  IEncoder            *_encoder;

  quint16             _rtpPort;
};

#endif // RTPCALLMANAGER_HPP
