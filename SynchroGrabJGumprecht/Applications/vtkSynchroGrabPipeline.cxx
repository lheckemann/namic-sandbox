/*=========================================================================

Module:    $RCSfile: vtkSynchroGrabPipeline.cxx,v $
Author:  Jonathan Boisvert, Queens School Of Computing

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.

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
 
//#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"

#include "vtkObjectFactory.h"
//#include "vtkSonixVideoSource.h"
#include "vtkSynchroGrabPipeline.h"

//#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
//#include "vtkUltrasoundCalibFileReader.h"
#include "vtk3DPanoramicVolumeReconstructor.h"

//#include "vtkTrackerTool.h"
//#include "vtkTrackerBuffer.h"
//#include "vtkNDICertusTracker.h"

//#include "utx_imaging_modes.h"
//#include "ulterius_def.h"

//#include "igtlclientsocket.h"
#include "igtlImageMessage.h"

#include "igtlMath.h"

#include "igtlOSUtil.h"

#define FUDGE_FACTOR 1.6
#define CERTUS_UPDATE_RATE 625

using namespace std;

vtkCxxRevisionMacro(vtkSynchroGrabPipeline, "$Revision$");
vtkStandardNewMacro(vtkSynchroGrabPipeline);

//----------------------------------------------------------------------------
vtkSynchroGrabPipeline::vtkSynchroGrabPipeline()
{
  this->ServerPort = 18944;
  this->NbFrames = 150;
  this->FrameRate = 0.1;

  this->VolumeOutputFile = NULL;
  this->SonixAddr=NULL;
  this->CalibrationFileName = NULL;
  this->OIGTLServer = NULL; 
  this->SetVolumeOutputFile("./outputVol.vtk");
  this->SetSonixAddr("127.0.0.1");
  this->SetOIGTLServer("localhost");


  this->TransfertImages = false; 
  this->VolumeReconstructionEnabled = false;
  this->UseTrackerTransforms = false;

/*
  this->calibReader = vtkUltrasoundCalibFileReader::New();
  this->sonixGrabber = vtkSonixVideoSource::New();
  this->tagger = vtkTaggedImageFilter::New();
  this->tracker = vtkNDICertusTracker::New();
*/
  this->socket = NULL;
  this->socket = igtl::ClientSocket::New();
}

//----------------------------------------------------------------------------
vtkSynchroGrabPipeline::~vtkSynchroGrabPipeline()
{
/*
  this->tracker->Delete();
  this->sonixGrabber->ReleaseSystemResources();
  this->sonixGrabber->Delete();
  this->tagger->Delete();
  this->calibReader->Delete();
*/
////  this->SetVolumeOutputFile(NULL);
////  this->SetSonixAddr(NULL);
  this->SetOIGTLServer(NULL);
////  this->SetCalibrationFileName(NULL);
}

