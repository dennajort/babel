#include <QSettings>
#include <QtEndian>
#include <functional>
#include <iostream>
#include "RTPCallManager.hpp"
#include "PortAudio.hpp"
#include "Opus.hpp"
#include "rtp.hpp"

RTPCallManager::RTPCallManager(QObject *parent) :
  QObject(parent),
  _contact(QHostAddress::Any),
  _contactPort(4242)
{
  _audioAPI = createPortAudio(24000, 240,
                              std::bind(&RTPCallManager::handleAudio, this,
                                        std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3, std::placeholders::_4));
  _encoder = new Opus(24000, 240);
  initSocket();
  QSettings settings;
  settings.beginGroup("account");
  _contact = settings.value("server", "127.0.0.1").toString();
  _contactPort = settings.value("port", "4242").toString().toUInt();
  settings.endGroup();
}

RTPCallManager::~RTPCallManager()
{
  delete _audioAPI;
//  _packetQueueMutex.lock();
//  while (_packetQueue)
//  _packetQueueMutex.unlock();
}

void RTPCallManager::sendDatagram(const QByteArray &datagram)
{
  _udpSocket->writeDatagram(datagram.data(), datagram.size(),
                            _contact, _contactPort);
}

void RTPCallManager::call()
{
  _audioAPI->start();
}

void RTPCallManager::finishCall()
{
  _audioAPI->stop();
}

void RTPCallManager::readPendingDatagrams()
{
    while (_udpSocket->hasPendingDatagrams())
      {
        QByteArray      *datagram = new QByteArray;
        QHostAddress    sender;
        quint16         senderPort;

        datagram->resize(_udpSocket->pendingDatagramSize());
        _udpSocket->readDatagram(datagram->data(), datagram->size(),
                                 &sender, &senderPort);
        if (sender == _contact && senderPort == _contactPort)
          processRTPDatagram(datagram);
        else
          delete datagram;
      }
}

void RTPCallManager::initSocket()
{
    _udpSocket = new QUdpSocket(this);
    _udpSocket->bind(QHostAddress::Any, 4242);
    connect(_udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
}

void RTPCallManager::processRTPDatagram(QByteArray *datagram)
{
  t_rtp         *rtp = reinterpret_cast<t_rtp*>(datagram->data());
  size_t        rtpHeaderLength;

  qDebug() << "-- Start Processing datagram";
  qDebug() << "size: " << datagram->size() << " >= " << sizeof(rtp)
           << "rtp_ver = " << rtp->rtp_ver
           << "rtp_cc = " << rtp->rtp_cc
           << "rtp_ts = " << qFromBigEndian(rtp->rtp_ts)
           << "rtp_type = " << rtp->rtp_type;
  if (static_cast<size_t>(datagram->size()) < sizeof(*rtp)
      || rtp->rtp_ver != 2
      || rtp->rtp_cc > 15
      || qFromBigEndian(rtp->rtp_ts) == 0
      || rtp->rtp_type > 34)
    {
      delete datagram;
      return;
    }
  rtpHeaderLength = RTP_LENGTH + rtp->rtp_cc * 4;
  if (rtp->rtp_ext)
    {
      t_rtpx *rtpx = (t_rtpx *)(datagram->data() + rtpHeaderLength);
      rtpHeaderLength += (qFromBigEndian(rtpx->rtpx_len) * 4) + RTPX_LENGTH;
    }
  if (rtp->rtp_pad)
    datagram->truncate(datagram->data()[datagram->size() - 1]);
  qDebug() << "-- End Processing datagram";
  _packetQueueMutex.lock();
  _packetQueue.push_back(new RTPPacket(datagram, rtpHeaderLength));
  _packetQueueMutex.unlock();
}

void RTPCallManager::createRTPFromAudio()
{
  t_rtp         rtp;

  rtp.rtp_ver = 2;
  rtp.rtp_pad = 0;
  rtp.rtp_ext = 0;
  rtp.rtp_cc = 0;
  rtp.rtp_marker = 0;
  rtp.rtp_type = 0;
  rtp.rtp_seq = qToBigEndian<quint16>(1);
  rtp.rtp_ts = qToBigEndian<quint32>(1);
  rtp.rtp_ssrc = qToBigEndian<quint32>(42);
  QByteArray    packet(reinterpret_cast<const char *>(&rtp), RTP_LENGTH);
  packet.append(reinterpret_cast<const char *>(_encoder->getEncodedData()),
                _encoder->getEncodedSize());
  sendDatagram(packet);
}

void RTPCallManager::handleAudio(const float *input, float *output,
                                 unsigned long frameCount, double currentTime)
{
  Q_UNUSED(frameCount);
  Q_UNUSED(currentTime);
  _encoder->encode(input);
  createRTPFromAudio();
  _packetQueueMutex.lock();
  if (!_packetQueue.isEmpty())
    {
      qDebug() << "packetQueue size = " << _packetQueue.size();
      RTPPacket *packet = _packetQueue.front();
      _packetQueue.pop_front();
      _encoder->decode(packet->getPayload(), packet->getPayloadSize(), output);
      delete packet;
    }
  _packetQueueMutex.unlock();
}
