#ifndef COMMON_TYPE
#define COMMON_TYPE

#include <vector>
#include <iostream>
#include <cstdint>

class data {
  public:
    enum data_type_t {ACCELEROMETER = 'a', CAMERA = 'i'};

    typedef struct frame_data {
      uint32_t tsec;
      uint32_t tusec;
      uint32_t width;
      uint32_t height;
      uint32_t bytes;
      std::vector<uint32_t> frame;
    } frame_data;

    typedef struct Point {
      unsigned int x;
      unsigned int y;
    } Point;
    
    typedef struct analized {
      Point point;
      double T_s;
    } analized;
    
    typedef std::vector<frame_data> raw_cnt;
    typedef std::vector<analized> cooked_cnt;

    data(data_type_t);
    ~data();
    
    raw_cnt& GetRawData();
    cooked_cnt& GetCookedData();

    const data_type_t& GetType();
  private:
    data_type_t m_data_type;

    // RAW
    raw_cnt m_raw;
    
    // COOKED
    cooked_cnt m_cooked;
};

data::data(data_type_t type) : m_data_type(type) {}

data::~data() {}

data::raw_cnt& data::GetRawData() {
  return m_raw;
}

data::cooked_cnt& data::GetCookedData() {
  return m_cooked;
}

const data::data_type_t& data::GetType() {
  return m_data_type;
}

#endif
