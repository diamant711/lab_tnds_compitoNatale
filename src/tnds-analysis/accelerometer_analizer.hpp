#include "common_type.hpp"

class accelerometer_analizer {
  public:
    accelerometer_analizer(data &);
    ~accelerometer_analizer();
  
    bool cook();

  private:
    data m_data;
};

accelerometer_analizer::accelerometer_analizer(data &dat) : m_data(dat) {}

accelerometer_analizer::~accelerometer_analizer() {}

bool accelerometer_analizer::cook() {
  
  return true;
}
