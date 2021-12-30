#include "common_type.hpp"

#include <cmath>

class image_analizer {
  public:
    image_analizer(data &);
    ~image_analizer();
  
    bool cooked();

  private:
    data m_data;
};

image_analizer::image_analizer(data &dat) : m_data(dat) {
  if(m_data.GetType() != data::CAMERA) {
    std::cerr << "Error in analizer constructor: wrong data type" << std::endl;
    exit(1);
  }
}

image_analizer::~image_analizer() {}

bool image_analizer::cooked() {
  for(unsigned int i = 0; i < m_data.GetRawData().size(); ++i){
    data::analized tmp_analized;
    tmp_analized.point.x = 0;
    tmp_analized.point.y = 0;
    tmp_analized.T_s = 0;
    double bx = 0., by = 0., n = 0.;
    for(unsigned int y = 0; y < m_data.GetRawData()[i].height; ++y){
      for(unsigned int x = 0; x < m_data.GetRawData()[i].width; ++x) {
        int r = (m_data.GetRawData()[i].frame[y*m_data.GetRawData()[i].width + x]
                                              &0xff0000)>>16;
        int g = (m_data.GetRawData()[i].frame[y*m_data.GetRawData()[i].width + x]
                                              &0xff00)>>8;
        int b = (m_data.GetRawData()[i].frame[y*m_data.GetRawData()[i].width + x]
                                              &0xff);
        if (r > (g+b/2)+0x20) {
          bx = bx * (n / (n+1.)) + static_cast<double>(x)* (1./(n+1.));
          by = by * (n / (n+1.)) + static_cast<double>(y)* (1./(n+1.));
        }
        else {
          int a = (r+g+b)/10.;
          m_data.GetRawData()[i].frame[y*m_data.GetRawData()[i].width + x] = 
                                              0xff000000 | ((a&0xff) << 16) |
                                              ((a&0xff) << 8) | (a&0xff);
        }
      }
    }
    tmp_analized.point.x = bx;
    tmp_analized.point.x = by;
    tmp_analized.T_s = static_cast<double>(m_data.GetRawData()[i].tsec) +
                       static_cast<double>(m_data.GetRawData()[i].tusec) *
                       std::pow(10,-6);
    m_data.GetCookedData().push_back(tmp_analized);
  }
  return true;
}
