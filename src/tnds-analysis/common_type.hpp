#ifndef COMMON_TYPE
#define COMMON_TYPE

#include <vector>

//-----------------------------------------------------------------------------
//--- acc_data ----------------------------------------------------------------
//-----------------------------------------------------------------------------

class acc_data {
  public:
    typedef struct frame_data {
      unsigned int tsec;
      unsigned int tusec;
      unsigned int width;
      unsigned int height;
      std::vector<unsigned int> frame;
    } frame_data;

    typedef struct analized {
      unsigned int Ax;
      unsigned int Ay;
      unsigned int Az;
      unsigned int T_C;
      unsigned int T_s;
    } analized;
 
    typedef std::vector<frame_data> raw_acc_cnt;
    typedef std::vector<analized> cooked_acc_cnt;

    acc_data();
    ~acc_data();
    
    raw_acc_cnt& GetRawData();
    cooked_acc_cnt& GetCookedData();

  private:
    // RAW
    raw_acc_cnt m_raw;
    
    // COOKED
    cooked_acc_cnt m_cooked;
};

acc_data::acc_data() {}

acc_data::~acc_data() {}

acc_data::raw_acc_cnt& acc_data::GetRawData() {
  return m_raw;
}

acc_data::cooked_acc_cnt& acc_data::GetCookedData() {
  return m_cooked;
}

//-----------------------------------------------------------------------------
//--- cam_data ----------------------------------------------------------------
//-----------------------------------------------------------------------------

class cam_data {
  public:
    typedef struct frame_data {
      unsigned int tsec;
      unsigned int tusec;
      unsigned int width;
      unsigned int height;
      std::vector<unsigned int> frame;
    } frame_data;
 
    typedef struct Point {
      unsigned int x;
      unsigned int y;
    } Point;
    
    typedef struct analized {
      Point center;
      unsigned int T_s;
    } analized;

    typedef std::vector<frame_data> raw_cam_cnt;
    typedef std::vector<analized> cooked_cam_cnt;

    cam_data();
    ~cam_data();

    raw_cam_cnt& GetRawData();
    cooked_cam_cnt& GetCookedData();

  private:
    //RAW
    raw_cam_cnt m_raw;
    
    // COOKED
    cooked_cam_cnt m_cooked;
};

cam_data::cam_data() {}

cam_data::~cam_data() {}

cam_data::raw_cam_cnt& cam_data::GetRawData() {
  return m_raw;
}

cam_data::cooked_cam_cnt& cam_data::GetCookedData() {
  return m_cooked;
}

#endif
