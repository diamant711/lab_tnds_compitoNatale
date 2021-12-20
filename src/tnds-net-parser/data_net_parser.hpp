#include "UDPframebroadcast/UDPframebroadcast.h"

#include <string>
#include <fstream>
#include <system_error>
#include <iostream>

#include <cstdio> //sprintf()

#define max_char_file_name 20

class data_net_parser {
  public:
    enum data_type {ACCELEROMETER = 'a', CAMERA = 'i'};
    
    data_net_parser(data_type, UDPframeReceiver &);
    ~data_net_parser();
    
    // recive, checksum, store
    bool receive_and_save_frame();

    //expose some internal state for logging?

  private:
    UDPframeReceiver m_udp;
    char m_file_path[max_char_file_name];
    std::ofstream m_current_file;
    int m_count_written_file = 0;
    long m_count_written_frame_x_current_file = 0;
    const long m_frame_x_file = 1000;
    unsigned int *m_tmp_frame;
    unsigned int m_tmp_width;
    unsigned int m_tmp_height;
    unsigned int m_tmp_tsec;
    unsigned int m_tmp_tusec;
    const int m_packet_size = 11;
    data_type m_data_type;

    void file_check();
};

data_net_parser::data_net_parser(data_type type, UDPframeReceiver &udp) {
  m_udp = udp;
  m_data_type = type;
  ::sprintf(m_file_path, "data/%c/%d.fdt", m_data_type, m_count_written_file);
  m_current_file.open(m_file_path);
  if (!m_current_file.good()) {
    std::error_code err_code(errno, std::system_category());
    std::cerr << "Error opening \"" << m_file_path << "\" with error: "
              << err_code.message() << std::endl;
    exit(errno);
  }
}

data_net_parser::~data_net_parser() {
  m_current_file.close();
}

void data_net_parser::file_check(){
  if (m_count_written_frame_x_current_file < m_frame_x_file)
    return;
  else {
    m_current_file.close();
    ++m_count_written_file;
    ::sprintf(m_file_path, "data/%c/%d.fdt", m_data_type, m_count_written_file);
    m_current_file.open(m_file_path);
    return;
  }
}

bool data_net_parser::receive_and_save_frame() {
  if (m_udp.good()) {
    file_check();
    m_udp.receive_frame(m_tmp_tsec, m_tmp_tusec, &m_tmp_frame, m_tmp_width, m_tmp_height);
    switch(m_data_type) {
      case ACCELEROMETER:
        { 
          unsigned char packet[m_packet_size];
          unsigned char partial_SUM = 0;
          for(int j = 0; j < m_packet_size; ++j) {
            packet[j] = (*m_tmp_frame << j) & 0b11; //rischioso da verificare
            if(j < (m_packet_size - 1))
              partial_SUM = partial_SUM | packet[j];
          }
          if(partial_SUM == packet[m_packet_size - 1])
            return false;
        }
      break;
      case CAMERA:
        
      break;
    }
    m_current_file.write(reinterpret_cast<char *>(m_tmp_frame), sizeof(m_tmp_frame));
    return true;
  } else {
    return false;
  }
}
