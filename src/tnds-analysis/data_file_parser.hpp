#include "common_type.hpp"

#include <string>
#include <fstream>

#include <cstdio> //sprintf

class data_file_parser {
  public:
    data_file_parser(data &);
    ~data_file_parser();
    
    /*
     *  check m_ready_to_read
     *  read metadata
     *  cicle to read *.fdt // <--- std::istream::read()
     *                      // unsigned int n;
     *                      // read(reinterpret_cast<char*>(&n), sizeof n)
     *  and store into data m_raw
    */
    bool fill_raw();
   
  private:
    data m_data;
    const unsigned int max_len_file_name = 20;
    std::string m_current_file_path;
    std::ifstream m_current_file;
    bool m_ready_to_read = false;
    unsigned int m_tot_number_of_file;
};

data_file_parser::data_file_parser(data &dat) : m_data(dat) {}

data_file_parser::~data_file_parser() {}

bool data_file_parser::fill_raw() {
  int file_to_read = 1;
  char tmp[max_len_file_name];
  /* ---------error on the saved metadata file------------------- 
  ::sprintf(tmp, "data/%c/metadata", m_data.GetType());
  std::ifstream metadata(tmp);
  metadata >> file_to_read;
  metadata.close();
  */
  for(int i = 0; i < file_to_read; i++){
    ::sprintf(tmp, "data/%c/%d.fdt", m_data.GetType(), i);
    m_current_file_path = tmp;
    m_current_file.open(m_current_file_path);
    while(!m_current_file.eof()){
      uint32_t magic_header = 0;
      while (magic_header != 0xe1e01cca) {
        m_current_file.read(reinterpret_cast<char *>(&magic_header), 
                                               sizeof(magic_header));
      }
      data::frame_data tmp_frame_data;
      m_current_file.read(reinterpret_cast<char *>(&tmp_frame_data.tsec), \
                                             sizeof(tmp_frame_data.tsec));
      m_current_file.read(reinterpret_cast<char *>(&tmp_frame_data.tusec), \
                                             sizeof(tmp_frame_data.tusec));
      m_current_file.read(reinterpret_cast<char *>(&tmp_frame_data.width), \
                                             sizeof(tmp_frame_data.width));
      m_current_file.read(reinterpret_cast<char *>(&tmp_frame_data.height), \
                                             sizeof(tmp_frame_data.height));
      uint32_t bytes; //<--------- !!!!!
      m_current_file.read(reinterpret_cast<char *>(&bytes), sizeof(bytes));
      m_current_file.read(reinterpret_cast<char *>(&tmp_frame_data.frame[0]), \
                              tmp_frame_data.width * tmp_frame_data.height *  \
                                             sizeof(tmp_frame_data.frame[0]));     
      m_data.GetRawData().push_back(tmp_frame_data);
    }
    m_current_file.close();
  }
  return true;
}
