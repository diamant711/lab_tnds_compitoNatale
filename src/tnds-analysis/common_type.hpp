#ifndef COMMON_TYPE
#define COMMON_TYPE

#include <vector>
#include <memory>

class data {
  public:
    enum data_type_t {ACCELEROMETER = 'a', CAMERA = 'i'};

    typedef struct frame_data {
      unsigned int tsec;
      unsigned int tusec;
      unsigned int width;
      unsigned int height;
      std::vector<unsigned int> frame;
    } frame_data;

    typedef struct acc_analized {
      unsigned int Ax;
      unsigned int Ay;
      unsigned int Az;
      unsigned int T_C;
      unsigned int T_s;
    } analized;
    
    typedef struct Point {
      unsigned int x;
      unsigned int y;
    } Point;
    
    typedef struct cam_analized {
      Point center;
      unsigned int T_s;
    } analized;
    
    typedef std::vector<frame_data> raw_cnt;
    typedef std::vector<acc_analized> cooked_acc_cnt;
    typedef std::vector<cam_analized> cooked_cam_cnt;

    data(data_type_t);
    ~data();
    
    raw_cnt& GetRawData();
    cooked_acc_cnt& GetCookedData();
    cooked_cam_cnt& GetCookedData();

  private:
    data_type_t data_type;

    // RAW
    raw_cnt m_raw;
    
    // COOKED
    cooked_acc_cnt m_acc_cooked;
    cooked_cam_cnt m_cam_cooked;
};

data::acc_data(data_type_t type) : data_type(type) {}

data::~acc_data() {}

data::raw_cnt& data::GetRawData() {
  return m_raw;
}

data::cooked_acc_cnt& data::GetCookedData() {
  if(m_data_type == ACCELEROMETER)
    return m_acc_cooked;
  else {
    std::cerr << "Tryed calling data::GetCoockedData() for "
                 " data::coocked_acc_cnt& when m_data_type is CAMERA" 
              << std::endl;
    exit(1);
  }
}

data::cooked_cam_cnt& data::GetCookedData() {
  if(m_data_type == CAMERA)
    return m_cam_cooked;
  else {
    std::cerr << "Tryed calling data::GetCoockedData() for "
                 " data::coocked_cam_cnt& when m_data_type is ACCELEROMETER" 
              << std::endl;
    exit(1);
  }
}

#endif
