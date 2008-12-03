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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <newmat/newmatap.h>

/*
#include <ace/OS.h>
#include <ace/Time_Value.h>
*/

#include "igtlImageMessage.h"
#include "AcquisitionGEExcite.h"
#include "igtlMath.h"
#include "igtlutil/igtl_util.h"


#define FLOWCOMP_REALTIME 0x01
#define SPGR_REALTIME 0x2
#define FATSAT_REALTIME 0x4
#define SPATIAL_SAT_REALTIME 0x8
#define IR_REALTIME 0x10
#define FOV_REALTIME 0x20
#define SWAP_PF_REALTIME 0x40
#define AUTO_NEX_REALTIME 0x80
#define ROTATE_REALTIME 0x100
#define TRANSLATE_REALTIME 0x200
#define FOV_ADDINFO_REALTIME 0x400
#define SLTHICK_REALTIME 0x800
#define FLIP_REALTIME 0x1000
#define SPECTRAL_SPATIAL_REALTIME 0x2000
#define HIRES3D_REALTIME 0x4000
#define TAGGING_TYPE_RT 0x8000
#define TAGGING_ANGLE_RT 0x10000


#ifdef _RSP_CONTROL
extern "C" {
  int rsp_init(int*,char**);
  int read_rsp_s(char*,int*);
  int read_rsp_i(char*,int*);
  int read_rsp_f(char*,float*);
  int set_rsp_s(char*,int*);
  int set_rsp_i(char*,int*);
  int set_rsp_f(char*,float*);
}
#endif


AcquisitionGEExcite::AcquisitionGEExcite()
{
  this->CurrentFrameIndex = 0;
  this->CurrentFrameSliceIndex = 0;
  this->imageArray.clear();
  this->Interval_ms = 100;  // default
  this->CurrentFrame = igtl::ImageMessage::New();
  //this->gemutex = new ACE_Thread_Mutex;
  this->Mutex = igtl::MutexLock::New();
  this->RetMatrixMutex = igtl::MutexLock::New();
  this->RspMutex = igtl::MutexLock::New();
  this->Time = igtl::TimeStamp::New();
  this->channels = 1;
  this->viewsxfer = 1;
  this->validate = false;

  this->Address = "";
  this->Port    = -1;

  this->Delay_s = 0.0;

  while (!this->ScanPlaneBuffer.empty())
    {
    this->ScanPlaneBuffer.pop();
    }

  this->RetMatrixMutex->Lock();
  this->RetMatrix[0][0] = 1.0;
  this->RetMatrix[1][0] = 0.0;
  this->RetMatrix[2][0] = 0.0;

  this->RetMatrix[0][1] = 0.0;
  this->RetMatrix[1][1] = 1.0;
  this->RetMatrix[2][1] = 0.0;

  this->RetMatrix[0][2] = 0.0;
  this->RetMatrix[1][2] = 0.0;
  this->RetMatrix[2][2] = 1.0;

  this->RetMatrix[0][3] = 0.0;
  this->RetMatrix[1][3] = 0.0;
  this->RetMatrix[2][3] = 0.0;
  this->RetMatrixMutex->Unlock();

}


AcquisitionGEExcite::~AcquisitionGEExcite()
{
  this->Mutex->Delete();
  this->RetMatrixMutex->Delete();
}


int AcquisitionGEExcite::Init()
{
  int argc = 1;
  char* argv[] = {"GEExciteIGTL"};

  this->raw_data=(short**)malloc(this->channels*sizeof(short*));
  
  /* Init rsp_modifier proxy */

#ifdef _RSP_CONTROL
  this->RspMutex->Lock();
  if(rsp_init(&argc,argv)==-1) {
    std::cout << "ERROR: No luck initializing the RSP modifier proxy\n" << std::endl;
    this->RspMutex->Unlock();
    return 0;
  }
  else
  printf("rsp_init successful\n");
  this->RspMutex->Unlock();

  /* Init SVAT interface */
  /*svat_driver_init((char*)console_host.c_str());*/
#endif // _RSP_CONTROL

  return 1;
}


int AcquisitionGEExcite::StartScan()
{
  this->Connect();
  this->Run();
  return 1;
}


int AcquisitionGEExcite::PauseScan()
{
  this->Sleep();
  return 1;
}


int AcquisitionGEExcite::StopScan()
{
  this->m_Stop = 0;
  this->Stop();
  return 1;
}


int AcquisitionGEExcite::SetMatrix(float* rmatrix)
{

  std::cerr << "AcquisitionGEExcite::SetMatrix() called." << std::endl;

  igtl::Matrix4x4 matrix;
  matrix[0][0] = rmatrix[0]; 
  matrix[1][0] = rmatrix[1]; 
  matrix[2][0] = rmatrix[2]; 
  matrix[0][1] = rmatrix[3]; 
  matrix[1][1] = rmatrix[4]; 
  matrix[2][1] = rmatrix[5]; 
  matrix[0][2] = rmatrix[6]; 
  matrix[1][2] = rmatrix[7]; 
  matrix[2][2] = rmatrix[8]; 
  matrix[0][3] = rmatrix[9]; 
  matrix[0][3] = rmatrix[10];
  matrix[0][3] = rmatrix[11];

  SetMatrix(matrix);

}


