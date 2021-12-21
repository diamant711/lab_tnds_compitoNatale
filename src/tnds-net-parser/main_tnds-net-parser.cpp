#include "data_net_parser.hpp"

int main(int argc, char *argv[]) {
  const std::string acc_addr("233.252.4.19");
  const std::string cam_addr("233.252.4.17");
  const unsigned int acc_port = 20017;
  const unsigned int cam_port = 20017;
  
  UDPframeReceiver acc_udp(acc_addr, acc_port);
  UDPframeReceiver cam_udp(cam_addr, cam_port);
  data_net_parser acc_net_parser(data_net_parser::ACCELEROMETER, acc_udp);
  data_net_parser cam_net_parser(data_net_parser::CAMERA, cam_udp);
  
  unsigned long i = 0, j = 0;
  while (1) {
    if (acc_net_parser.receive_and_save_frame()){
      ++i;
      std::cout << "Accelerometer frame saved = " << i << std::endl;
    }
    if (acc_net_parser.receive_and_save_frame()){
      ++j;
      std::cout << "Camera frame saved = " << j << std::endl;
    }
  }
  return 0;
}
