// public access data method

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
 
    typedef raw_acc_cnt std::vector<frame_data>;
    typedef cooked_acc_cnt std::vector<analized>;

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

raw_acc_cnt& acc_data::GetRawData() {
  return m_raw;
}

cooked_acc_cnt& acc_data::GetCookedData() {
  return m_coocked;
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

    typedef raw_cam_cnt std::vector<frame_data>;
    typedef cooked_cam_cnt std::vector<analized>;

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

raw_cam_cnt& cam_data::GetRawData() {
  return m_raw;
}

cooked_cam_cnt& cam_data::GetCookedData() {
  return m_coocked;
}