int AcquisitionGEExcite::SetMatrix(igtl::Matrix4x4& m)
{

  // push matrix and time stamp data into the queue
  ScanPlaneType sp;
  this->RetMatrixMutex->Lock();
  sp.matrix[0][0] = m[0][0];
  sp.matrix[1][0] = m[1][0];
  sp.matrix[2][0] = m[2][0];
  sp.matrix[0][1] = m[0][1];
  sp.matrix[1][1] = m[1][1];
  sp.matrix[2][1] = m[2][1];
  sp.matrix[0][2] = m[0][2];
  sp.matrix[1][2] = m[1][2];
  sp.matrix[2][2] = m[2][2];
  sp.matrix[0][3] = m[0][3];
  sp.matrix[1][3] = m[1][3];
  sp.matrix[2][3] = m[2][3];
  this->Time->GetTime();
  sp.ts = this->Time->GetTimeStamp();
  this->ScanPlaneBuffer.push(sp);
  this->RetMatrixMutex->Unlock();

  igtl::Matrix4x4 matrix;
  float position[3];

  position[0] = m[0][3];
  position[1] = m[1][3];
  position[2] = m[2][3];

  matrix[0][0] = m[0][0];
  matrix[1][0] = m[1][0];
  matrix[2][0] = m[2][0];

  matrix[0][1] = m[0][1];
  matrix[1][1] = m[1][1];
  matrix[2][1] = m[2][1];

  matrix[0][2] = m[0][2];
  matrix[1][2] = m[1][2];
  matrix[2][2] = m[2][2];


#ifdef _RSP_CONTROL

  // check if frequency and phase encodings are flipped.
  int swap = 1;
  this->RspMutex->Lock();
  read_rsp_i("cont_swap_pf",&swap);
  //set_rsp_i("cont_swap_pf",&swap);
  this->RspMutex->Unlock();
  
  //if(swap)
  //  {
  //    // flipped
  //  }
  //else
  //  {
  //    // not flipped
  //  }

  /* from RAS to image */
  /* inv(R)*T */
  float Px, Py, Pz;
  int Ix, Iy, Iz;

  Px=matrix[0][0]*(position[0])+matrix[1][0]*(position[1])+matrix[2][0]*(position[2]);
  Py=matrix[0][1]*(position[0])+matrix[1][1]*(position[1])+matrix[2][1]*(position[2]);
  Pz=matrix[0][2]*(position[0])+matrix[1][2]*(position[1])+matrix[2][2]*(position[2]);

  /*
  Px=position[0];
  Py=position[1];
  Pz=position[2];
  */

  Ix=lrintf(Px);
  Iy=lrintf(Py);
  Iz=lrintf(Pz);

  int feature_available;
  // Check which feature is available for  real-time RSP updaate
  this->RspMutex->Lock();
  read_rsp_i("cont_avail_flag",&feature_available);
  this->RspMutex->Unlock();


  if(feature_available & TRANSLATE_REALTIME)
    {
      this->RspMutex->Lock();
      /*
      set_rsp_i("cont_GWRloc",&px);
      set_rsp_i("cont_GWPloc",&py);
      set_rsp_i("cont_GWTloc",&pz);
      */
      set_rsp_i("cont_GWRloc",&Ix);
      set_rsp_i("cont_GWPloc",&Iy);
      set_rsp_i("cont_GWTloc",&Iz);
  
      int fov;
      /* read field-of-view */
      read_rsp_i("cont_fov",&fov);
      /*
      event.setAttribute("FOV",fov);
      MathUtils::quaternionToMatrix(orientation,matrix);
      */
      
      /* from image to RAS */
      /* T*R */
      float value = matrix[0][0]*fov/2+matrix[0][1]*fov/2+position[0];
      set_rsp_f("cont_image_p0x",&value);
      value = matrix[1][0]*fov/2+matrix[1][1]*fov/2+position[1];
      set_rsp_f("cont_image_p0y",&value);
      value = matrix[2][0]*fov/2+matrix[2][1]*fov/2+position[2];
      set_rsp_f("cont_image_p0z",&value);
      
      value = matrix[0][0]*(-fov/2)+matrix[0][1]*fov/2+position[0];
      set_rsp_f("cont_image_p1x",&value);
      
      value = matrix[1][0]*(-fov/2)+matrix[1][1]*fov/2+position[1];
      set_rsp_f("cont_image_p1y",&value);
      
      value = matrix[2][0]*(-fov/2)+matrix[2][1]*fov/2+position[2];
      set_rsp_f("cont_image_p1z",&value);
      
      value = matrix[0][0]*(-fov/2)+matrix[0][1]*(-fov/2)+position[0];
      set_rsp_f("cont_image_p2x",&value);
      
      value = matrix[1][0]*(-fov/2)+matrix[1][1]*(-fov/2)+position[1];
      set_rsp_f("cont_image_p2y",&value);
      
      value = matrix[2][0]*(-fov/2)+matrix[2][1]*(-fov/2)+position[2];
      set_rsp_f("cont_image_p2z",&value);

      this->RspMutex->Unlock();
    }
  else
    {
      std::cout << "PDS does not support feature TRANSLATE_REALTIME" << std::endl;
    }


  if(feature_available & ROTATE_REALTIME)
    {
      this->RspMutex->Lock();

      //// rotation
      
      //float value = (float)matrix[0][0];
      //set_rsp_f("cont_Rot00",&value);
      //
      //value = (float)matrix[1][0];
      //set_rsp_f("cont_Rot01",&value);
      //
      //value = (float)matrix[2][0];
      //set_rsp_f("cont_Rot02",&value);
      //
      //value = (float)matrix[0][1];
      //set_rsp_f("cont_Rot10",&value);
      //
      //value = (float)matrix[1][1];
      //set_rsp_f("cont_Rot11",&value);
      //
      //value = (float)matrix[2][1];
      //set_rsp_f("cont_Rot12",&value);
      //
      //value = (float)matrix[0][2];
      //set_rsp_f("cont_Rot20",&value);
      //
      //value = (float)matrix[1][2];
      //set_rsp_f("cont_Rot21",&value);
      //
      //value = (float)matrix[2][2];
      //set_rsp_f("cont_Rot22",&value);

      // rotation
      float value = (float)matrix[0][0];
      set_rsp_f("cont_Rot00",&value);
      
      value = (float)matrix[1][0];
      set_rsp_f("cont_Rot10",&value);
      
      value = (float)matrix[2][0];
      set_rsp_f("cont_Rot20",&value);
      
      value = (float)matrix[0][1];
      set_rsp_f("cont_Rot01",&value);
      
      value = (float)matrix[1][1];
      set_rsp_f("cont_Rot11",&value);
      
      value = (float)matrix[2][1];
      set_rsp_f("cont_Rot21",&value);
      
      value = (float)matrix[0][2];
      set_rsp_f("cont_Rot02",&value);
      
      value = (float)matrix[1][2];
      set_rsp_f("cont_Rot12",&value);
      
      value = (float)matrix[2][2];
      set_rsp_f("cont_Rot22",&value);

      this->RspMutex->Unlock();
    }
  else
    {
      std::cout << "PSD does not support feature ROTATE_REALTIME" << std::endl;
    }

#endif //_RSP_CONTROL

  return 1;
}



