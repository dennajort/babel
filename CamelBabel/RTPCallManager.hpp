#ifndef RTPCALLMANAGER_HPP
#define RTPCALLMANAGER_HPP

#include <QObject>
#include <QUdpSocket>
#include "PortAudio.hpp"

class RTPCallManager : public QObject
{
  Q_OBJECT
public:
  explicit RTPCallManager(QObject *parent = 0);
  
signals:
  
public slots:
  void sendDatagram(const QByteArray &datagram);
  void call();
  void finishCall();
  void resumePlaying();

private slots:
  void readPendingDatagrams();

private:
  void initSocket();
  void processRTPDatagram(QByteArray &datagram);
  void startConversation();
  void stopConversation();
  void handleAudio(const float *input, float *output,
                   unsigned long frameCount,
                   const PaStreamCallbackTimeInfo *timeInfo,
                   PaStreamCallbackFlags statusFlags);

  QUdpSocket          *_udpSocket;

  QHostAddress        _contact;
  quint16             _contactPort;
};

#endif // RTPCALLMANAGER_HPP
