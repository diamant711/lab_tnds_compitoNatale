//
// File: UDPframebroadcast.h
//
// Revision history:
// 18-Nov-2009 Initial version.
//
// Description:
// Very simple utilities to send and receive via UDP over IP(v4) multicast 
// video frames (actually arrays of unsigned ints...)
//

#ifndef _UDPframebroadcast_h_included
#define _UDPframebroadcast_h_included

#include <string>
#include <sstream>

// Need socket library.

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cerrno>
#include <stdint.h> // cstdint only in c++0x
#include <cstring> // strerror()

// The multicast address is taken out of the GLOP (RFC3180) /24 space
// for Autonomous System 64516.

#define _UDPframebroadcast_h_DEFAULT_MCAST_ADDR "233.252.4.17"
#define _UDPframebroadcast_h_DEFAULT_PORT       20017
#define _UDPframebroadcast_h_MAX_MSG_LEN        350
#define _UDPframebroadcast_h_FRAME_START_MAGIC  0xAF00AF00

struct UDPframeStart
{
  uint32_t start_magic;
  uint32_t timestamp_sec;
  uint32_t timestamp_usec;
  uint32_t width;
  uint32_t height;
  uint32_t ballast[_UDPframebroadcast_h_MAX_MSG_LEN-5];
};

struct UDPframePacket
{
  uint32_t offset;
  uint32_t size;
  uint32_t data[_UDPframebroadcast_h_MAX_MSG_LEN-2];
};

class UDPframeSender
{
  public:

    // Constructor.
    UDPframeSender(const std::string &dest_address = std::string(_UDPframebroadcast_h_DEFAULT_MCAST_ADDR),
                   unsigned short dest_port = _UDPframebroadcast_h_DEFAULT_PORT);
  
    // Destructor.
    ~UDPframeSender();

    bool good();

    bool send_frame(unsigned int timestamp_sec,
                    unsigned int timestamp_usec,
                    const unsigned int *data, 
                    unsigned int width, unsigned int height);

    const std::string &get_error() const;

  private:

    int m_sfd;
    bool m_good;
    std::string m_error_string;
};

class UDPframeReceiver
{
  public:

    // Constructor.
    UDPframeReceiver(const std::string &listen_address = std::string(_UDPframebroadcast_h_DEFAULT_MCAST_ADDR),
                     unsigned short listen_port = _UDPframebroadcast_h_DEFAULT_PORT);
  
    // Destructor.
    ~UDPframeReceiver();

    bool good();

    bool receive_frame(unsigned int &timestamp_sec,
                       unsigned int &timestamp_usec,
                       unsigned int **data,
                       unsigned int &width, unsigned &height);

    unsigned int *get_frame();

    const std::string &get_error() const;

  private:

    int m_sfd;
    bool m_good;
    std::string m_error_string;
    unsigned int *m_data;
    unsigned int m_data_length;
    struct ip_mreq m_mreq;
};

#endif /* defined _UDPframebroadcast_h_included */