//----------------------------------------------------------------------------
void vtkSynchroGrabPipeline::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  // TODO

}
/*
bool vtkSynchroGrabPipeline::ConfigurePipeline()
{
  this->calibReader->SetFileName(this->CalibrationFileName);
  this->calibReader->ReadCalibFile();

  // set up the video source (ultrasound machine)
  this->sonixGrabber->SetSonixIP(this->SonixAddr);
  this->sonixGrabber->SetFrameRate(this->FrameRate);
  this->sonixGrabber->SetImagingMode(BMode);
  this->sonixGrabber->SetAcquisitionDataType(udtBPost);
  this->sonixGrabber->SetFrameBufferSize(this->NbFrames);

  double *imageOrigin = this->calibReader->GetImageOrigin();
  this->sonixGrabber->SetDataOrigin(imageOrigin);
  double *imageSpacing = this->calibReader->GetImageSpacing();
  this->sonixGrabber->SetDataSpacing(imageSpacing);

  int *imSize = this->calibReader->GetImageSize();
  this->sonixGrabber->SetFrameSize(imSize[0], imSize[1], 1);

  // Setting up the synchronization filter
  this->tagger->SetVideoSource(sonixGrabber);

  // set up the tracker if necessary
  bool error;
  if( this->UseTrackerTransforms)
    error = this->StartTracker();

  this->tagger->Initialize();

  return error;
}

bool vtkSynchroGrabPipeline::StartTracker()
{
  if(this->tracker->Probe() != 1)
    {
    cerr << "Tracking system not found" << endl;
    return false;
    }

  // make sure the tracking buffer is large enough for the number of the image sequence requested
  vtkTrackerTool *tool = this->tracker->GetTool(0);
  tool->GetBuffer()->SetBufferSize( this->NbFrames * CERTUS_UPDATE_RATE / this->FrameRate * FUDGE_FACTOR ); 
  this->tracker->StartTracking();

  this->tagger->SetTrackerTool(tool);
  this->tagger->SetCalibrationMatrix(this->calibReader->GetCalibrationMatrix());
}
*/
bool vtkSynchroGrabPipeline::ReconstructVolume()
{
  int volumeExtent[6] = {0, 419, 0, 319, 0, 2};
  int origin[3] = {0, 0, 0};
  double spacing [3] = {1, 1, 1}; 
  
//  this->sonixGrabber->Record();  //start recording frame from the video

  // wait for the images (delay in milliseconds)
//  Sleep(this->NbFrames / this->FrameRate * 1000);
 
//  this->sonixGrabber->Stop();

//  this->tracker->StopTracking();

//  cout << "Recorded synchronized transforms and ultrasound images for " << this->NbFrames / this->FrameRate * 1000 << "ms" << endl;

  // set up the panoramic reconstruction class
  vtk3DPanoramicVolumeReconstructor *panoramaReconstructor = vtk3DPanoramicVolumeReconstructor::New();
  panoramaReconstructor->CompoundingOn();
  panoramaReconstructor->SetInterpolationModeToLinear();
//  panoramaReconstructor->GetOutput()->SetScalarTypeToUnsignedChar();

  // Determine the extent of the volume that needs to be reconstructed by 
  // iterating throught all the acquired frames
//TODO: create clip rectangle data
  double clipRectangle[4];
  clipRectangle[0] = volumeExtent[0];
  clipRectangle[1] = volumeExtent[1];
  clipRectangle[2] = volumeExtent[2];
  clipRectangle[3] = volumeExtent[3];
  
//  this->calibReader->GetClipRectangle(clipRectangle);
  panoramaReconstructor->SetClipRectangle(clipRectangle);
  double xmin = clipRectangle[0], ymin = clipRectangle[1], 
         xmax = clipRectangle[2], ymax = clipRectangle[3];

  double imCorners[4][4]= { 
      { xmin, ymin, 0, 1}, 
      { xmin, ymax, 0, 1},
      { xmax, ymin, 0, 1},
      { xmax, ymax, 0, 1} };

  double transformedPt[4];

  double maxX, maxY, maxZ;
  maxX = maxY = maxZ = -1 * numeric_limits<double>::max();
  double minX, minY, minZ;
  minX = minY = minZ = numeric_limits<double>::max();

//  int nbFramesGrabbed = sonixGrabber->GetFrameCount();
  int nbFramesGrabbed = 10; //Randomly selected

//  this->sonixGrabber->Rewind();
//  for(int i=0; i < nbFramesGrabbed; i++)
//    {
    // get those transforms... and computer the bounding box
//    this->tagger->Update();

    // determine the bounding box occupied by the reconstructed volume
//    for(int j=0; j < 4; j++)
//      {
//      this->tagger->GetTransform()->MultiplyPoint(imCorners[j],transformedPt);
//      minX = min(transformedPt[0], minX);
//      minY = min(transformedPt[1], minY);
//      minZ = min(transformedPt[2], minZ);
//      maxX = max(transformedPt[0], maxX);
//      maxY = max(transformedPt[1], maxY);
//      maxZ = max(transformedPt[2], maxZ);
//      }
//    this->sonixGrabber->Seek(1);
//    }

//  double spacing[3] = {0.5,0.5,0.5};
//  int volumeExtent[6] = { 0, (int)( (maxX - minX) / spacing[0] ), 
//                          0, (int)( (maxY - minY) / spacing[1] ), 
//                          0, (int)( (maxZ - minZ) / spacing[2] ) };

  panoramaReconstructor->SetOutputExtent(volumeExtent);
  panoramaReconstructor->SetOutputSpacing(spacing);
//  panoramaReconstructor->SetOutputOrigin(minX, minY, minZ);
  panoramaReconstructor->SetOutputOrigin(origin[0], origin[1], origin[2]);

  // rewind and add the slices the panoramaReconstructor
  vtkImageData* outputImage = vtkImageData::New();
//  panoramaReconstructor->SetSlice(tagger->GetOutput());
  panoramaReconstructor->SetSlice(outputImage);
  panoramaReconstructor->GetOutput()->Update();
//  this->sonixGrabber->Rewind();
//  this->tagger->Update();

  vtkMatrix4x4 *sliceAxes = vtkMatrix4x4::New();
  sliceAxes->Zero();
  sliceAxes->Element[0][0] = 1; 
  sliceAxes->Element[1][1] = -1; 
  sliceAxes->Element[2][2] = 1; 
  sliceAxes->Element[3][3] = 1; 
////  this->tagger->GetTransform()->GetMatrix(sliceAxes);
  panoramaReconstructor->SetSliceAxes(sliceAxes);

  for(int i=0; i < nbFramesGrabbed; i++)
    {
////    this->tagger->Update();
////    this->tagger->GetTransform()->GetMatrix(sliceAxes);
    panoramaReconstructor->SetSliceAxes(sliceAxes);
    panoramaReconstructor->InsertSlice();
//    this->sonixGrabber->Seek(1);
    }

  cout << "Inserted " << panoramaReconstructor->GetPixelCount() << " pixels into the output volume" << endl;

  panoramaReconstructor->FillHolesInOutput();

  // To remove the alpha channel of the reconstructed volume
  vtkImageExtractComponents *extract = vtkImageExtractComponents::New();
  extract->SetInput(panoramaReconstructor->GetOutput());
  extract->SetComponents(0);

  // write it to the specified file.
  vtkDataSetWriter *writer3D = vtkDataSetWriter::New();
  writer3D->SetFileTypeToBinary();
  writer3D->SetInput(extract->GetOutput());
  writer3D->SetFileName(this->VolumeOutputFile);
  writer3D->Write();

  writer3D->Delete();
  extract->Delete();
  panoramaReconstructor->Delete();
  return true;
}


