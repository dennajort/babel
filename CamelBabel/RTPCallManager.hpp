#ifndef RTPCALLMANAGER_HPP
#define RTPCALLMANAGER_HPP

#include <QObject>
#include <QUdpSocket>
#include <QMutex>
#include <queue>
#include "RTPPacket.hpp"
#include "IAudioAPI.hpp"
#include "IEncoder.hpp"

typedef std::priority_queue<RTPPacket*> RTPPacketQueue;

class RTPCallManager : public QObject
{
  Q_OBJECT
public:
  explicit  RTPCallManager(QObject *parent = 0);
  virtual   ~RTPCallManager();

signals:

public slots:
  void sendDatagram(const QByteArray &datagram);
  void call();
  void finishCall();

private slots:
  void readPendingDatagrams();

private:
  void initSocket();
  void processRTPDatagram(QByteArray *datagram);
  void createRTPFromAudio();
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
};

#endif // RTPCALLMANAGER_HPP
