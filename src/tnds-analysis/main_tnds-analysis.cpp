#include "data_file_parser.hpp"
#include "image_analizer.hpp"
#include "common_type.hpp"
#include "accelerometer_analizer.hpp"

int main(int argc, char *argv[]) {
  const data::data_type_t acc_data_type = data::ACCELEROMETER;
  const data::data_type_t cam_data_type = data::CAMERA;
  data acc_data(acc_data_type);
  data cam_data(cam_data_type);
  data_file_parser acc_data_file_parser(acc_data);
  data_file_parser cam_data_file_parser(cam_data);
  accelerometer_analizer acc_analizer(acc_data);
  image_analizer cam_analizer(cam_data);
  TApplication app("Results", &argc, argv);
  TCanvas c1("c1", 800, 1200);
  TGraph acc_graph();
  TGraph cam_graph();

  acc_data_file_parser.fill_raw();
  cam_data_file_parser.fill_raw();
  acc_analizer.cook();
  cam_analizer.cook();

  for(unsigned int i = 0; i < acc_data.GetCookedData().size(); ++i){
    acc_graph.AddPoint(acc_data.GetCookedData()[i].T_s, 
                       acc_data.GetCookedData()[i].point.y);
  }
  for(unsigned int i = 0; i < cam_data.GetCookedData().size(); ++i){
    cam_graph.AddPoint(cam_data.GetCookedData()[i].T_s, 
                       cam_data.GetCookedData()[i].point.x);
  }
  
  return 0;
}
