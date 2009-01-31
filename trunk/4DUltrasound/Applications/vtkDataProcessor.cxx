/*========================================================================

Module:  $RCSfile: vtkDataProcessor.cxx,v $
Author:  Jonathan Boisvert, Queens School Of Computing
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School
Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada All rights reserved.
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

 * Neither the name of Harvard Medical School nor the names of any
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
//#define REMOVE_ALPHA_CHANNEL
#define DEBUG_IMAGES //Write tagger output to HDD
//#define DEBUG_MATRICES //Prints tagger matrices to stdout
 
//#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkBMPWriter.h"
#include "vtkMatrix4x4.h"


#include "vtkObjectFactory.h"
#include "vtkDataProcessor.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkUltrasoundCalibFileReader.h"
#include "vtk3DPanoramicVolumeReconstructor.h"

#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

#ifdef USE_TRACKER_DEVICE
#include "vtkNDITracker.h"
#else
#include "vtkTrackerSimulator.h"
#endif

#ifdef USE_ULTRASOUND_DEVICE
#include "vtkV4L2VideoSource.h"
#else
#include "vtkVideoSourceSimulator.h"
#endif

#define FUDGE_FACTOR 1.6
#define CERTUS_UPDATE_RATE 625
#define US_IMAGE_FAN_RATIO 0.6 //Amount of ultrasound images's height that
                               //is used by the ultrasound fan

using namespace std;

static inline void vtkSleep(double duration);

vtkCxxRevisionMacro(vtkDataProcessor, "$Revision$");
vtkStandardNewMacro(vtkDataProcessor);

//----------------------------------------------------------------------------
vtkDataProcessor::vtkDataProcessor()
{
  this->Verbose = false;
  this->VolumeReconstructionEnabled = false;
  this->Processing = false;
  
  this->newDataBufferSize = 100;
  this->newDataBufferIndex = -1;
  
  this->clipRectangle = {0, 0, 0, 0}
  
  this->PlayerThreader = vtkMultiThreader::New();;
  this->PlayerThreadId = -1;  
  
  this->CalibrationFileName = NULL;
  this->calibReader = vtkUltrasoundCalibFileReader::New();
}

//----------------------------------------------------------------------------
vtkDataProcessor::~vtkDataProcessor()
{
  if(VolumeReconstructionEnabled)
    {
    this->reconstructor->Delete();
    }
    
  this->PlayerThreader->Delete();
  
  this->calibReader->Delete();
  this->SetCalibrationFileName(NULL);
}
//----------------------------------------------------------------------------
void vtkDataProcessor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

/******************************************************************************
 *  static inline void vtkSleep(double duration) 
 *
 *  Platform-independent sleep function
 *  Set the current thread to sleep for a certain amount of time
 * 
 *  @Param: double duration - Time to sleep in ms 
 * 
 * ****************************************************************************/
static inline void vtkSleep(double duration)
{
  duration = duration; // avoid warnings
  // sleep according to OS preference
#ifdef _WIN32
  Sleep((int)(1000*duration));
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi) || defined(__APPLE__)
  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = (int)duration;
  sleep_time.tv_nsec = (int)(1000000000*(duration-sleep_time.tv_sec));
  nanosleep(&sleep_time,&dummy);
#endif
}


/******************************************************************************
 *  static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time) 
 *
 *  Sleep until the specified absolute time has arrived.
 *  You must pass a handle to the current thread.  
 *  If '0' is returned, then the thread was aborted before or during the wait. * 
 *  
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2008
 * 
 *  @Param: vtkMultiThreader::ThreadInfo *data *  
 *  @Param: double time - Time until which to sleep
 *  @Param: bool checkActiveFlagImmediately - avoid waiting if enabled
 * 
 * ****************************************************************************/
