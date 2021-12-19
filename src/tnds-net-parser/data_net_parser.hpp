#include "UDPframebroadcast/UDPframebroadcast.h"

#include <string>
#include <fstream>
#include <system_error>
#include <iostream>

#include <cstdio> //sprintf()

#define max_char_file_name 20

class data_net_parser : UDPframeReceiver {
  public:
    data_net_parser();
    ~data_net_parser();
    
    // recive, checksum, store
    bool recive_and_save_frame();

    //expose some internal state for logging?

  private:
    char m_file_path[max_char_file_name];
    std::ofstream m_current_file;
    int m_count_written_file = 0;
    long m_count_written_frame_x_current_file = 0;
    const long m_frame_x_file = 100;

    bool current_file_full();
};

data_net_parser::data_net_parser() { 
  ::sprintf(m_file_path, "data/%d.fdt", m_count_written_file);
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

bool data_net_parser::current_file_full(){
  if (m_count_written_frame_x_current_file < m_frame_x_file)
    return false;
  else {
    m_current_file.close();
    ::sprintf(m_file_path, "data/%d.fdt", m_count_written_file);
    m_current_file.open(m_file_path);
    return true;
  }
}