void AcquisitionGEExcite::SetRdsHost(std::string host, int port)
{
  //this->address.set(port, host.c_str());
  this->Address = host;
  this->Port = port;
}

void AcquisitionGEExcite::SetConsolHost(std::string address)
{
  /* used for SVAT */
}

void AcquisitionGEExcite::SetRevision(int revision)
{
  if (revision == REV_12_X)
    {
      this->bigendian = true;
    }
  else
    {
      this->bigendian = false;
    }
}

void AcquisitionGEExcite::SetChannels(int ch)
{
  this->channels = ch;

}

void AcquisitionGEExcite::SetViewsXfer(int v)
{
  this->viewsxfer = v;
}

void AcquisitionGEExcite::SetValidate(bool s)
{
  this->validate = s;
}

void AcquisitionGEExcite::SetLineOrder(std::string l)
{
  this->lineorder = l;
}

void AcquisitionGEExcite::SetDelay(int d) // in ms
{
  if (d > 0)
    {
      this->Delay_s = (double)d / 1000.0;
    }
}



void AcquisitionGEExcite::Process()
{
  igtl::ImageMessage::Pointer frame;
  this->SetCircularFrameBufferSize(3);

  //ACE_DEBUG((LM_INFO, ACE_TEXT("ot:starting Raw Data Server module thread - tt\n")));
  std::cerr << "starting Raw Data Server module thread" << std::endl;
  
  //ACE_Time_Value timeOut( 100, 0 );
  //int retval;
  int server_ready=0;
  RDS_MSG_HDR header;
  RDS_RAW_READY_PKT packet;
  int opcode;
  int packet_len;
  unsigned int totalSize;
  byte datakey_waitfornext = 0;
  unsigned long datakey = 0;

#ifdef _RSP_CONTROL
  IMAGE_INFO imgInfo;
#endif


  /*
  float tx;
  float ty;
  float tz;
  float sx;
  float sy;
  float sz;
  float nx;
  float ny;
  float nz;
  float px;
  float py;
  float pz;
  */
  
#ifdef USE_64_BIT
  unsigned long trans_bytes = 0;
#else
  unsigned int trans_bytes = 0;
#endif

  for(int c=0;c<channels;c++)
    {
      R[c]=new Matrix();
      I[c]=new Matrix();
    }

  this->full_data=(short**)malloc(channels*sizeof(short*));

  int row=0;
  int pass=0;
  xsize=0;
  ysize=0;
  
  for(int c=0;c<this->channels;c++)
    raw_data[c]=NULL;

  this->m_Stop = 0;

  //ACE_DEBUG((LM_INFO, ACE_TEXT("Start looping....\n")));
  std::cerr << "Start looping..." << std::endl;
  while (!this->m_Stop)
    {
      // Get header
      /*
      if((retval = socket.recv_n( &header, sizeof(RDS_MSG_HDR), &timeOut,(size_t*)&trans_bytes )) == -1 )
        {
          if( errno != ETIME && errno != 0 )
            {
              ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d receiving header data !\n"), errno));
              exit( -1 );
            }
        }
      */
      if (this->ClientSocket->Receive(&header, sizeof(RDS_MSG_HDR)) != sizeof(RDS_MSG_HDR))
        {
          std::cerr << "Irregluar size." << std::endl;
          exit(-1);
        }

      if(bigendian)
        {
          /*
          packet_len=ntohl(header.pktlen);
          opcode = ntohl(header.opcode);
          */
          packet_len=BYTE_SWAP_INT32(header.pktlen);
          opcode = BYTE_SWAP_INT32(header.opcode);
        }
      else
        {
          packet_len=header.pktlen;
          opcode = header.opcode;
        }
      if(opcode==OP_RDS_PREPPED)
        {
          datakey_waitfornext=1;
        }
      
      if(opcode==OP_RDS_RAW_READY)
        {
          // Get Packet
          /*
          if((retval = socket.recv_n( &packet, sizeof(RDS_RAW_READY_PKT), &timeOut, (size_t*)&trans_bytes )) == -1 )
            {
              if( errno != ETIME && errno != 0 )
                {
                  ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d receiving packet data !\n"), errno));
                  exit( -1 );
                }
            }
          */
          if (this->ClientSocket->Receive(&packet, sizeof(RDS_RAW_READY_PKT)) != sizeof(RDS_RAW_READY_PKT))
            {
              std::cerr << "Error: receiving packet data." << std::endl;
              exit(-1);
            }

          //                      std::cout << " revieved aa; " << retval << std::endl;
          /* ...and then convert to host byte order. */
          if(bigendian)
            {
              /*
              packet.id = ntohl(packet.id);
              packet.timestamp = ntohl(packet.timestamp);
              packet.seq_num = ntohl(packet.seq_num);
              packet.raw_cs = ntohl(packet.raw_cs);
              packet.msg_cs = ntohl(packet.msg_cs);
              packet.raw_size = ntohl(packet.raw_size);
              packet.msg_size = ntohl(packet.msg_size);
              packet.aps_buff_addr = ntohl(packet.aps_buff_addr);
              packet.aps_buff_size = ntohl(packet.aps_buff_size);
              packet.start_offset = ntohl(packet.start_offset);
              packet.control_flags = ntohl(packet.control_flags);
              */
              packet.id = BYTE_SWAP_INT32(packet.id);
              packet.timestamp = BYTE_SWAP_INT32(packet.timestamp);
              packet.seq_num = BYTE_SWAP_INT32(packet.seq_num);
              packet.raw_cs = BYTE_SWAP_INT32(packet.raw_cs);
              packet.msg_cs = BYTE_SWAP_INT32(packet.msg_cs);
              packet.raw_size = BYTE_SWAP_INT32(packet.raw_size);
              packet.msg_size = BYTE_SWAP_INT32(packet.msg_size);
              packet.aps_buff_addr = BYTE_SWAP_INT32(packet.aps_buff_addr);
              packet.aps_buff_size = BYTE_SWAP_INT32(packet.aps_buff_size);
              packet.start_offset = BYTE_SWAP_INT32(packet.start_offset);
              packet.control_flags = BYTE_SWAP_INT32(packet.control_flags);
            }

          //std::cerr << "packet.id             " << packet.id            << std::endl;
          //std::cerr << "packet.timestamp      " << packet.timestamp     << std::endl;
          //std::cerr << "packet.seq_num        " << packet.seq_num       << std::endl;
          //std::cerr << "packet.raw_cs         " << packet.raw_cs        << std::endl;
          //std::cerr << "packet.msg_cs         " << packet.msg_cs        << std::endl;
          //std::cerr << "packet.raw_size       " << packet.raw_size      << std::endl;
          //std::cerr << "packet.msg_size       " << packet.msg_size      << std::endl;
          //std::cerr << "packet.aps_buff_addr  " << packet.aps_buff_addr << std::endl;
          //std::cerr << "packet.aps_buff_size  " << packet.aps_buff_size << std::endl;
          //std::cerr << "packet.start_offset   " << packet.start_offset  << std::endl;
          //std::cerr << "packet.control_flags  " << packet.control_flags << std::endl;
          

          totalSize = packet.raw_size + packet.msg_size;
          
          if(datakey != packet.seq_num) {
            if(datakey_waitfornext) {
              datakey = packet.seq_num;
              datakey_waitfornext = 0;
            }
            else {
              printf("Notification packet out of sync. %ld != %ld\n",
                     datakey, packet.seq_num);
              exit(1);
            }
          }
          datakey++;

          // Get Packet
          if(totalSize>0)
            {
              this->pByteArray=(byte*)malloc((size_t)totalSize);
              if (this->ClientSocket->Receive(this->pByteArray, totalSize) != totalSize)
                {
                  std::cerr << "Error: receiving packet data!" << std::endl;
                  exit(-1);
                }
              /*
              if((retval = socket.recv_n( this->pByteArray, totalSize, &timeOut, (size_t*)&trans_bytes )) == -1 )
                {
                  if( errno != ETIME && errno != 0 )
                    {
                      ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d receiving packet data !\n"), errno));
                      exit( -1 );
                    }
                }
              */
            }
          
          if(this->validate)
            {
              if(packet.msg_cs!=validateData(&packet))
                std::cout << " message checksum error " << std::endl;
            }
          else /*validateData swaps the bytes, so we have to do it here */
            {
              int numShortPoints = packet.raw_size / sizeof(short);
              short *sip = (short*)this->pByteArray;
              if(bigendian)
                
                for(int i = 0; i < numShortPoints; i++) {
                  //sip[i] = ntohs(sip[i]);
                  sip[i] = BYTE_SWAP_INT16(sip[i]);
                }
              
            }
          if(packet.control_flags & RDS_DATA_SPLIT)
            std::cout << "RDS_DATA_SPLIT" << std::endl;
          if(packet.control_flags & RDS_DATA_SEND_VIEW_COUNT)
            {
              if(this->write_raw_data)
                this->filehandle.write(this->pByteArray,packet.raw_size);
              //gemutex->acquire();
              this->Mutex->Lock();
              xsize = packet.raw_size / (sizeof(short)*2*channels*this->viewsxfer);
              for(int c=0;c<channels;c++)
                {
                  if(row+viewsxfer>ysize)
                    raw_data[c]=(short*)realloc(raw_data[c],xsize*(row+viewsxfer)*2*sizeof(short));
                  for(int k=0;k<this->viewsxfer;k++)
                    memcpy(raw_data[c]+(k+row)*xsize*2,this->pByteArray+(c+k*channels)*xsize*sizeof(short)*2,xsize*sizeof(short)*2);
                }
              //gemutex->release();
              this->Mutex->Unlock();
              free(this->pByteArray);
              row+=this->viewsxfer;

#ifdef _RSP_CONTROL
              this->RspMutex->Lock();
//              read_rsp_f("cont_xoffset",   &imgInfo.xoffset  );
//              read_rsp_f("cont_yoffset",   &imgInfo.yoffset  );
//              read_rsp_f("cont_zoffset",   &imgInfo.zoffset  );
//              read_rsp_f("cont_alpha",     &imgInfo.alpha    );
//              read_rsp_f("cont_beta",      &imgInfo.beta     );
//              read_rsp_f("cont_gamma",     &imgInfo.gamma    );
              read_rsp_f("cont_slthick",   &imgInfo.slthick  );
//              //read_rsp_i("cont_time_sec",  &imgInfo.time_sec );
//              //read_rsp_i("cont_time_usec", &imgInfo.time_usec);
//              //read_rsp_i("cont_rtia_mode", &imgInfo.rtia_mode);
              read_rsp_i("cont_fov",       &imgInfo.fov      );
              read_rsp_i("cont_TR",        &imgInfo.tr       );
              read_rsp_i("cont_TI",        &imgInfo.ti       );
              read_rsp_i("cont_TE",        &imgInfo.te       );
//              read_rsp_i("cont_rotate",    &imgInfo.rotate   );
//              read_rsp_i("cont_transpose", &imgInfo.transpose);
//              read_rsp_f("cont_image_p0x", &imgInfo.image_p0x);
//              read_rsp_f("cont_image_p0y", &imgInfo.image_p0y);
//              read_rsp_f("cont_image_p0z", &imgInfo.image_p0z);
//              read_rsp_f("cont_image_p1x", &imgInfo.image_p1x);
//              read_rsp_f("cont_image_p1y", &imgInfo.image_p1y);
//              read_rsp_f("cont_image_p1z", &imgInfo.image_p1z);
//              read_rsp_f("cont_image_p2x", &imgInfo.image_p2x);
//              read_rsp_f("cont_image_p2y", &imgInfo.image_p2y);
//              read_rsp_f("cont_image_p2z", &imgInfo.image_p2z);
              this->RspMutex->Unlock();
#endif // _RSP_CONTROL

            }
          if(packet.control_flags & RDS_DATA_SEND_EOP)
            {
              /*

              for (NodeVector::iterator it = sources.begin(); it != sources.end(); it ++ )
                {
                  GEExciteSource *source = dynamic_cast<GEExciteSource*>((Node*)*it);
                  gemutex->acquire();
                  source->modified=1;
                  gemutex->release();
                }
              */
              pass++;
              //gemutex->acquire();
              this->Mutex->Lock();
              ysize = row;
              //gemutex->release();
              this->Mutex->Unlock();
              row=0;

              //std::cerr << "xsize = " << xsize << std::endl;
              //std::cerr << "ysize = " << xsize << std::endl;

              if (this->PostProcessThread)
                {
                  int dim[3];
                  dim[0] = xsize;
                  dim[1] = ysize;
                  dim[2] = 1;

                  float spacing[3];
#ifdef _RSP_CONTROL
                  if (xsize > 0 && ysize > 0)
                    {
                      spacing[0] = (float)imgInfo.fov/xsize;
                      spacing[1] = (float)imgInfo.fov/ysize;
                      spacing[2] = (float)imgInfo.slthick;
                    }
#else
                  spacing[0] = 1.0;
                  spacing[1] = 1.0;
                  spacing[2] = 5.0;
#endif

                  int off[3];
                  off[0] = 0;
                  off[1] = 0;
                  off[2] = 0;

                  this->CurrentFrame->SetDimensions(dim);
                  this->CurrentFrame->SetSpacing(spacing);
                  this->CurrentFrame->SetSubVolume(dim, off);
                  this->CurrentFrame->SetScalarType(4);
                  this->CurrentFrame->SetDeviceName("Scanner");
                  this->CurrentFrame->AllocateScalars();

                  for (int c = 0; c < channels; c ++)
                    {
                      Reconstruct(xsize, ysize, channels, c, raw_data[c],
                                  (short*)this->CurrentFrame->GetScalarPointer());
                    }

                  //int scalarSize = this->CurrentFrame->GetScalarSize();
                  //GetScanPlane(matrix);
                  GetDelayedTransform(this->RetMatrix);
                  this->RetMatrixMutex->Lock();
                  this->CurrentFrame->SetMatrix(this->RetMatrix);
                  this->RetMatrixMutex->Unlock();

                  this->CurrentFrame->Pack();
                  int id = this->PutFrameToBuffer(static_cast<igtl::MessageBase::Pointer>(this->CurrentFrame));
                  std::cerr << "Process(): frame in bffer #" <<  id << std::endl;
                  this->PostProcessThread->PullTrigger((void*)id);
                }
            }

          if(packet.control_flags & RDS_DATA_SEND_EOS)
            std::cout << "RDS_DATA_SEND_EOS" << std::endl;
          if(packet.control_flags & RDS_DATA_SEND_NEWPATH)
            std::cout << "RDS_DATA_SEND_NEWPATH" << std::endl;
          
          
        }
    }

  if(this->write_raw_data)
    this->filehandle.close();
  //socket.close();
  this->ClientSocket->CloseSocket();
  this->ClientSocket->Delete();
  //ACE_DEBUG((LM_INFO, ACE_TEXT("ot:Stopping thread\n")));
  std::cerr << "Stopping thread" << std::endl;

}


