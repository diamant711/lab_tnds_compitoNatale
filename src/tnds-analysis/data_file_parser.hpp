#include "common_type.hpp"

#include <string>
#include <fstream>

#include <cstdio> //sprintf

class data_file_parser {
  public:
    data_file_parser(data::data_type_t);
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
   
    bool set_data(data &);
    
    void print();

  private:
    data::data_type_t m_data_type;
    data m_data;
    const unsigned int max_len_file_name = 20;
    std::string m_current_file_path;
    std::ofstream m_current_file;
    bool m_ready_to_read = false;
    unsigned int m_tot_number_of_file;
};
/*
data_file_parser::data_file_parser(data::data_type_t type) {
  m_data_type = type;
  char tmp[max_len_file_name];
  ::sprintf(tmp, "data/%c/0.fdt", m_data_type);
  m_current_file_path = tmp;
  m_current_file.open(m_current_file_path);
}
*/
data_file_parser::~data_file_parser() {
  m_current_file.close();
}

bool data_file_parser::set_data(data &dat){
  if(dat.GetType() == m_data_type) {
    m_data = dat;
    m_ready_to_read = true;
    return true;
  } else
    return false;
}

void data_file_parser::print(){
}
