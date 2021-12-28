//
// File: UDPframebroadcast.cpp
//
// Revision history:
// 18-Nov-2009 Initial version.
// 21-Dec-2021 Transfer frame length in bytes instead of ints.
//
// Description:
// Very simple utilities to send and receive via UDP over IP(v4) multicast 
// video frames (actually arrays of unsigned ints...)
//

#include "UDPframebroadcast.h"

UDPframeSender::UDPframeSender(const std::string &dest_address, unsigned short dest_port)
{
  m_good = false;

  m_sfd = ::socket(PF_INET,SOCK_DGRAM, 0);

  if (m_sfd < 0)
   {
    std::ostringstream message;
    message << "Error creating socket: " << ::strerror(errno);
    m_error_string = message.str();
    return;
   }

  struct sockaddr_in dest;

  unsigned char multicast_loop = 1;
  ::setsockopt(m_sfd, IPPROTO_IP, IP_MULTICAST_LOOP, 
               &multicast_loop, sizeof(multicast_loop));

  dest.sin_family = AF_INET;
  dest.sin_port = htons(dest_port);
  if (::inet_aton(dest_address.c_str(), &dest.sin_addr) < 0)
   {
    ::close(m_sfd);
    m_sfd = -1;
    std::ostringstream message;
    message << "Error converting address string '" <<  dest_address
                   << "' : " << ::strerror(errno);
    m_error_string = message.str();
    return;
   }

  if (::connect(m_sfd, (struct sockaddr *)&dest, sizeof(dest)) < 0)
   {
    ::close(m_sfd);
    m_sfd = -1;
    std::ostringstream message;
    message << "Error connecting datagram socket: " << ::strerror(errno);
    m_error_string = message.str();
    return;
   }

  int ttl=1;
  if (::setsockopt(m_sfd, IPPROTO_IP, IP_MULTICAST_TTL,
                   &ttl, sizeof(ttl)) < 0)
   {
    ::close(m_sfd);
    m_sfd = -1;
    std::ostringstream message;
    message << "Error setting multicast TTL to " << ttl 
            << ": " << ::strerror(errno);
    m_error_string = message.str();
    return;
   }

  m_good = true;
}
  
UDPframeSender::~UDPframeSender()
{
  if (m_sfd >= 0) ::close(m_sfd);
}

bool UDPframeSender::good()
{
  return m_good;
}

bool UDPframeSender::send_frame(unsigned int timestamp_sec,
                                unsigned int timestamp_usec,
                                const unsigned int *data, unsigned int width,
                                                          unsigned int height,
                                                          unsigned int blen)
{
  if (!m_good) return false;
  ssize_t ret;
  unsigned int sent_len = 0;
  unsigned int length = width * height;

  // Send start of frame packet.
  UDPframeStart mstart;
  mstart.start_magic = _UDPframebroadcast_h_FRAME_START_MAGIC;
  mstart.timestamp_sec = timestamp_sec;
  mstart.timestamp_usec = timestamp_usec;
  mstart.width = width;
  mstart.height = height;
  mstart.byte_len = blen;
  if ((ret = ::send(m_sfd, &mstart, sizeof(mstart), 0)) < static_cast<int>(sizeof(mstart)))
   {
    std::ostringstream message;
    message << "Error sending " << sizeof(mstart) << " bytes to datagram socket. "
            << "Send returns " << ret << " : " << ::strerror(errno);
    m_error_string = message.str();
    return false;
   }

  UDPframePacket msg;
  unsigned int max_msg_length = sizeof(msg.data)/sizeof(unsigned int);

  while (sent_len < length)
   {
    msg.offset = sent_len;
    unsigned int this_len = length - sent_len + 2;
    if (this_len > max_msg_length) this_len = max_msg_length;
    msg.size = this_len - 2;
    ::memcpy(msg.data, &data[sent_len], msg.size * sizeof(unsigned int));
    sent_len += msg.size;

    ssize_t len_in_bytes = sizeof(msg);
    if ((ret = ::send(m_sfd, &msg, len_in_bytes, 0)) < len_in_bytes)
     {
      std::ostringstream message;
      message << "Error sending " << len_in_bytes << " bytes to datagram socket. "
              << "Send returns " << ret << " : " << ::strerror(errno);
      m_error_string = message.str();
      return false;
     }
   }
  return true;
}