int AcquisitionGEExcite::Connect()
{
  INIT_MSG init_package;
  init_package.rds_init_pkt.views_per_xfer = this->viewsxfer;
  std::cout << "viewsxfer " << viewsxfer << std::endl;
  init_package.rds_init_pkt.aqs_pkt_flags = 0;
  init_package.rds_init_pkt.xfer_mode = 0;
  init_package.rds_init_pkt.host_buff_addr = 0;
  init_package.rds_init_pkt.host_buff_size = 0;
  init_package.rds_msg_hdr.opcode = OP_RDS_INIT;
  init_package.rds_msg_hdr.pktlen = sizeof(init_package.rds_init_pkt);
  if(bigendian)
    {
      std::cout << " swap the bytes " << std::endl;
      /*
      init_package.rds_init_pkt.views_per_xfer = htonl(init_package.rds_init_pkt.views_per_xfer);
      init_package.rds_init_pkt.aqs_pkt_flags = htonl(init_package.rds_init_pkt.aqs_pkt_flags);
      init_package.rds_init_pkt.xfer_mode = htonl(init_package.rds_init_pkt.xfer_mode);
      init_package.rds_init_pkt.host_buff_addr = htonl(init_package.rds_init_pkt.host_buff_addr);
      init_package.rds_init_pkt.host_buff_size = htonl(init_package.rds_init_pkt.host_buff_size);
      init_package.rds_msg_hdr.opcode = htonl(init_package.rds_msg_hdr.opcode);
      init_package.rds_msg_hdr.pktlen = htonl(init_package.rds_msg_hdr.pktlen);
      */
      init_package.rds_init_pkt.views_per_xfer = BYTE_SWAP_INT32(init_package.rds_init_pkt.views_per_xfer);
      init_package.rds_init_pkt.aqs_pkt_flags = BYTE_SWAP_INT32(init_package.rds_init_pkt.aqs_pkt_flags);
      init_package.rds_init_pkt.xfer_mode = BYTE_SWAP_INT32(init_package.rds_init_pkt.xfer_mode);
      init_package.rds_init_pkt.host_buff_addr = BYTE_SWAP_INT32(init_package.rds_init_pkt.host_buff_addr);
      init_package.rds_init_pkt.host_buff_size = BYTE_SWAP_INT32(init_package.rds_init_pkt.host_buff_size);
      init_package.rds_msg_hdr.opcode = BYTE_SWAP_INT32(init_package.rds_msg_hdr.opcode);
      init_package.rds_msg_hdr.pktlen = BYTE_SWAP_INT32(init_package.rds_msg_hdr.pktlen);
    }
  
  int retval;
  //ACE_Time_Value timeOut(1,0);
  
  std::cout << "ready to connect" << std::endl;
  
  //retval = connector.connect(socket, address, &timeOut);
  if (this->Port > 0)
    {
      this->ClientSocket = igtl::ClientSocket::New();
      retval = this->ClientSocket->ConnectToServer(this->Address.c_str(), this->Port);
      if (retval == 0)
        {
          std::cerr << "Connected" << std::endl;;
        }
      else
        {
          std::cerr << "Error : connection failed." << std::endl;
          this->ClientSocket->Delete();
          return -1;
        }
    }
  else
    {
      return -1;
    }

  
  /*
  if(retval == -1 && errno != ETIME && errno != 0 )
    {
      ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d connection failed for socket nr.: %d\n"), errno));
      return -1;
    }
  else
    {
      ACE_DEBUG((LM_INFO, ACE_TEXT("ot:connected to socket nr.: %d - sending GO command\n")));
    }
  */
  
  //retval = socket.send_n(&init_package,sizeof(INIT_MSG),&timeOut);
  retval = this->ClientSocket->Send(&init_package, sizeof(INIT_MSG));
  /*
  if(retval == -1 && errno != ETIME && errno != 0 )
    {
      ACE_DEBUG((LM_ERROR, ACE_TEXT("Error %d sending data!\n"), errno));
      exit(-1);
    }
  */
  if (retval == 0)
    {
      std::cerr << "Error : Connection Lost!\n";
      exit(-1);
    }

  
  return 0;
}


