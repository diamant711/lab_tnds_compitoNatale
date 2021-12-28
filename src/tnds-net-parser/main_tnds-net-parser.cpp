#include "data_net_parser.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
  const unsigned int max_frame = 10000;
  
  const std::string acc_addr("233.252.4.19");
  const std::string cam_addr("233.252.4.17");
  const unsigned int acc_port = 20017;
  const unsigned int cam_port = 20017;
  
  UDPframeReceiver acc_udp(acc_addr, acc_port);
  UDPframeReceiver cam_udp(cam_addr, cam_port);
  data_net_parser acc_net_parser(data_net_parser::ACCELEROMETER, acc_udp);
  data_net_parser cam_net_parser(data_net_parser::CAMERA, cam_udp);
  
  if(acc_udp.good() && cam_udp.good())
    std::cout << "good to go" << std::endl;
  else
    std::cout << "not good" << std::endl;

  for(int i = 0; i < max_frame; ++i) {
    if (acc_net_parser.receive_and_save_frame()){
      std::cout << "Accelerometer frame saved = " << i << std::endl;
    }
    if (cam_net_parser.receive_and_save_frame()){
      std::cout << "Camera frame saved = " << i << std::endl;
    }
  }
  return 0;
}
