/*=========================================================================

Module:    $RCSfile: vtkUltrasoundVolumeSender.cxx,v $
Author:  Jonathan Boisvert, Queens School Of Computing
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.
Copyright (c) 2008, Brigham and Women's Hospital, Boston, MA

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

 * Neither the name of Queen's University nor the names of any
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include <iostream>
#include <sstream>
#include <limits>
#include <string>

#define NOMINMAX
#undef REMOVE_ALPHA_CHANNEL
 
//#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"

#include "vtkObjectFactory.h"
#include "vtkUltrasoundVolumeSender.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkVideoSourceSimulator.h"

#include "igtlImageMessage.h"
#include "igtlMath.h"
#include "igtlOSUtil.h"

#define FUDGE_FACTOR 1.6
#define CERTUS_UPDATE_RATE 625

using namespace std;

vtkCxxRevisionMacro(vtkUltrasoundVolumeSender, "$Revision$");
vtkStandardNewMacro(vtkUltrasoundVolumeSender);

//----------------------------------------------------------------------------
vtkUltrasoundVolumeSender::vtkUltrasoundVolumeSender()
{
  this->ServerPort = 18944;
  this->OIGTLServer = NULL; 
  this->SetOIGTLServer("localhost");

  this->TransfertImages = false; 
  
  this->socket = NULL;
  this->socket = igtl::ClientSocket::New();

  // Initialize Transfer Buffer
  this->ImageBuffer = NULL; 
}

//----------------------------------------------------------------------------
vtkUltrasoundVolumeSender::~vtkUltrasoundVolumeSender()
{

  this->SetOIGTLServer(NULL);
}


//----------------------------------------------------------------------------
void vtkUltrasoundVolumeSender::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeSender::ConnectToServer()
{
  // Opening an OpenIGTLink socket
  int e = this->socket->ConnectToServer(OIGTLServer, ServerPort);

  if(e != 0)
    {
    cout << "Failed to connect the OpenIGTLink socket to the server.  Error code : " << e << endl;
    return false;
    }
  else
    cout << "Successful connection to the OpenIGTLink server ("<< OIGTLServer <<":"<<  ServerPort << ")" << endl;
  return true;
}

//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeSender::CloseServerConnection()
{
  this->socket->CloseSocket();
  return true;
}


//----------------------------------------------------------------------------
//McGumbel : This is the Original
//bool vtkUltrasoundVolumeSender::SendImages()
//{
//  // Start the video source and configure an image frame
//  this->sonixGrabber->Record();
//  this->sonixGrabber->Update();
//
//  igtl::ImageMessage::Pointer imMessage = igtl::ImageMessage::New();
//  imMessage->SetDeviceName("V4LVideoSource");
//  vtkImageData *image = this->sonixGrabber->GetOutput();
//  int *dim = image->GetDimensions();
//  double *spacing = image->GetSpacing();
//
//  cout << "Dimensions " << dim[0] << " " << dim[1] << " " << dim[2] << endl;
//  cout << "Spacing " << spacing[0] << " " << spacing[1] << " " << spacing[2] << endl;  
//  cout << "Data dimension : " << image->GetDataDimension() << endl;
//  cout << "Number of scalar components : " << image->GetNumberOfScalarComponents() << endl;
//  cout << "Scalar Type : " << image->GetScalarType() << " ( " << image->GetScalarTypeAsString() << ")" << endl;
//  cout << "Frame count : " << this->sonixGrabber->GetFrameCount() <<endl;
//
//  imMessage->SetDimensions(dim[0], dim[1], 1);
//  imMessage->SetSpacing(spacing[0], spacing[1], spacing[2]);
//  imMessage->SetScalarType(image->GetScalarType());
//  imMessage->AllocateScalars();
//
//  int svd[3];
//  int svoff[3];
//  imMessage->GetSubVolume(svd, svoff);
//
//  cout << "SubVolume dimensions : " << svd[0] << " " << svd[1] << " " << svd[2] << endl;
//  cout << "SubVolume offsets : " << svoff[0] << " " << svoff[1] << " " << svoff[2] << endl;
//
//  // Setup a transform matrix for the OpenIGTLink message
//  igtl::Matrix4x4 igtlMatrix = {{1.0, 0.0, 0.0, 0.0},{0.0, 1.0, 0.0, 0.0},{0.0, 0.0, 1.0, 0.0},{0.0, 0.0, 0.0, 1.0}};
//  vtkMatrix4x4 *vtkMat = vtkMatrix4x4::New();
//  imMessage->SetMatrix(igtlMatrix);
//
//  for( int i=0; i < this->NbFrames; i++)
//    {
//////    igtl::Sleep(1/this->FrameRate*1000);
//    this->tagger->Update();
//
//    if(this->UseTrackerTransforms)
//      {
//      this->tagger->GetTransform()->GetMatrix(vtkMat);
//      for(int i=0;i<4;i++)
//        for(int j=0;j<4;j++)
//          igtlMatrix[i][j]=(*vtkMat)[i][j];
//      igtl::PrintMatrix(igtlMatrix);  
//      imMessage->SetMatrix(igtlMatrix);
//      }
//
//    unsigned char* imgPtr = (unsigned char*)this->tagger->GetOutput()->GetScalarPointer();
////    unsigned char* imgPtr = (unsigned char*) image->GetScalarPointer();
//    int imSize = imMessage->GetImageSize();
//
//    // copy the image raw data 
//    memcpy(imMessage->GetScalarPointer(), imgPtr, imSize);
//    imMessage->Pack();
//
//    // sending this frame to the server
//    std::cout << "PackSize:  " << imMessage->GetPackSize() << std::endl;
//    std::cout << "BodyMode:  " << imMessage->GetBodyType() << std::endl;
//    cout << "Frame color value : " <<(int) ((char *)(image->GetScalarPointer()))[10] << endl;
//
//    int ret = socket->Send(imMessage->GetPackPointer(), imMessage->GetPackSize());
//    if (ret == 0)
//      {
//      std::cerr << "Error : Connection Lost!\n";
//      return false;
//      }
//    else      
//      cout << "image frame send successfully" << endl;
//      
//    igtl::Sleep(10000); //Wait for 10 seconds    
//    
//    }
//  return true;
//}

//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeSender::SendImages()
{
  int limit = 10;
  
  if(this->GetVolumeReconstructionEnabled())
    {
    limit = 1;// Once is enough
    }

  //------------------------------------------------------------
  // loop
  for (int i = 0; i < limit; i ++)
    { 

    //------------------------------------------------------------
    // Get Image data
    igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
    
    vtkGetTestImage(imgMsg);

    //------------------------------------------------------------
    // Get randome orientation matrix and set it.
    igtl::Matrix4x4 matrix;
    vtkGetRandomTestMatrix(matrix);
    imgMsg->SetMatrix(matrix);

    //------------------------------------------------------------
    // Pack (serialize) and send
    imgMsg->Pack();
    
    int ret = this->socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
    if (ret == 0)
      {
      std::cerr << "vtkUltrasoundVolumeSender::SendImages: Error : Connection Lost!\n";
      return false;
      }
    else
      {
      cout << "vtkUltrasoundVolumeSender::SendImages: image frame send successfully" << endl;
      }     
    
      //int sleeptime = (int) (1000.0 / this->FrameRate); 
      //igtl::Sleep(sleeptime); // wait    
      igtl::Sleep(33); // wait    
    }
    
  return true;

}


//------------------------------------------------------------
// Function to get image data
int vtkUltrasoundVolumeSender::vtkGetTestImage(igtl::ImageMessage::Pointer& msg)
{
  int size[3];      // image dimension
  double spacing[3]; // spacing (mm/pixel)
  int svsize[3];    // sub-volume size
  int svoffset[3];  // sub-volume offset
  int scalarType;   // scalar type

  //If we reconstructing a 3DVolume everything is already set for us
  if(this->GetVolumeReconstructionEnabled())
    {
    this->ImageBuffer->GetDimensions(size);
    this->ImageBuffer->GetSpacing(spacing);
    svsize[0]   = size[0];       
    svsize[1]   = size[1];       
    svsize[2]   = size[2];           
    svoffset[0] = svoffset[1] = svoffset[2] = 0;           
    scalarType = this->ImageBuffer->GetScalarType();    
    }
  else
    {   
    //------------------------------------------------------------
    //Imager Setting
    size[0] = VOLUME_X_LENGTH;
    size[1] = VOLUME_Y_LENGTH;
    size[2] = VOLUME_Z_LENGTH;
    spacing[0] = VOLUME_X_SPACING;
    spacing[1] = VOLUME_Y_SPACING;
    spacing[2] = VOLUME_Z_SPACING;
    svsize[0] = VOLUME_X_LENGTH;
    svsize[1] = VOLUME_Y_LENGTH;
    svsize[2] = VOLUME_Z_LENGTH;
    svoffset[0] = svoffset[1] = svoffset[2] = 0;           
    scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
    
    this->FillImage();
    }
    cout << "vtkUltrasoundVolumeSender::SendImages: Size parameters set" << endl;
  
    //------------------------------------------------------------
    // Create a new IMAGE type message    
    msg->SetDimensions(size);
    msg->SetSpacing((float) spacing[0],(float) spacing[1], (float) spacing[2]);
    msg->SetScalarType(scalarType);
    msg->SetDeviceName("ImagerClient");
    msg->SetSubVolume(svsize, svoffset);
    msg->AllocateScalars();
    

  char * p_msg = (char*) msg->GetScalarPointer();
  char * p_ibuffer = (char*) this->ImageBuffer->GetScalarPointer();

  for(int i=0 ; i < size[0] * size[1] * size[2] ; i++ )
    {
    *p_msg = (char) * p_ibuffer;
    p_ibuffer++; p_msg++;
    }

  return 1;
}

//------------------------------------------------------------
// Function to generate random matrix.
void vtkUltrasoundVolumeSender::vtkGetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  /*
  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 0;
  phi = phi + 0.2;

  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  */

  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