int AcquisitionGEExcite::validateData(RDS_RAW_READY_PKT * rdsRawReadyPkt)
{
  unsigned int numShortPoints = 0; /* number of shorts in raw data */
  unsigned int numIntPoints = 0;   /* number of ints in raw data */
  unsigned int * uip;
  short * sip;
  unsigned int offset = 0;
  
  /* Note: uses global variable "pByteArray". */
  
  unsigned int i;
  unsigned int tempval = 0;
  unsigned int checksum = 0;
  
  if(rdsRawReadyPkt == NULL) {
    printf("RDS_RAW_READY_PKT pointer was NULL.\n");
      return(1);
  }
  
  /* pByteArray contains raw data followed by AQS messages (if any). */
  /* While the raw data consists of 16 bit ints, the checksum is     */
  /* computed by interpreting the data as 32 bit ints.               */
  /* So, first we compute the checksum (32 bit) and then convert the */
  /* pByteArray is host order (16 bit). */
  
  /* The number of 32 bit uints in the raw data. */
  numIntPoints = rdsRawReadyPkt->raw_size / sizeof(int);
  
  /* Step through the array and calculate the checksum. */
  uip = (unsigned int *) pByteArray;
  for(i = 0; i < numIntPoints; i++) {
    if(bigendian) {
      /*checksum += ntohl(uip[i]);*/
      checksum += BYTE_SWAP_INT32(uip[i]);
    } else {
      checksum +=uip[i];
    }
  }
  
  if(checksum != rdsRawReadyPkt->raw_cs) {
    printf("Raw Computed != rdsRawReadyPkt checksum.\n");
    return(1);
  }
  
  /* Raw data checksum is valid.  Now check AQS message block. */
  
  checksum = 0;
  
  /* Save the number of ints in the raw data. Then compute the */
  /* number of ints in the message block */
  offset = numIntPoints;
  numIntPoints = rdsRawReadyPkt->msg_size / sizeof(int);
  
  /* Calculate the checksum.  Don't forget the offset! */
  for(i = 0; i < numIntPoints; i++) {
    if(bigendian) {
      //checksum += ntohl(uip[offset + i]);
      checksum += BYTE_SWAP_INT32(uip[offset + i]);
    } else {
      checksum += uip[offset+i];
    }
  }
  
  if(checksum != rdsRawReadyPkt->msg_cs) {
    printf("Msg Computed != rdsRawReadyPkt checksum.\n");
    return(1);
  }
  
  /* Message data checksum is valid.  Now byte swap raw data. */
  /* NOTE: AQS message block is not converted. */
  
  /* Number of ushorts in raw data. */
  numShortPoints = rdsRawReadyPkt->raw_size / sizeof(short);
  
  /* Byte swap raw data only. */
  sip = (short *) pByteArray;
  if(bigendian)
    for(i = 0; i < numShortPoints; i++) {
      //sip[i] = ntohs(sip[i]);
      sip[i] = BYTE_SWAP_INT16(sip[i]);
    }
  
  return(0);  /* All done! */
}