bool vtkSynchroGrabPipeline::ConnectToServer()
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

bool vtkSynchroGrabPipeline::CloseServerConnection()
{
  this->socket->CloseSocket();
  return true;
}

/*bool vtkSynchroGrabPipeline::SendImages()
{
  // Start the video source and configure an image frame
  this->sonixGrabber->Record();
  this->sonixGrabber->Update();

  igtl::ImageMessage::Pointer imMessage = igtl::ImageMessage::New();
  imMessage->SetDeviceName("SonixRP");
  vtkImageData *image = sonixGrabber->GetOutput();
  int *dim = image->GetDimensions();
  double *spacing = image->GetSpacing();

  cout << "Dimensions " << dim[0] << " " << dim[1] << " " << endl;
  cout << "Spacing " << spacing[0] << " " << spacing[1] << " " << spacing[2] << endl;
  cout << "Data dimension : " << image->GetDataDimension() << endl;
  cout << "Number of scalar components : " << image->GetNumberOfScalarComponents() << endl;
  cout << "Scalar Type : " << image->GetScalarType() << " ( " << image->GetScalarTypeAsString() << ")" << endl;

  imMessage->SetDimensions(dim[0], dim[1], 1);
  imMessage->SetSpacing(spacing[0], spacing[1], spacing[2]);
  imMessage->SetScalarType(image->GetScalarType());
  imMessage->AllocateScalars();

  int svd[3];
  int svoff[3];
  imMessage->GetSubVolume(svd, svoff);

  cout << "SubVolume dimensions : " << svd[0] << " " << svd[1] << " " << svd[2] << endl;
  cout << "SubVolume offsets : " << svoff[0] << " " << svoff[1] << " " << svoff[2] << endl;

  // Setup a transform matrix for the OpenIGTLink message
  igtl::Matrix4x4 igtlMatrix = {{1.0, 0.0, 0.0, 0.0},{0.0, 1.0, 0.0, 0.0},{0.0, 0.0, 1.0, 0.0},{0.0, 0.0, 0.0, 1.0}};
  vtkMatrix4x4 *vtkMat = vtkMatrix4x4::New();
  imMessage->SetMatrix(igtlMatrix);

  for( int i=0; i < this->NbFrames; i++)
    {
    Sleep(1/this->FrameRate*1000);
    this->tagger->Update();

    if(this->UseTrackerTransforms)
      {
      this->tagger->GetTransform()->GetMatrix(vtkMat);
      for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
          igtlMatrix[i][j]=(*vtkMat)[i][j];
      imMessage->SetMatrix(igtlMatrix);
      }

    unsigned char* imgPtr = (unsigned char*)this->tagger->GetOutput()->GetScalarPointer();
    int imSize = imMessage->GetImageSize();

    // copy the image raw data 
    memcpy(imMessage->GetScalarPointer(), imgPtr, imSize);
    imMessage->Pack();

    // sending this frame to the server
    std::cout << "PackSize:  " << imMessage->GetPackSize() << std::endl;
    std::cout << "BodyMode:  " << imMessage->GetBodyType() << std::endl;

    int ret = socket->Send(imMessage->GetPackPointer(), imMessage->GetPackSize());
    if (ret == 0)
      {
      std::cerr << "Error : Connection Lost!\n";
      return false;
      }
    else
      cout << "image frame send successfully" << endl;
    }
  return true;
}
*/

