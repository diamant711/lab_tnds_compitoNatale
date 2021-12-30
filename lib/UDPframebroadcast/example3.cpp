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
#include <memory>
#include <vector>
#include <string>

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TThread.h"
#include "TASImage.h"

#include "UDPframebroadcast.h"

// Global canvas for drawing camera images.
TCanvas *c1;
UDPframeReceiver *ufr_p;

static const int MEAS_LEN = 11;
static const int START_OF_MEAS = 0x55;

int
handle_measurement(unsigned char *dat,
                   std::vector<double> &vx,
                   std::vector<double> &vy,
                   std::vector<double> &vz)
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
#ifdef DEBUG
      if (ay > 11)
       {
        fprintf(stderr, "DEBUG: ax == %g, ay == %g, az == %g, t == %g\n",
                ax, ay, az, t);
       }
#endif
      break;
    case 0x52: /* Angular velocity */
      ds = (dat[2] | (static_cast<short>(dat[3]) << 8));
      ax = ds / 32768. * 2000.;
      ds = (dat[4] | (static_cast<short>(dat[5]) << 8));
      ay = ds / 32768. * 2000;
      ds = (dat[6] | (static_cast<short>(dat[7]) << 8));
      az = ds / 32768. * 2000;
      ds = (dat[8] | (static_cast<short>(dat[9]) << 8));
      vx.push_back(ax);
      vy.push_back(ay);
      vz.push_back(az);
#ifdef DEBUG
      fprintf(stderr, "DEBUG: ax == %g, ay == %g, az == %g\n",
              ax, ay, az);
#endif
      break;
    default:
      break;
   }
  return ret;
 }

int
handle_measurements(unsigned char *dat, int len,
                    std::vector<double> &vx,
                    std::vector<double> &vy,
                    std::vector<double> &vz)
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
      int ret = handle_measurement(sm, vx, vy, vz);
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

 std::vector<double> vx, vy, vz;
 std::shared_ptr<TGraph> rx, ry, rz;
 std::shared_ptr<TF1> ffit, ftfitx, ftfity; 

 double t0 = -1;
 while(ufr_p->good())
  {
   ufr_p->receive_frame(ts_sec, ts_usec, &dfr, width, height);
   if (height > 1)
    {
     // Minimal search of red points
     double bx = 0., by = 0., n = 0.;
     for (int y=0; y<height; ++y)
       for (int x=0; x<width; ++x)
        {
         int r = (dfr[y*width + x]&0xff0000)>>16;
         int g = (dfr[y*width + x]&0xff00)>>8;
         int b = (dfr[y*width + x]&0xff);
         if (r > (g+b/2)+0x20)
          {
           bx = bx * (n / (n+1.)) + static_cast<double>(x)* (1./(n+1.)); 
           by = by * (n / (n+1.)) + static_cast<double>(y)* (1./(n+1.)); 
          }
         else
          {
           int a = (r+g+b)/10.;
           dfr[y*width + x] = 0xff000000 | ((a&0xff) << 16) | 
                              ((a&0xff) << 8) | (a&0xff);
          }
        }
     vx.push_back(bx);
     vy.push_back(by);
     if (t0 < 0)
      {
        t0 = static_cast<double>(ts_sec) +
             static_cast<double>(ts_usec)/1000000.;
      }
     vz.push_back(static_cast<double>(ts_sec) +
               static_cast<double>(ts_usec)/1000000. - t0);

     if ((vx.size() % 10) == 0)
         std::cerr << "DEBUG: size: " << vx.size() << " bx == " << bx
                   << " by == " << by << std::endl;

     myt.Lock();
     TASImage img(width, height);
     ::memcpy(img.GetArgbArray(), dfr, width*height*sizeof(unsigned int));
     c1->Clear();
     c1->Divide(1,3);
     c1->cd(1);
     img.Draw();
     if (vx.size() == 40)
      {
       rx.reset(new TGraph(vx.size(), &(vz[0]), &(vx[0])));
       ry.reset(new TGraph(vy.size(), &(vz[0]), &(vy[0])));
       ftfitx.reset(new TF1("ftfitx", "[1]*sin([0]*x + [2]) + [3]", vz[0], vz.back()));
       ftfity.reset(new TF1("ftfity", "[1]*sin([0]*x + [2]) + [3]", vz[0], vz.back()));
       ftfitx->SetParameter(0, 3.);
       ftfitx->SetParameter(1, 160.);
       ftfitx->SetParameter(2, 4.);
       ftfitx->SetParameter(3, by);
       ftfitx->SetParError(0, 0.01);
       ftfitx->SetParError(1, 2.);
       ftfitx->SetParError(2, 0.01);
       ftfitx->SetParError(3, 2.);
       rx->Fit(ftfitx.get(),"");
       std::cout << "FitX parameter 0: " << ftfitx->GetParameter(0) << " +/- "
                 << ftfitx->GetParError(0) << " ChiSquare: " << ftfitx->GetChisquare() << std::endl;
       ftfity->SetParameter(0, 3.);
       ftfity->SetParameter(1, 160.);
       ftfity->SetParameter(2, 4.);
       ftfity->SetParameter(3, by);
       ftfity->SetParError(0, 0.01);
       ftfity->SetParError(1, 2.);
       ftfity->SetParError(2, 0.01);
       ftfity->SetParError(3, 2.);
       ry->Fit(ftfity.get(),"");
       std::cout << "FitY parameter 0: " << ftfity->GetParameter(0) << " +/- "
                 << ftfity->GetParError(0) << " ChiSquare: " << ftfity->GetChisquare() << std::endl;
       c1->cd(2);
       rx->Draw();
       c1->cd(3);
       ry->Draw();
      }
     else
      {
       if (rx)
        {
         c1->cd(2);
         rx->Draw();
        }
       if (ry)
        {
         c1->cd(3);
         ry->Draw();
        }
      }
     c1->Modified();
     c1->Update();
     myt.UnLock();
    }
   else
    {
     int ret = handle_measurements(reinterpret_cast<unsigned char *>(dfr),
                         ufr_p->get_last_byte_length(),
                         vx, vy, vz);
     if ((ret > 0) && (vx.size() == 300))
      {
       std::vector<double> xv(vx.size());
       // The BWT901CL probe reads by default ao 10 Hz. 
       for (int i = 0; i<vx.size(); ++i) xv[i] = static_cast<double>(i)/10.;
       myt.Lock();
       rx.reset(new TGraph(vx.size(), &(xv[0]), &(vx[0])));
       ry.reset(new TGraph(vx.size(), &(xv[0]), &(vy[0])));
       rz.reset(new TGraph(vx.size(), &(xv[0]), &(vz[0])));
       ffit.reset(new TF1("ffit", "[1]*sin([0]*x + [2])", xv[0], xv.back()));
       ffit->SetParameter(0, 0.8);
       ffit->SetParameter(1, 100.);
       ffit->SetParameter(2, 0.);
       ffit->SetParError(0, 0.01);
       ffit->SetParError(1, 0.01);
       ffit->SetParError(2, 0.01);
       rx->Fit(ffit.get(),"");
       std::cout << "Fit parameter 0: " << ffit->GetParameter(0) << " +/- "
                 << ffit->GetParError(0) << " ChiSquare: " << ffit->GetChisquare() << std::endl;
       c1->Clear();
       c1->Divide(1,3);
       c1->cd(1);
       rx->Draw();
       c1->cd(2);
       ry->Draw();
       c1->cd(3);
       rz->Draw();
       c1->Modified();
       c1->Update();
       myt.UnLock();
      }
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

  c1 = new TCanvas("c1","Camera broadcast", 800, 1200);

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

