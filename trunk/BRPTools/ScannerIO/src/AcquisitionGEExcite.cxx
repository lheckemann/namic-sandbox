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

#include <ace/OS.h>
#include <ace/Time_Value.h>

#include "igtlImageMessage.h"
#include "AcquisitionGEExcite.h"

AcquisitionGEExcite::AcquisitionGEExcite()
{
  this->CurrentFrameIndex = 0;
  this->CurrentFrameSliceIndex = 0;
  this->imageArray.clear();
  this->Interval_ms = 100;  // default
  this->CurrentFrame = igtl::ImageMessage::New();
  this->gemutex = new ACE_Thread_Mutex;
}


AcquisitionGEExcite::~AcquisitionGEExcite()
{
  delete gemutex;
}


int AcquisitionGEExcite::Init()
{

  this->raw_data=(short**)malloc(this->channels*sizeof(short*));
  
  /* Init rsp_modifier proxy */
  /*
  if(rsp_init(&argc,argv)==-1) {
    std::cout << "ERROR: No luck initializing the RSP modifier proxy\n" << std::endl;
    return;
  }
  else
  printf("rsp_init successful\n");
  */
  
  /* Init SVAT interface */
  /*  svat_driver_init((char*)console_host.c_str());*/

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

int AcquisitionGEExcite::SetMatrix(float* matrix)
{
  return 1;
}

void AcquisitionGEExcite::SetRdsHost(std::string host, int port)
{
  this->address.set(port, host.c_str());
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

void AcquisitionGEExcite::SetDataOrder(std::string o)
{
  this->data_order = o;
}

void AcquisitionGEExcite::SetLineOrder(std::string l)
{
  this->lineorder = l;
}


void AcquisitionGEExcite::Process()
{
  igtl::ImageMessage::Pointer frame;
  this->SetCircularFrameBufferSize(3);


  ACE_DEBUG((LM_INFO, ACE_TEXT("ot:starting Raw Data Server module thread - tt\n")));
  
  ACE_Time_Value timeOut( 100, 0 );
  int retval;
  int server_ready=0;
  RDS_MSG_HDR header;
  RDS_RAW_READY_PKT packet;
  int opcode;
  int packet_len;
  unsigned int totalSize;
  byte datakey_waitfornext = 0;
  unsigned long datakey = 0;
  
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

  while (!this->m_Stop)
    {
      // Get header
      if((retval = socket.recv_n( &header, sizeof(RDS_MSG_HDR), &timeOut,(size_t*)&trans_bytes )) == -1 )
        {
          if( errno != ETIME && errno != 0 )
            {
              ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d receiving header data !\n"), errno));
              exit( -1 );
            }
        }
      if(bigendian)
        {
          packet_len=ntohl(header.pktlen);
          opcode = ntohl(header.opcode);
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
          if((retval = socket.recv_n( &packet, sizeof(RDS_RAW_READY_PKT), &timeOut, (size_t*)&trans_bytes )) == -1 )
            {
              if( errno != ETIME && errno != 0 )
                {
                  ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d receiving packet data !\n"), errno));
                  exit( -1 );
                }
            }
          //                      std::cout << " revieved aa; " << retval << std::endl;
          /* ...and then convert to host byte order. */
          if(bigendian)
            {
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
            }
          
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
              if((retval = socket.recv_n( this->pByteArray, totalSize, &timeOut, (size_t*)&trans_bytes )) == -1 )
                {
                  if( errno != ETIME && errno != 0 )
                    {
                      ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d receiving packet data !\n"), errno));
                      exit( -1 );
                    }
                }
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
                  sip[i] = ntohs(sip[i]);
                }
              
            }
          if(packet.control_flags & RDS_DATA_SPLIT)
            std::cout << "RDS_DATA_SPLIT" << std::endl;
          if(packet.control_flags & RDS_DATA_SEND_VIEW_COUNT)
            {
              if(this->write_raw_data)
                this->filehandle.write(this->pByteArray,packet.raw_size);
              gemutex->acquire();
              xsize = packet.raw_size / (sizeof(short)*2*channels*this->viewsxfer);
              
              for(int c=0;c<channels;c++)
                {
                  if(row+viewsxfer>ysize)
                    raw_data[c]=(short*)realloc(raw_data[c],xsize*(row+viewsxfer)*2*sizeof(short));
                  for(int k=0;k<this->viewsxfer;k++)
                    memcpy(raw_data[c]+(k+row)*xsize*2,this->pByteArray+(c+k*channels)*xsize*sizeof(short)*2,xsize*sizeof(short)*2);
                }
              gemutex->release();
              free(this->pByteArray);
              row+=this->viewsxfer;
              
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
              gemutex->acquire();
              ysize=row;
              gemutex->release();
              row=0;

              for (int c = 0; c < channels; c ++)
                {
                  Reconstruct(xsize, ysize, channels, c, raw_data[c]);
                }

              // /// //// 


            }
          if(packet.control_flags & RDS_DATA_SEND_EOS)
            std::cout << "RDS_DATA_SEND_EOS" << std::endl;
          if(packet.control_flags & RDS_DATA_SEND_NEWPATH)
            std::cout << "RDS_DATA_SEND_NEWPATH" << std::endl;
          
          
        }
    }

  if(this->write_raw_data)
    this->filehandle.close();
  socket.close();
  ACE_DEBUG((LM_INFO, ACE_TEXT("ot:Stopping thread\n")));

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
      init_package.rds_init_pkt.views_per_xfer = htonl(init_package.rds_init_pkt.views_per_xfer);
      init_package.rds_init_pkt.aqs_pkt_flags = htonl(init_package.rds_init_pkt.aqs_pkt_flags);
      init_package.rds_init_pkt.xfer_mode = htonl(init_package.rds_init_pkt.xfer_mode);
      init_package.rds_init_pkt.host_buff_addr = htonl(init_package.rds_init_pkt.host_buff_addr);
      init_package.rds_init_pkt.host_buff_size = htonl(init_package.rds_init_pkt.host_buff_size);
      init_package.rds_msg_hdr.opcode = htonl(init_package.rds_msg_hdr.opcode);
      init_package.rds_msg_hdr.pktlen = htonl(init_package.rds_msg_hdr.pktlen);
    }
  
  int retval;
  ACE_Time_Value timeOut(1,0);
  
  std::cout << "ready to connect" << std::endl;
  
  retval = connector.connect(socket, address, &timeOut);
  if(retval == -1 && errno != ETIME && errno != 0 )
    {
      ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d connection failed for socket nr.: %d\n"), errno));
      return -1;
    }
  else
    {
      ACE_DEBUG((LM_INFO, ACE_TEXT("ot:connected to socket nr.: %d - sending GO command\n")));
    }
  
  retval = socket.send_n(&init_package,sizeof(INIT_MSG),&timeOut);
  if(retval == -1 && errno != ETIME && errno != 0 )
    {
      ACE_DEBUG((LM_ERROR, ACE_TEXT("Error %d sending data!\n"), errno));
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
    if(bigendian)
      checksum += ntohl(uip[i]);
    else
      checksum +=uip[i];
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
    if(bigendian)
      checksum += ntohl(uip[offset + i]);
    else
      checksum += uip[offset+i];
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
      sip[i] = ntohs(sip[i]);
    }
  
  return(0);  /* All done! */
}


void AcquisitionGEExcite::Reconstruct(int xsize, int ysize, int channels, int c, short* raw)
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
      short* final_image = (short*)malloc(xsize*ysize*sizeof(short));
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




void AcquisitionGEExcite::GetRandomTestMatrix(igtl::Matrix4x4& matrix)
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






