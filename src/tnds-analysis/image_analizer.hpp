#include "common_type.hpp"

class image_analizer {
  public:
    image_analizer(data &);
    ~image_analizer();
  
    bool cooked();

  private:
    data m_data;
};

image_analizer::image_analizer(data &dat) : m_data(dat) {}

image_analizer::~image_analizer() {}

image_analizer::cooked() {
  
  return true;
}