static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time, bool checkActiveFlagImmediately)
{
  if(checkActiveFlagImmediately)
    {
    // check to see if we are being told to quit 
      data->ActiveFlagLock->Lock();
      int activeFlag = *(data->ActiveFlag);
      data->ActiveFlagLock->Unlock();

      if (activeFlag == 0)
        {//We are told to quit
        return 0;
        }
      
      //Go on processing
      return 1;
    }

  // loop either until the time has arrived or until the thread is ended
  for (int i = 0;; i++)
    {
      double remaining = time - vtkTimerLog::GetUniversalTime();

      // check to see if we have reached the specified time
      if (remaining <= 0)
        {
        return 1;
      }
      // check the ActiveFlag at least every 0.1 seconds
      if (remaining > 0.1)
        {
        remaining = 0.1;
        }

      // check to see if we are being told to quit 
      data->ActiveFlagLock->Lock();
      int activeFlag = *(data->ActiveFlag);
      data->ActiveFlagLock->Unlock();

      if (activeFlag == 0)
        {
        break;
        }

      vtkSleep(remaining);
    }

  return 0;
}

/******************************************************************************
 *  static void *vtkDataProcessorThread(vtkMultiThreader::ThreadInfo *data) 
 *
 *  This function runs in an alternate thread to asyncronously collect data 
 *  
 *  @Author:Jan Gumprecht
 *  @Date:  27.Januar 2009
 * 
 *  @Param: vtkMultiThreader::ThreadInfo *data
 * 
 * ****************************************************************************/
static void *vtkDataProcessorThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkDataCollector *self = (vtkDataCollector *)(data->UserData);
  
  double startTime = vtkTimerLog::GetUniversalTime();
  double processPeriod = self->GetProcessPeriod();
  int lastDataSenderIndex = -2;
  int currentIndex = -1;
  bool dataAvailable = false;
  
  do
    {
    //JG 09/01/21
    //cerr << "New frame to process" <<endl;

    if(dataAvailable || self->NewDataAvailable())
      {//New data to process available
      currentIndex = self->GetHeadOfNewDataBuffer();

      self->CheckandUpdateVolume(currentIndex, lastDataSenderIndex);//Check if volume must be expanded      
      
      self->ReconstructVolume(currentIndex);
       
      lastDataSenderIndex = self->ForwardVolume();//Forward Volume to data sender
      }
      
      dataAvailable = self->NewDataAvailable();
    
    } 
  while(vtkThreadSleep(data, vtkTimerLog::GetUniversalTime() + processPeriod), dataAvailable);
  
  return NULL;
}

/******************************************************************************
 *  int vtkDataProcessor::StartProcessing() 
 *
 *  Starts process thread 
 *  
 *  @Author:Jan Gumprecht
 *  @Date:  27.Januar 2009
 * 
 * ****************************************************************************/
int vtkDataProcessor::StartProcessing()
{
  if(!this->VolumeReconstructionEnabled)
    {//Nothing to start
        return 0;
        }
        
  if(this->Processing)
    {
    cerr << "ERROR: Data Processor already processes data" << endl;
    return -1;
    }
  
  //Start process thread
  this->PlayerThreadId =  
            this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
            &vtkDataProcessorThread, this);  

  //Check if thread successfully started
  if(this->PlayerThreadId != -1)
    {
    this->Processing = true;
    if(Verbose)
      {
      cout << "Successfully started to process data" << endl;
      }
    return 0;
    }
  else
    {
    cerr << "ERROR: Could not start process thread" << endl;
    return -1;
    } 
}

/******************************************************************************
 *  int vtkDataProcessor::StopProcessing() 
 *
 *  Stops the process thread if running 
 *  
 *  @Author:Jan Gumprecht
 *  @Date:  27.Januar 2009
 * 
 * ****************************************************************************/
int vtkDataProcessor::StopProcessing()
{
  if(this->Processing)
    {//Stop thread
        this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    this->Processing = false;
    
    if(Verbose)
      {
      cout << "Stopped processing data" <<endl;
      }
        }
  else
        {
        return -1;
        }
          
  return 0;     
}

/******************************************************************************
 *  int vtkDataProcessor::EnableVolumeReconstruction(bool flag) 
 *
 *  Turn volume reconstruction ON or OFF
 * 
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 * 
 *  @Param: bool flag - to set the state of volume reconstruction
 * 
 * ****************************************************************************/