void AcquisitionGEExcite::Reconstruct(int xsize, int ysize, int channels, int c, short* raw, short* final_image)
{
  /*
  short **real_data;
  short **imag_data;
  short **raw_data;
  */

  float scalefactor = 1.0;
  
  full_data[c]=(short*) raw;
  
  //    std::cout << "MRIRecon: xsize " << xsize << std::endl;
  //    std::cout << "MRIRecon: ysize " << ysize << std::endl;
  R[c]->ReSize(xsize,ysize);
  I[c]->ReSize(xsize,ysize);
  
  SHORTCOMPLEX* tmp=(SHORTCOMPLEX*)full_data[c];
  if(this->lineorder.compare("out_in")==0)
    {
      for(int i=0;i<xsize/2;i++)
        {
          for(int j=0;j<ysize;j++)
            {
              //                std::cout << "f: " << xsize-1-i << " s: " << (2*i+1)*ysize+j << std::endl;
              R[c]->element(i,j)=tmp[(2*i)*ysize+j].re;
              R[c]->element(xsize-1-i,j)=tmp[(2*i+1)*ysize+j].re;
              I[c]->element(i,j)=tmp[(2*i)*ysize+j].im;
              I[c]->element(xsize-1-i,j)=tmp[(2*i+1)*ysize+j].im;
            }
        }
    }
  
  if(this->lineorder.compare("down_up")==0)
    {
      for(int i=0;i<xsize;i++)
        {
          for(int j=0;j<ysize;j++)
            {
              R[c]->element(i,j)=tmp[i*ysize+j].re;
              I[c]->element(i,j)=tmp[i*ysize+j].im;
            }
        }
    }
  
  
  if(c==(channels-1))
    {
      Matrix imgR;
      Matrix imgI;
      imgR.ReSize(xsize,ysize);
      imgI.ReSize(xsize,ysize);
      //short* final_image = (short*)malloc(xsize*ysize*sizeof(short));
      memset(final_image,0,xsize*ysize*sizeof(short));
      for(int k=0;k<channels;k++)
        {
          FFT2(*(R[k]),*(I[k]),*(R[k]),*(I[k]));
          //        imgR+=*R[k];
          //        imgI+=*I[k];
        }
      if(bigendian)
        {
          for(int k=0;k<channels;k++)
            {
              for(int i=0;i<xsize/2;i++)
                {
                  for(int j=0;j<ysize/2;j++)
                    {
                      final_image[(ysize/2-j-1)*xsize + (xsize/2-i-1)]=(short)(sqrt(pow(R[k]->element(i,j),2)+pow(I[k]->element(i,j),2))/(float)(channels*4)*scalefactor);
                    }
                  for(int j=ysize/2;j<ysize;j++)
                    {
                      final_image[(3*ysize/2-j-1)*xsize + (xsize/2-i-1)]=(short)(sqrt(pow(R[k]->element(i,j),2)+pow(I[k]->element(i,j),2))/(float)(channels*4)*scalefactor);
                    }
                }
              for(int i=xsize/2;i<xsize;i++)
                {
                  for(int j=0;j<ysize/2;j++)
                    {
                      final_image[(ysize/2-j-1)*xsize + (3*xsize/2-i-1)]=(short)(sqrt(pow(R[k]->element(i,j),2)+pow(I[k]->element(i,j),2))/(float)(channels*4)*scalefactor);
                    }
                  
                  for(int j=ysize/2;j<ysize;j++)
                    {
                      final_image[(3*ysize/2-j-1)*xsize + (3*xsize/2-i-1)]=(short)(sqrt(pow(R[k]->element(i,j),2)+pow(I[k]->element(i,j),2))/(float)(channels*4)*scalefactor);
                    }
                }
              
            }
        }               
      else
        {
          
          for(int k=0;k<channels;k++)
            for(int i=0;i<xsize;i++)
              {
                for(int j=0;j<ysize/2;j++)
                  {
                    final_image[(ysize/2-j-1)*xsize+i]+=(short)(sqrt(pow(R[k]->element(i,j),2)+pow(I[k]->element(i,j),2))/((float)(channels*40)*scalefactor));
                  }
                for(int j=ysize/2;j<ysize;j++)
                  {
                    final_image[(3*ysize/2-j-1)*xsize+i]+=(short)(sqrt(pow(R[k]->element(i,j),2)+pow(I[k]->element(i,j),2))/((float)(channels*40)*scalefactor));
                  }
              }
        }
      
    }

}

