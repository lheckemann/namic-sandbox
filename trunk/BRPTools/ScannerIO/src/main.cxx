#ifdef WIN32
#define NOMINMAX
#ifdef USE_RAWINPUT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#endif
#endif

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <math.h>

// for TCP/IP connection
#include <ace/ACE.h>
#include <ace/OS.h>
#include <ace/FILE.h>
#include <ace/Thread.h>
#include <ace/Synch.h>
#include <ace/Thread_Mutex.h>

#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>

#include <iostream>

#include "igtl_header.h"
#include "igtl_image.h"

#include "igtlMath.h"
#include "ScannerSim.h"


class ACE_Thread_Mutex;

// for TCP/IP connection
ACE_INET_Addr   address;
ACE_SOCK_Stream sock;
ACE_SOCK_Connector connector;
// for thread
void * thread;
ACE_Thread_Mutex * mutex;

int  stop;
bool connected;

static void thread_func( void * data );

bool  checkAndConnect();
void  serverMonitor();
int   connectToSlicer(const char* hostname, int port);
void  disconnect();
char* loadImageData(char* fnameTemp, int bindex, int eindex, size_t fsize);
int   session(ScannerSim* scanner, int intv_ms);
//int session(char* data, int psize, int* size, int t, float* spacing, int intv_ms);


bool checkAndConnect()
{
  ACE_Time_Value timeOut(5,0);
  int ret;
  
  mutex->acquire();  //lock
  if (!connected)
    {
      ret = connector.connect(sock, address, &timeOut);
      if(ret == -1 && errno != ETIME && errno != 0 )
        {
          ACE_DEBUG((LM_ERROR, ACE_TEXT("Error %d : Cannot connect to Slicer Daemon!\n"), errno));
        }
      else
        {
          connected = true;
        }
    }
  mutex->release();  //unlock
  return connected;
}


void serverMonitor()
{
  int ret;
  char buf[256];
  ACE_Time_Value timeOut(10,0);
  int tbytes;
  
  while (!stop) {
    if (checkAndConnect()) {
      ret = sock.recv_n(buf, 256, &timeOut, (size_t*)&tbytes);
      if (ret > 0) {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("Received: %s\n"), buf));
      } else if (ret == 0) {
        disconnect();
      }
      else if (ret == -1 ) {
        if( errno != ETIME && errno != 0 ) {
          ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d!\n"), errno));
          disconnect();
        }
        // else -- time out
      }
    }
    else // wait for a while
      {
        ACE_Time_Value timeVal(0, (int)(1000)*1000 );
        ACE_OS::sleep( timeVal );
      }
  }
}


static void thread_func( void * data )
{
  serverMonitor();
}


int connectToSlicer(const char* hostname, int port)
{
  address.set(port, hostname);
  checkAndConnect();
  
  stop = 0;
  thread = new ACE_thread_t;
  ACE_Thread::spawn((ACE_THR_FUNC)thread_func, 
                    NULL, 
                    THR_NEW_LWP | THR_JOINABLE,
                    (ACE_thread_t *)thread );
  return 1;
}


void disconnect()
{
  mutex->acquire();  //lock
  if (connected)
    {
      connected = false;
      sock.close();
    }
  mutex->release();  //unlock
}


char* loadImageData(char* fnameTemp, int bindex, int eindex, size_t fsize)
{
  char filename[128];

  int nframes = eindex - bindex + 1;
  char* data = new char[nframes*fsize];

  for (int i = 0; i < nframes; i ++) {
    sprintf(filename, fnameTemp, i+bindex);
    std::cerr << "Reading " << filename << std::endl;
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
      std::cerr << "File opeining error: " << filename << std::endl;
      return NULL;
    }
    size_t b = fread(&data[i*fsize], 1, fsize, fp);
    fclose(fp);
    if (b != fsize) {
      std::cerr << "File reading error: " << filename << std::endl;
      std::cerr << "   File size: " << fsize << std::endl;
      std::cerr << "   Read data: " << b << std::endl;

      return NULL;
    }
  }
  return data;
}


