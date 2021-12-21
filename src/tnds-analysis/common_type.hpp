// public access data method

#include <vector>

class acc_data {
  public:
    acc_data();
    ~acc_data();
  private:
    // RAW
    typedef struct frame_data {
      unsigned int tsec;
      unsigned int tusec;
      unsigned int width;
      unsigned int height;
      std::vector<unsigned int> frame;
    } frame_data;
    std::vector<frame_data> raw;
    
    // COOKED
    typedef struct analized {
      unsigned int Ax;
      unsigned int Ay;
      unsigned int Az;
      unsigned int T_C;
      unsigned int T_s;
    } analized;
    std::vector<analized> cooked;
};

acc_data::acc_data() {}

acc_data::~acc_data() {}

class cam_data {
  public:
    cam_data();
    ~cam_data();
  private:
    //RAW
    typedef struct frame_data {
      unsigned int tsec;
      unsigned int tusec;
      unsigned int width;
      unsigned int height;
      std::vector<unsigned int> frame;
    } frame_data;
    std::vector<frame_data> raw;
    
    // COOKED
    typedef struct Point {
      unsigned int x;
      unsigned int y;
    } Point;
    typedef struct analized {
      Point center;
      unsigned int T_s;
    } analized;
    std::vector<analized> cooked;
};

cam_data::cam_data() {}

cam_data::~cam_data() {}
