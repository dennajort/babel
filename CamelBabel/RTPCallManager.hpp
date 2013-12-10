#ifndef RTPCALLMANAGER_HPP
#define RTPCALLMANAGER_HPP

#include <QObject>
#include <QUdpSocket>
#include "IAudioAPI.hpp"
#include "IEncoder.hpp"

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
  void processRTPDatagram(QByteArray &datagram);
  void handleAudio(const float *input, float *output,
                   unsigned long frameCount, double currentTime);

  QUdpSocket          *_udpSocket;

  QHostAddress        _contact;
  quint16             _contactPort;

  IAudioAPI           *_audioAPI;
  IEncoder            *_encoder;
};

#endif // RTPCALLMANAGER_HPP