bool vtkSynchroGrabPipeline::SendImages()
{
//  cout << "vtkSynchroGrabPipeline::SendImages: Start" << endl;
  
  char*  filedir  = "/projects/mrrobot/gumprecht/slicer/tmp/OpenIGTLink/Examples/Imager/img";

  //------------------------------------------------------------
  // loop
//  cout << "vtkSynchroGrabPipeline::SendImages: Start SendImages loop " << endl;
  for (int i = 0; i < 100; i ++)
    {
//    cout << "vtkSynchroGrabPipeline::SendImages: SendImages loop i: "<< i << endl;  

    //------------------------------------------------------------
    // size parameters    
     int   size[]     = {256, 256, 256};       // image dimension
    float spacing[]  = {1, 1, 1};     // spacing (mm/pixel)
    int   svsize[]   = {256, 256, 256};       // sub-volume size
    int   svoffset[] = {0, 0, 0};           // sub-volume offset
    int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
//    int   size[]     = {256, 256, 1};       // image dimension
//    float spacing[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
//    int   svsize[]   = {256, 256, 1};       // sub-volume size
//    int   svoffset[] = {0, 0, 0};           // sub-volume offset
//    int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
//    cout << "vtkSynchroGrabPipeline::SendImages: Size parameters set" << endl;  

    //------------------------------------------------------------
    // Create a new IMAGE type message
    igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
    imgMsg->SetDimensions(size);
    imgMsg->SetSpacing(spacing);
    imgMsg->SetScalarType(scalarType);
    imgMsg->SetDeviceName("ImagerClient");
    imgMsg->SetSubVolume(svsize, svoffset);
    imgMsg->AllocateScalars();
//    cout << "vtkSynchroGrabPipeline::SendImages: New image type message created" << endl;  

    //------------------------------------------------------------
    // Set image data (See GetTestImage() bellow for the details)
    vtkGetTestImage(imgMsg, filedir, i % 5);
//    cout << "vtkSynchroGrabPipeline::SendImages: Image data set" << endl;  

    //------------------------------------------------------------
    // Get randome orientation matrix and set it.
    igtl::Matrix4x4 matrix;
    vtkGetRandomTestMatrix(matrix);
    imgMsg->SetMatrix(matrix);
//    cout << "vtkSynchroGrabPipeline::SendImages: Receives ramdom orientation matrix" << endl;

    //------------------------------------------------------------
    // Pack (serialize) and send
    imgMsg->Pack();
    
    int ret = this->socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
    if (ret == 0)
      {
      std::cerr << "vtkSynchroGrabPipeline::SendImages: Error : Connection Lost!\n";
      return false;
      }
    else
      {
      cout << "vtkSynchroGrabPipeline::SendImages: image frame send successfully" << endl;
      }
     
//    cout << "vtkSynchroGrabPipeline::SendImages: Message sent" << endl;
    
    int sleeptime = (int) (1000.0 / this->FrameRate); 

    igtl::Sleep(sleeptime); // wait    
//    cout << "vtkSynchroGrabPipeline::SendImages: After Sleep" << endl;

    }
    
    cout << "vtkSynchroGrabPipeline::SendImages: LoopDone" << endl;
    
    // Close connection
    socket->CloseSocket();
    return true;


  //------------------------------------------------------------
  // Establish Connection
  /*igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer("localhost", 18944);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // loop
  for (int i = 0; i < 100; i ++)
    {

    //------------------------------------------------------------
    // size parameters
    int   size[]     = {256, 256, 256};       // image dimension
    float spacing[]  = {1, 1, 1};     // spacing (mm/pixel)
    int   svsize[]   = {256, 256, 256};       // sub-volume size
    int   svoffset[] = {0, 0, 0};           // sub-volume offset
    int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
/*    int   size[]     = {256, 256, 124};       // image dimension
    float spacing[]  = {-1.5, 0.9375, -0.9375};     // spacing (mm/pixel)
    int   svsize[]   = {256, 256, 1};       // sub-volume size
    int   svoffset[] = {0, 0, 0};           // sub-volume offset
    int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type*/
/*
    //------------------------------------------------------------
    // Create a new IMAGE type message
    igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
    imgMsg->SetDimensions(size);
    imgMsg->SetSpacing(spacing);
    imgMsg->SetScalarType(scalarType);
    imgMsg->SetDeviceName("ImagerClient");
    imgMsg->SetSubVolume(svsize, svoffset);
    imgMsg->AllocateScalars();

    //------------------------------------------------------------
    // Set image data (See GetTestImage() bellow for the details)
    vtkGetTestImage(imgMsg, filedir, i % 5);

    //------------------------------------------------------------
    // Get randome orientation matrix and set it.
    igtl::Matrix4x4 matrix;
    vtkGetRandomTestMatrix(matrix);
    imgMsg->SetMatrix(matrix);

    //------------------------------------------------------------
    // Pack (serialize) and send
    imgMsg->Pack();
    socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());

    int  interval = (int) (10000.0);

    igtl::Sleep(interval); // wait

    }

  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();
*/
}


