//
// File: receive_camera.cpp
//
// Revision history:
// 23-Nov-2009 Initial version.
//
// Description:
// Receive a camera image from the network, convert it into a ROOT
// TASImage and display it. 
//

#include <iostream>
#include <string>

#include "TApplication.h"
#include "TCanvas.h"
#include "TThread.h"
#include "TASImage.h"

#include "UDPframebroadcast.h"

// Global canvas for drawing camera images.
TCanvas *c1;
UDPframeReceiver *ufr_p;

void *rc_thread(void *)
{
 TThread myt(0L); // Myself.
 unsigned int *data;
 unsigned int width, height;
 unsigned int ts_sec, ts_usec;

 c1->cd();

 while(ufr_p->good())
  {
   ufr_p->receive_frame(ts_sec, ts_usec, &data, width, height);
   myt.Lock();
   TASImage img(width, height);
   ::memcpy(img.GetArgbArray(), data, width*height*sizeof(unsigned int));
   c1->cd();
   img.Draw();
   c1->Modified();
   c1->Update();
   myt.UnLock();
  }
 return 0;
}

int
main(int argc, char *argv[])
{
  std::string source_address;

  if (argc > 1)
   {
    source_address = argv[1];
   }
  else
   {
    source_address = _UDPframebroadcast_h_DEFAULT_MCAST_ADDR;
   }

  UDPframeReceiver ufr(source_address);

  if (!ufr.good())
   {
    std::cerr << argv[0] << "Error initializing UDP frame receiver: "
              << ufr.get_error() << std::endl;
    return 1;
   } 
  
  ufr_p = &ufr;

  TApplication theApp("Camera broadcast", &argc, argv);

  c1 = new TCanvas("c1","Camera broadcast", 600, 600);

  TThread rc("Receive camera data",rc_thread);
  rc.Run();

  std::cout << "*** Exit the program by selecting Quit from the File menu ***"
    << std::endl;
  theApp.Run(kTRUE);

  rc.SetCancelAsynchronous();
  rc.Kill();

  delete c1;
  return 0;
}

