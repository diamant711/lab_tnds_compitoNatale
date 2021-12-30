#include "common_type.hpp"

#include <iostream>
#include <cmath>

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
  for(int i = 0; i < m_data.GetRawData().size(); ++i) {
    data::analized tmp_analized;
    tmp_analized.T_s = 0;
    tmp_analized.point.x = 0;
    tmp_analized.point.y = 0;
    uint32_t first_four_byte = m_data.GetRawData()[i].frame[0];
    if(((first_four_byte & 0xFFFF0000) >> 2) == 0x5551) {
      std::cerr << "Espected identifier 0x5551xxxx. found: "
                << ((first_four_byte & 0xFFFF0000) >> 2) << std::endl;
      return false;
    }
    tmp_analized.T_s = static_cast<double>(m_data.GetRawData()[i].tsec) + 
                       static_cast<double>(m_data.GetRawData()[i].tusec) * 
                       std::pow(10,-6);
    tmp_analized.point.y = (first_four_byte & 0x0000FFFF);
    m_data.GetCookedData().push_back(tmp_analized);
  }
  return true;
}
