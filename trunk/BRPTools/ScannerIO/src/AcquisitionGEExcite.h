/*=========================================================================

  Program:   Imaging Scanner Interface
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __AcquisitionGEExcite_H
#define __AcquisitionGEExcite_H

#include <vector>
#include <fstream>
#include <map>
#include <queue>
/*
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
*/

/*
#include <ace/Thread.h>
#include <ace/Synch.h>
#include <ace/Thread_Mutex.h>
*/

#include "igtlWin32Header.h"
#include "igtlClientSocket.h"
#include "igtlImageMessage.h"
#include "AcquisitionBase.h"

#include "rds.h"
#include "newmat/newmat.h"

#define byte char
#define OP_RDS_RAW_READY 1
#define OP_RDS_PREPPED 3


typedef std::map<int,Matrix*> MatrixMap;

typedef struct
{
  short I; /// imaginary component of complex number
  short R; /// real component of complex number
} COMPLEX;

typedef struct{
  short re;
  short im;
} SHORTCOMPLEX;

typedef struct
{
  RDS_MSG_HDR rds_msg_hdr;
  RDS_INIT_PKT rds_init_pkt;
} INIT_MSG;

typedef struct
{
  float    xoffset;
  float    yoffset;
  float    zoffset;
  float    alpha;
  float    beta;
  float    gamma;
  float    slthick;
  int      time_sec;
  int      time_usec;
  int      rtia_mode;
  int      fov;
  int      tr;
  int      ti;
  int      te;
  int      rotate;
  int      transpose;
  float    image_p0x;
  float    image_p0y;
  float    image_p0z;
  float    image_p1x;
  float    image_p1y;
  float    image_p1z;
  float    image_p2x;
  float    image_p2y;
  float    image_p2z;
} IMAGE_INFO;

class IGTLCommon_EXPORT AcquisitionGEExcite: public AcquisitionBase
{
public:
  enum {
    REV_12_X,
    REV_14_X
  };
  
public:

  virtual int Init();
  int StartScan();
  int PauseScan();
  int StopScan();
  int SetMatrix(float* matrix);
  int SetMatrix(igtl::Matrix4x4& m);

  void SetRdsHost(std::string host, int port);
  void SetConsolHost(std::string address);
  void SetRevision(int revision);  // REV_12_X or REV_14_X
  void SetChannels(int ch);
  void SetViewsXfer(int v);
  void SetValidate(bool s);
  void SetLineOrder(std::string l);
  void SetDelay(int d); // in ms

public:
  AcquisitionGEExcite();

protected:
  virtual ~AcquisitionGEExcite();
  virtual void Process();

  // for RDS
  int Connect();
  int validateData(RDS_RAW_READY_PKT * rdsRawReadyPkt);
  void Reconstruct(int xsize, int ysize, int channels, int c, short* raw, short* final_image);

  void GetScanPlane(igtl::Matrix4x4& matrix);
  void GetCurrentFrame(igtl::ImageMessage::Pointer& cf);
  
  void GetDelayedTransform(igtl::Matrix4x4& matrix);

protected:

  // for RDS
  int  m_Stop;

  /*
  ACE_INET_Addr address;
  ACE_SOCK_Stream socket;
  ACE_SOCK_Connector connector;
  */
  std::string Address;
  int         Port;
  igtl::ClientSocket::Pointer ClientSocket;
  
  //NodeVector sources;

  byte *pByteArray;
  short **real_data;
  short **imag_data;
  short **raw_data;
  int channels;
  int viewsxfer;
  bool validate;
  bool bigendian;

  int xsize;
  int ysize;
  bool write_raw_data;

  std::string raw_file;
  std::ofstream filehandle;

  // for image reconstrucntion
  MatrixMap R;
  MatrixMap I;
  short **full_data;
  std::string lineorder;

  // for image buffer

  int CurrentFrameIndex;
  int CurrentFrameSliceIndex;
  igtl::ImageMessage::Pointer CurrentFrame;
  std::vector<igtl::ImageMessage::Pointer> imageArray;

  int SubVolumeDimension[3];

  Thread* AcquisitionThread;
  Thread* TransferThread;

  int Interval_ms;

  //ACE_Thread_Mutex* gemutex;
  igtl::MutexLock::Pointer Mutex;
  igtl::Matrix4x4          RetMatrix;
  igtl::MutexLock::Pointer RetMatrixMutex;

  // Position Delay
  double Delay_s;          // delay of image plane control

  typedef struct {
    igtl::Matrix4x4 matrix;  // rotation matrix for slice orientation
    double     ts;           // time stamp
  } ScanPlaneType;

  std::queue<ScanPlaneType> ScanPlaneBuffer;
  igtl::TimeStamp::Pointer Time;
  //igtl::MutexLock::Pointer ScanPlaneMutex;
  igtl::Matrix4x4 CurrentMatrix;

};


#endif // __AcquisitionGEExcite_H