//  matrix[0][1] = 0.0;  matrix[1][1] = -1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][1] = 0.0;  matrix[1][1] = 1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
  
  igtl::PrintMatrix(matrix);
}

void vtkUltrasoundVolumeSender::FillImage()
{
      // get the pointer to actual incoming data on to a local pointer
    char* deviceDataPtr = (char *) this->ImageBuffer->GetScalarPointer();  
  
    int R = 50; //Radius of globe
    double r; //Radius of circle in slice
    int c = 50; //Center of globe
    double a;// Start to write at this position in the actual line
    double b;// Stop to write at this position in the actual line
    int n = 30;
  
    for (int z = 0 ; z < SLICE_Z_LENGTH ; z++)
      {
      //Calc radius of slice
      if(n - c >= R || c - n >= R)
        {
        r = -1;
        }
      else
        {
        if(n >= c)
          {
          r = sqrt(R * R - (n - c) * (n - c));
          }
        else
          {
          r = sqrt(R * R - (c - n) * (c - n));
          }
//      cout << "******************************************\n";
//      cout << "Globe Calculation: Slice has radius: " << r << endl;
        }
      for (int y = 0 ; y < SLICE_Y_LENGTH ; y++)
        {
        //Calc a, b
        if(r == -1 || y - SLICE_Y_LENGTH / 2 >= r || SLICE_Y_LENGTH / 2 - y >= r)
          {
          a = -1;
          b = -1;
          }
        else
          {
          if (y <= SLICE_X_LENGTH / 2)
            {
            a = SLICE_X_LENGTH / 2 - sqrt(r * r - (SLICE_Y_LENGTH / 2 - y) * (SLICE_Y_LENGTH / 2 - y));
            b = SLICE_X_LENGTH / 2 + sqrt(r * r - (SLICE_Y_LENGTH / 2 - y) * (SLICE_Y_LENGTH / 2 - y));
            }
          else
            {
            a = SLICE_X_LENGTH / 2 - sqrt(r * r - (y - SLICE_Y_LENGTH / 2) * (y - SLICE_Y_LENGTH / 2));
            b = SLICE_X_LENGTH / 2 + sqrt(r * r - (y - SLICE_Y_LENGTH / 2) * (y - SLICE_Y_LENGTH / 2));          
            }
//            cout << "A, B for this line: " << a << ", " << b << endl; 
          } 
        for (int x = 0 ; x < SLICE_X_LENGTH ; x++)
          {
          if(a == -1 || b == -1 || x <= a || x >= b)
            {
            deviceDataPtr[ z * SLICE_Z_LENGTH + y * SLICE_Y_LENGTH + x] = 255;
            continue;
            }                     
          deviceDataPtr[ z * SLICE_Z_LENGTH + y * SLICE_Y_LENGTH + x] = 0;        
          }            
        }
      }  
}



