#include "common_type.hpp"

#include <iostream>
#include <cmath>

class accelerometer_analizer {
  public:
    accelerometer_analizer(data &);
    ~accelerometer_analizer();
  
    bool cook();

  private:
    data &m_data;
    const double m_g = 9.81;
};

accelerometer_analizer::accelerometer_analizer(data &dat) : m_data(dat) {
  if(m_data.GetType() != data::ACCELEROMETER) {
    std::cerr << "Error in analizer constructor: wrong data type" << std::endl;
    exit(1);
  }
}

accelerometer_analizer::~accelerometer_analizer() {}

bool accelerometer_analizer::cook() {
  for(unsigned int i = 0; i < m_data.GetRawData().size(); ++i) {
    data::analized tmp_analized;
    tmp_analized.T_s = 0;
    tmp_analized.point.x = 0;
    tmp_analized.point.y = 0;
    
    uint16_t two_bytes[2] = {0,0};
    two_bytes[0] = m_data.GetRawData()[i].frame[0] & 0x0000FFFF;
    if(two_bytes[0] != 0x5155) continue;
    two_bytes[1] = (m_data.GetRawData()[i].frame[0] & 0xFFFF0000) >> 16;
    uint8_t AxH = (two_bytes[1] & 0xFF00) >> 8,
            AxL = two_bytes[1] & 0x00FF;
    tmp_analized.T_s = static_cast<double>(m_data.GetRawData()[i].tsec) + 
                       static_cast<double>(m_data.GetRawData()[i].tusec) * 
                       std::pow(10,-6);
    tmp_analized.point.y = static_cast<double>((AxH << 8) | AxL) / 32768 * 16 * m_g;
    m_data.GetCookedData().push_back(tmp_analized);
  }
  return true;
}
