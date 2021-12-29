#include "common_type.hpp"

#include <iostream>

class accelerometer_analizer {
  public:
    accelerometer_analizer(data &);
    ~accelerometer_analizer();
  
    bool cook();

  private:
    data m_data;
};

accelerometer_analizer::accelerometer_analizer(data &dat) : m_data(dat) {
  if(m_data.GetType() != 'a') {
    std::cerr << "Error in analizer constructor: wrong data type" << std::endl;
    exit(1);
  }
}

accelerometer_analizer::~accelerometer_analizer() {}

bool accelerometer_analizer::cook() {
  
  return true;
}