//------------------------------------------------------------
// Function to read test image data
int vtkSynchroGrabPipeline::vtkGetTestImage(igtl::ImageMessage::Pointer& msg, const char* dir, int i)
{

  //------------------------------------------------------------
  // Check if image index is in the range
  if (i < 0 && i >= 5) 
    {
    std::cerr << "Image index is invalid." << std::endl;
    return 0;
    }

  //------------------------------------------------------------
  // Generate path to the raw image file
  char filename[128];
  sprintf(filename, "%s/igtlTestImage%d.raw", dir, i+1);
//  std::cerr << "Reading " << filename << "...";
  std::cerr << "Reading spgr.nhdr  ...";

  //------------------------------------------------------------
  // Load raw data from the file
//  FILE *fp = fopen(filename, "rb");
  FILE *fp = fopen("/projects/mrrobot/gumprecht/tutorials/na-mic/CUDA/heart/Heart256.raw", "rb");
//  FILE *fp = fopen("/projects/mrrobot/gumprecht/tutorials/na-mic/data_loading_and_visualization/SlicerSampleVisualization/nrrd/spgr.nhdr", "rb");

  if (fp == NULL)
    {
    std::cerr << "File opeining error: " << filename << std::endl;
    return 0;
    }
  int fsize = msg->GetImageSize();
  size_t b = fread(msg->GetScalarPointer(), 1, fsize, fp);

  fclose(fp);

  std::cerr << "done." << std::endl;

  return 1;
}

//------------------------------------------------------------
// Function to generate random matrix.
void vtkSynchroGrabPipeline::vtkGetRandomTestMatrix(igtl::Matrix4x4& matrix)
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
  matrix[0][1] = 0.0;  matrix[1][1] = -1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
  
  igtl::PrintMatrix(matrix);
}