void AcquisitionGEExcite::GetScanPlane(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 0;
  phi = phi + 0.2;

  position[0] = 0;
  position[1] = 0;
  position[2] = 0;
  
  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  orientation[0]=0.0;
  orientation[1]=0.0;
  orientation[2]=0.0;
  orientation[3]=1.0;
  
  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  //printMatrix(matrix);
}



void AcquisitionGEExcite::GetDelayedTransform(igtl::Matrix4x4& matrix)
{

  std::cerr << "AcquisitionSimulator::GetDelayedTransform(igtl::Matrix4x4& matrix) is called." << std::endl;

  this->RetMatrixMutex->Lock();

  this->Time->GetTime();
  double ts = this->Time->GetTimeStamp() - this->Delay_s;

  while (!this->ScanPlaneBuffer.empty() && this->ScanPlaneBuffer.front().ts < ts)
    {
    std::cerr << "Getting delayed matrix." << std::endl;
    if (this->ScanPlaneBuffer.size() > 1)
      {
      this->ScanPlaneBuffer.pop();
      }
    else
      {
      break;
      }
    }

  if (!this->ScanPlaneBuffer.empty())
    {
    std::cerr << "Getting delayed matrix." << std::endl;

    igtl::Matrix4x4& dm = this->ScanPlaneBuffer.front().matrix;
    
    matrix[0][0] = dm[0][0];
    matrix[1][0] = dm[1][0];
    matrix[2][0] = dm[2][0];
    matrix[0][1] = dm[0][1];
    matrix[1][1] = dm[1][1];
    matrix[2][1] = dm[2][1];
    matrix[0][2] = dm[0][2];
    matrix[1][2] = dm[1][2];
    matrix[2][2] = dm[2][2];
    matrix[0][3] = dm[0][3];
    matrix[0][3] = dm[0][3];
    matrix[0][3] = dm[0][3];
    }

  this->RetMatrixMutex->Unlock();

}