int session(ScannerSim* scanner, int intv_ms)
{

  while (1) {
    float position[3];
    float orientation[4];
    static float phi = 0.0;
    position[0] = 50.0 * cos(phi);
    position[1] = 50.0 * sin(phi);
    position[2] = 0;
    phi = phi + 0.2;
    
    static float theta = 0.0;
    orientation[0]=0.0;
    orientation[1]=0.6666666666*cos(theta);
    orientation[2]=0.577350269189626;
    orientation[3]=0.6666666666*sin(theta);
    theta = theta + 0.1;
    
    if (connected) {
      // Get position/orientation from the NaviTrack stream and
      // convert them for Slicer Daemon
      igtl::Matrix4x4 matrix;
      igtl::QuaternionToMatrix(orientation, matrix);
      igtl::Image::Pointer frame = scanner->GetCurrentFrame();

      matrix[0][3] = position[0];
      matrix[1][3] = position[1];
      matrix[2][3] = position[2];
      //frame->SetNormals(matrix);
      //frame->SetOrigin(position);
      frame->SetMatrix(matrix);

      igtl_header header;
      header.version = IGTL_HEADER_VERSION;
      memcpy(header.name, "IMAGE", 6);
      memcpy(header.device_name, "NaviTrackRTImage", 17);
      header.timestamp = 0;
      header.body_size = frame->GetPackSize();
      header.crc       = 0;
      
      igtl_header_convert_byte_order(&header);

      int ret;
      ACE_Time_Value timeOut(1,0);
  
      ret = sock.send_n(&header, IGTL_HEADER_SIZE, &timeOut); 
      ret = sock.send_n(frame->GetPackPointer(), frame->GetPackSize(), &timeOut);

      if (ret <= 0) {
        disconnect();
        ACE_DEBUG((LM_ERROR, ACE_TEXT("Error %d : Connection Lost!\n"), errno));
      }
    }
    
    ACE_Time_Value timeVal(0, (int)(intv_ms)*1000 );
    ACE_OS::sleep( timeVal );
  }
}


int main(int argc, char **argv)
{

  if (argc < 8) {
    std::cerr << "Invalid arguments!" << std::endl;
    std::cerr << "Usage: " << argv[0] << " <x> <y> <z> <type> <rx> <ry> <rz>"
              << " <fname_temp> <bindex> <eindex> <fps> <hostname>"
              << std::endl;
    std::cerr << "    <x>, <y>, <z>   : Number of pixels in x, y and z direction." << std::endl;
    std::cerr << "    <type>          : Type (2:int8 3:int8 4:int16 5:uint16 6:int32 7:uint32" << std::endl;
    std::cerr << "    <rx>, <ry>, <rz>: resolution (pixel size) in x, y and z direction" << std::endl;
    std::cerr << "    <fname_temp>    : File name template (e.g. \"RawImage_\%04d.raw\")." << std::endl;
    std::cerr << "    <bindex>        : Begin index." << std::endl;
    std::cerr << "    <eindex>        : End  index." << std::endl;
    std::cerr << "    <fps>           : Frame rate (frames per second)." << std::endl;
    std::cerr << "    <hostname>      : hostname (port# is fixed to 18944)" << std::endl;
    exit(-1);
  }

  int size[3];
  size[0] = atoi(argv[1]);
  size[1] = atoi(argv[2]);
  size[2] = atoi(argv[3]);
  int type = atoi(argv[4]);
  
  float spacing[3];
  spacing[0] = atoi(argv[5]);
  spacing[1] = atoi(argv[6]);
  spacing[2] = atoi(argv[7]);

  char* filenameTemp = argv[8];
  int bindex = atoi(argv[9]);
  int eindex = atoi(argv[10]);
  float fps  = atof(argv[11]);
  char* host = argv[12];

  int nframe = eindex - bindex + 1;
  int psize = 0;

  if (type == 2 || type == 3) {
    psize = 1;
  } else if (type == 4 || type == 5) {
    psize = 2;
  } else {
    psize = 4;
  }

  int interval_ms = (int) (1000 / fps);

  ScannerSim* scanner = new ScannerSim();
  int r = scanner->LoadImageData(filenameTemp, bindex, eindex,
                                 type, size, spacing);
  if (r == 0) {
    std::cerr << "Data load error" << std::endl;
    exit(-1);
  }

  bool stop      = 1;
  bool connected = false;
  mutex = new ACE_Thread_Mutex;

  connectToSlicer(host, 18944);
  session(scanner,  interval_ms);

    // Stop thread
#ifdef WIN32
  ACE_Thread::join( (ACE_thread_t*)thread );
#else
  ACE_Thread::join( (ACE_thread_t)thread );
#endif
  delete ((ACE_thread_t *)thread);

  disconnect();
  delete mutex;

}