const std::string& UDPframeSender::get_error() const
{
  return m_error_string;
}


UDPframeReceiver::UDPframeReceiver(const std::string &listen_address, unsigned short listen_port)
{
  m_good = false;
  m_data = 0;
  m_data_length = 0;
  m_last_byte_len = 0;

  m_sfd = ::socket(PF_INET,SOCK_DGRAM, 0);

  if (m_sfd < 0)
   {
    std::ostringstream message;
    message << "Error creating socket: " << ::strerror(errno);
    m_error_string = message.str();
    return;
   }

  struct sockaddr_in src;

  src.sin_family = AF_INET;
  src.sin_port = htons(listen_port);
  if (::inet_aton(listen_address.c_str(), &src.sin_addr) < 0)
   {
    ::close(m_sfd);
    m_sfd = -1;
    std::ostringstream message;
    message << "Error converting address string '" << listen_address
            << "' : " << ::strerror(errno);
    m_error_string = message.str();
    return;
   }

  ::memcpy(&m_mreq.imr_multiaddr.s_addr, &src.sin_addr, sizeof(struct in_addr));
  m_mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  setsockopt(m_sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
             &m_mreq, sizeof(m_mreq));
  if (::bind(m_sfd, (struct sockaddr *)&src, sizeof(src)) < 0)
   {
    ::close(m_sfd);
    m_sfd = -1;
    std::ostringstream message;
    message << "Error binding datagram socket: " << ::strerror(errno);
    m_error_string = message.str();
    return;
   }

  m_good = true;
}

UDPframeReceiver::~UDPframeReceiver()
{
  
  if (m_sfd >= 0)
   {
    setsockopt(m_sfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, 
               &m_mreq, sizeof(m_mreq));
    ::close(m_sfd);
   }
  if (m_data != 0) delete[] m_data;
}

bool UDPframeReceiver::good()
{
  return m_good;
}

bool UDPframeReceiver::receive_frame(unsigned int &timestamp_sec,
                                     unsigned int &timestamp_usec,
                                     unsigned int **data, 
                                     unsigned int &width, unsigned int &height)
{
  if (!m_good) return false;
  if (data == 0) return false;
  *data = 0;

  // Grab the first start of frame packet
  ssize_t ret;
  bool start_found = false;
  UDPframeStart mstart;

  while (!start_found)
   {
    ret = ::recv(m_sfd, &mstart, sizeof(mstart), 0);

    if (ret < 0)
     {
      std::ostringstream message;
      message << "Error receiving " << sizeof(mstart) << " ints from datagram socket. "
              << "Recv returns " << ret << " : " << ::strerror(errno);
      m_error_string = message.str();
      return false;
     }

    if (mstart.start_magic == _UDPframebroadcast_h_FRAME_START_MAGIC)
     {
      width  = mstart.width;
      height = mstart.height;
      m_last_byte_len = mstart.byte_len;
      timestamp_sec = mstart.timestamp_sec;
      timestamp_usec = mstart.timestamp_usec;
      start_found = true;
     } 
   }

  unsigned int frame_len = width*height;

  if (frame_len > m_data_length)
   {
    if (m_data != 0) delete[] m_data; 
    m_data = new unsigned int[frame_len];
    if (m_data == 0) return false;
    m_data_length = frame_len;
   }

  UDPframePacket msg;

  while(1) // Will exit with break on end-of-frame.
   {
    ret = ::recv(m_sfd, &msg, sizeof(msg), 0);

    if (ret < 0)
     {
      std::ostringstream message;
      message << "Error receiving " << sizeof(msg) << " ints from datagram socket. "
              << "Recv returns " << ret << " : " << ::strerror(errno);
      m_error_string = message.str();
      return false;
     }

    if (msg.offset+msg.size > frame_len) continue; // Discard packet.
    ::memcpy(&m_data[msg.offset], msg.data, msg.size*sizeof(unsigned int));
    if ((msg.offset+msg.size) == frame_len) break; // End of frame.
   }

  *data = m_data;
  return true;
}

unsigned int * UDPframeReceiver::get_frame()
{
  return m_data;
}

const std::string& UDPframeReceiver::get_error() const
{
  return m_error_string;
}
