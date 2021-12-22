#include "common_type.hpp"

#include <string>
#include <fstream>

#include <cstdio> //sprintf

class data_file_parser {
  public:
    enum data_type_t {ACCELEROMETER = 'a', CAMERA = 'i'};

    data_file_parser(data_type_t);
    ~data_file_parser();
  private:
    data_type_t m_data_type;
    std::string m_current_file_path;
    ofstream m_current_file;
};

data_file_parser::data_file_parser(data_type_t type) {
  m_data_type = type;
  ::sprintf(m_current_file_path, "data/%c/0.fdt", m_data_type);
  m_current_file.open(m_current_file_path);
}

data_file_parser::~data_file_parser() {
  m_current_file.close();
}
