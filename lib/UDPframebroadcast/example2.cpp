//
// File: receive_udp.cpp
//
// Revision history:
// 20-Dec-2021 Forked from receive_camera.cpp.
//
// Description:
// Receive a camera image from the network, convert it into a ROOT
// TASImage and display it -or- receive accelerometer data and plot
//  them..
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

static const int MEAS_LEN = 11;
static const int START_OF_MEAS = 0x55;

int
handle_measurement(unsigned char *dat)
 {
  static const float g = 9.8;
  int sum = 0;
  int i;
  int ret = MEAS_LEN;
  float ax, ay, az, t;

  if (dat[0] != START_OF_MEAS) return -1;

  for (i=0; i<(MEAS_LEN - 1); ++i) sum += dat[i];

  if (dat[MEAS_LEN - 1] != (sum&0xff))
   {
#ifdef DEBUG
    fprintf(stderr, "DEBUG: Checksum check fails: [%02X] instead of [%02X]\n",
            dat[MEAS_LEN - 1], (sum&0xff));
    for (i=0; i < MEAS_LEN+5; i++)
     {
      if ((i > 0) && ((i%16) == 0)) printf ("\n");
      fprintf(stderr, "[%02hhx]", dat[i]);
     }
    fprintf(stderr, "\n");
#endif
    if (dat[MEAS_LEN - 1] != START_OF_MEAS) return -1;
    
    // NEED TO INVESTIGATE: Sometimes the checksum byte is missing 
    // allow this kind of short events
#ifdef DEBUG
    fprintf(stderr, "DEBUG: Allow short read\n");
#endif
    --ret;
   }
#ifdef DEBUG
   else fprintf(stderr, "DEBUG: Checksum OK!\n");
#endif

  short ds;

  switch(dat[1])
   {
    case 0x51: /* Acceleration */
      ds = (dat[2] | (static_cast<short>(dat[3]) << 8));
      ax = ds* g /2048;
      ds = (dat[4] | (static_cast<short>(dat[5]) << 8));
      ay = ds* g /2048;
      ds = (dat[6] | (static_cast<short>(dat[7]) << 8));
      az = ds* g /2048;
      ds = (dat[8] | (static_cast<short>(dat[9]) << 8));
      t  = ds/100.;
      fprintf(stderr, "DEBUG: ax == %g, ay == %g, az == %g, t == %g\n",
              ax, ay, az, t);
      break;
    default:
      break;
   }
  return ret;
 }

int
handle_measurements(unsigned char *dat, int len)
 {
  unsigned char sm[MEAS_LEN];
  static int mpos = 0;
  int count = 0;

  // If a new measurement starts, discard the old one.
  if (dat[0] == START_OF_MEAS) mpos = 0;

  for (int i=0; i < len; ++i)
   {
    if ((mpos == 0) && (dat[i] != START_OF_MEAS)) continue;
    sm[mpos] = dat[i];
    ++mpos;
    if (mpos >= MEAS_LEN)
     {
      int ret = handle_measurement(sm);
      if (ret > 0) ++count;
      mpos = 0;
      if (ret < MEAS_LEN)
       {
        for (int i=ret; i<MEAS_LEN; ++i)
         {
          sm[mpos] = sm[i];
          ++mpos;
         }
       }
     }
   }
  return count;
 }

void *rc_thread(void *)
{
 TThread myt(0L); // Myself.
 unsigned int *dfr;
 unsigned int width, height;
 unsigned int ts_sec, ts_usec;

 c1->cd();

 while(ufr_p->good())
  {
   ufr_p->receive_frame(ts_sec, ts_usec, &dfr, width, height);
   if (height > 1)
    {
     myt.Lock();
     TASImage img(width, height);
     ::memcpy(img.GetArgbArray(), dfr, width*height*sizeof(unsigned int));
     c1->cd();
     img.Draw();
     c1->Modified();
     c1->Update();
     myt.UnLock();
    }
   else
    {
     handle_measurements(reinterpret_cast<unsigned char *>(dfr),
                         ufr_p->get_last_byte_length());
    }
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

