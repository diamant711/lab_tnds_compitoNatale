/*

  known errors:
   - error on the time values (is increment correct ?)

*/

#include "data_file_parser.hpp"
#include "image_analizer.hpp"
#include "common_type.hpp"
#include "accelerometer_analizer.hpp"

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TVectorT.h"

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
  TCanvas c1("c1", "Results", 800, 1200);

  acc_data_file_parser.fill_raw();
#ifdef DEBUG
  std::cerr << "DEBUG: Readed all accelerometer data from file" << std::endl;
#endif
  cam_data_file_parser.fill_raw();
#ifdef DEBUG
  std::cerr << "DEBUG: Readed all camera data from file" << std::endl;
#endif
  acc_analizer.cook();
#ifdef DEBUG
  std::cerr << "DEBUG: Cooked all accelerometer data" << std::endl;
#endif 
  cam_analizer.cook();
#ifdef DEBUG
  std::cerr << "DEBUG: Cooked all camera data" << std::endl;
#endif 
  
  TVectorT<double> acc_x(acc_data.GetCookedData().size());
  TVectorT<double> acc_y(acc_data.GetCookedData().size());
  TVectorT<double> cam_x(cam_data.GetCookedData().size());
  TVectorT<double> cam_y(cam_data.GetCookedData().size());

  for(unsigned int i = 0; i < acc_data.GetCookedData().size(); ++i){
    acc_x[i] = acc_data.GetCookedData()[i].T_s;
    acc_y[i] = acc_data.GetCookedData()[i].point.y;
#ifdef DEBUG
    std::cerr << "DEBUG: " << i << " - " << acc_x[i] << " - " << acc_y[i] << std::endl;
#endif 
  }

#ifdef DEBUG
  std::cerr << "DEBUG: loaded all accelerometer point in TGraph" << std::endl;
#endif 

  for(unsigned int i = 0; i < cam_data.GetCookedData().size(); ++i){
    cam_x[i] = cam_data.GetCookedData()[i].T_s;
    cam_y[i] = cam_data.GetCookedData()[i].point.x;
#ifdef DEBUG
    std::cerr << "DEBUG: " << i << " - " << cam_x[i] << " - " << cam_y[i] << std::endl;
#endif 
  }
  
#ifdef DEBUG
  std::cerr << "DEBUG: loaded all camera point in TGraph" << std::endl;
#endif 

  TGraph acc_graph(acc_x, acc_y);
  TGraph cam_graph(cam_x, cam_y);

  acc_graph.SetTitle("Accelerometer");
  cam_graph.SetTitle("Camera");

  c1.Divide(1,2,0,0);
  c1.cd(1);
  acc_graph.Draw("AC*");
  c1.cd(2);
  cam_graph.Draw("AC*");

  app.Run();
  
  return 0;
}
