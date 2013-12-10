#ifndef RTP_HPP
#define RTP_HPP

#include <cstdint>

/*
 * Realtime Transfer Protocol and extension
 */
#define RTP_LENGTH      12
#define RTPX_LENGTH     4

typedef struct    s_rtpx
{
  quint16         rtpx_profile;   // Profile specific
  quint16         rtpx_len;       // Length of extension header
}                 t_rtpx;

typedef struct    s_rtp
{
#  if __BYTE_ORDER == __LITTLE_ENDIAN
  quint8          rtp_cc:4;
  quint8          rtp_ext:1;
  quint8          rtp_pad:1;
  quint8          rtp_ver:2;

  quint8          rtp_type:7;
  quint8          rtp_marker:1;

#  elif __BYTE_ORDER == __BIG_ENDIAN
  quint8          rtp_ver:2;
  quint8          rtp_pad:1;
  quint8          rtp_ext:1;
  quint8          rtp_cc:4;

  quint8          rtp_marker:1;
  quint8          rtp_type:7;

#  else
#   error "Adjust your <bits/endian.h> defines"
#  endif

  quint16         rtp_seq;
  quint32         rtp_ts;
  quint32         rtp_ssrc;
}                 t_rtp;


#endif // RTP_HPP