int vtkDataProcessor::EnableVolumeReconstruction(bool flag)
{ 
  if(flag && !this->VolumeReconstructionEnabled)
    {//Reconstruction was not enabled and shall be enabled now 

    // set up the panoramic reconstruction class
    this->reconstructor = vtk3DPanoramicVolumeReconstructor::New();
    this->reconstructor->CompoundingOn();
    this->reconstructor->SetInterpolationModeToLinear();
    
    this->calibReader->GetClipRectangle(this->clipRectangle);
    reconstructor->SetClipRectangle(this->clipRectangle);
    
    }
    
  if(!flag && this->VolumeReconstructionEnabled)
    {//Turn of volume reconstruction    
    this->reconstructor->Delete();
    }

  this->VolumeReconstructionEnabled = flag;

  return 0;
}

/******************************************************************************
 *  int vtkDataProcessor::CheckandUpdateVolume(int index, int dataSenderIndex) 
 *
 *  
 * 
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 * 
 * 
 * ****************************************************************************/
int vtkDataProcessor::CheckandUpdateVolume(int index, int dataSenderIndex)
{

  if(!this->VolumeReconstructionEnabled)
    {
    cerr << "ERROR: Try check and update a volume without reconstruction enabled" <<endl;     
    return -1;
    }
    
  //Calculate needed volume extent
  double xmin = this->clipRectangle[0], ymin = this->clipRectangle[1], 
         xmax = this->clipRectangle[2], ymax = this->clipRectangle[3];
     
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

    // determine the bounding box occupied by the reconstructed volume
    for(int j=0; j < 4; j++)
      {

      this->newTrackerMatrixMap[index]->MultiplyPoint(imCorners[j],transformedPt);

      minX = min(transformedPt[0], minX);
      minY = min(transformedPt[1], minY);
      minZ = min(transformedPt[2], minZ);
      maxX = max(transformedPt[0], maxX);
      maxY = max(transformedPt[1], maxY);
      maxZ = max(transformedPt[2], maxZ);
      }
    }

  double spacing[3] = {1,1,1};
  int volumeExtent[6] = { 0, (int)( (maxX - minX) / spacing[0] ), 
                          0, (int)( (maxY - minY) / spacing[1] ), 
                          0, (int)( (maxZ - minZ) / spacing[2] )};
  
  //Check if new extent larger than old extent or origin changed
  vtkFloatingPointType oldOrigin[3];
  bool originChanged;
  int oldExtent[6];
  bool extentChanged;                        
     
  if(dataSenderIndex != -2)
    {//This is not the first volume we reconstruct
    
    //Check if origin changed    
    reconstructor->GetOrigin(oldOrigin);
    originChanged = oldOrigin[0] != minX || oldOrigin[1] != minY || oldOrigin[2] != minZ;
    
    //Check if extent changed    
    reconstructor->GetExtent(oldExtent);
    for(int i = 0; i < 6; ++i)
      {
      if(oldExtent[i] != volumeExtent[i])
        {
        extentChanged = true;
        brake;
        }
      }    
    }
    
   if(dataSender == 2 || originChanged || extentChanged)
     {//Create new volume
     reconstructor->SetOutputExtent(volumeExtent);
     reconstructor->SetOutputSpacing(spacing);
     reconstructor->SetOutputOrigin(minX, minY, minZ);
     
     reconstructor->SetSlice(this->newFrameMap[index]);
     reconstructor->GetOutput()->Update();

     reconstructor->SetSliceAxes(this->newTrackerMatrixMap[index]);

     //Set the correct amount of the output volume's scalar components
     reconstructor->GetOutput()->SetNumberOfScalarComponents(this->newFrameMap[index]); 
     reconstructor->GetOutput()->AllocateScalars();
     
     //check if not first volume
     if(dataSenderIndex != -2)
       {//Copy Old Volume at correct position and volume into new volume
       fuege altes Volumen in neues ein;
       }        
     }
}

/******************************************************************************
 *  int vtkDataProcessor::ReconstructVolume(int index) 
 *
 *  
 *  
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 * 
 *  @Param: bool flag -
 * 
 * ****************************************************************************/
int vtkDataProcessor::ReconstructVolume(int index)
{
  if(!this->VolumeReconstructionEnabled)
    {
    cerr << "ERROR: Try to reconstruct a volume without reconstruction enabled" <<endl;     
    return -1;
    }
    
    reconstructor->SetSliceAxes(this->newTrackerMatrixMap[index]); //Set current trackingmatrix
    reconstructor->SetSlice(this->newFrameMap[index]);
    
    reconstructor->InsertSlice(); //Add current slice to the reconstructor

  //----- Reconstruction done -----

  if(reconstructor->GetPixelCount() > 0)
    {      
      if(Verbose)
        {
        cout << "Reconstruction successfully completed" << endl;
        cout << "  Inserted " << reconstructor->GetPixelCount() << " pixels into the output volume" << endl;
        }      
    }
  else
    {
      cerr << "Reconstruction failed" << endl;
      return -1;
    }

  reconstructor->FillHolesInOutput(); 
  
  return 0;
}

/******************************************************************************
 *  int vtkDataProcessor::ForwardData() 
 *
 *  
 *  
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 * 
 *  @Param: bool flag -
 * 
 * ****************************************************************************/
int vtkDataProcessor::ForwardData()
{
  //---------------------------------------------------------------------------
  // Prepare reconstructed 3D volume for transfer

#ifdef REMOVE_ALPHA_CHANNEL
  // To remove the alpha channel of the reconstructed volume
  vtkImageExtractComponents *extract = vtkImageExtractComponents::New();
  extract->SetInput(reconstructor->GetOutput());
  extract->SetComponents(0);
    
  vtkImageData * volume = extract->GetOutput();
  
  extract->Delete();
  
#else
  vtkImageData * volume = reconstructor->GetOutput();
  
#endif  

        //--- Adjust Properties of Volume ---
  
  //Dimensions
  int dimensions[3];   
  extractOutput->GetDimensions(dimensions);
  Volume->SetDimensions(dimensions);

  //Spacing
  double ouputSpacing[3];
  extractOutput->GetSpacing(ouputSpacing);
  Volume->SetSpacing(ouputSpacing);
  
  //SetScalarType
  Volume->SetScalarType(extractOutput->GetScalarType());
  Volume->AllocateScalars();

  unsigned char * pBuff = (unsigned char *) Volume->GetScalarPointer();
  unsigned char * pExtract = (unsigned char *) extractOutput->GetScalarPointer();  

  //Fill transfer buffer
  for(int i = 0 ; i < dimensions[2] ; ++i)
    {
    for(int j = 0 ; j < dimensions[1] ; ++j)
      {
      for(int k = 0 ; k < dimensions[0] ; ++k)
        {
        *pBuff = *pExtract;
        pBuff++;
        pExtract++;
        }
      }
    }   
}

/******************************************************************************
 *  int vtkDataProcessor::AddFrameToFrameMap(int index, vtkImageData* frame) 
 *
 *  Adds a new frame to the index(key) - image(value) map 
 *  
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 * 
 *  @Param: int index - Add new frame to map with this key
 *  @Param: vtkImageData* frame - Add this frame to map
 * 
 * ****************************************************************************/
int vtkDataProcessor::AddFrameToFrameMap(int index, vtkImageData* frame)
{
  if(this->newImageMap[index] != NULL)
    {
    cerr << "ERROR: new image map already has an image for index: " << index << endl;
    return -1;
    }
    
  this->newImageMap[index] = frame;
  return 0;
}

/******************************************************************************
 *  int vtkDataProcessor::AddMatrixToMatrixMap(int index, vtkMatrix4x4* matrix) 
 *
 *  Adds a new frame to the index(key) - matrix(value) map 
 *  
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 * 
 *  @Param: int index - Add new frame to map with this key
 *  @Param: vtkMatrix4x4* matrix - Add this matrix to map
 * 
 * ****************************************************************************/
int vtkDataProcessor::AddMatrixToMatrixMap(int index, vtkMatrix4x4* matrix)
{
  if(this->newTrackerMatrixMap[index] != NULL)
    {
    cerr << "ERROR: new tracker matrix map already has a matrix for index: " << index << endl;
    return -1;
    }
    
  this->newTrackerMatrixMap[index] = matrix;
  return 0;
}
