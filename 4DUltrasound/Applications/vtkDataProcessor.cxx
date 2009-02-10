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
#include <fstream>
#include <sstream>
#include <limits>
#include <string>
#include <assert.h>

#define NOMINMAX
//#define REMOVE_ALPHA_CHANNEL
#define DEBUG_IMAGES //Write tagger output to HDD
//#define DEBUG_MATRICES //Prints tagger matrices to stdout
#define NEWFORWARDING
#define MERGE
#define NEWMERGE

//#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkBMPWriter.h"
#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkTimerLog.h"
#include "vtkMutexLock.h"

#include "vtkObjectFactory.h"
#include "vtkDataProcessor.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkUltrasoundCalibFileReader.h"
#include "vtk3DPanoramicVolumeReconstructor.h"

#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

#include "vtkDataSender.h"

#define US_IMAGE_FAN_RATIO 0.6 //Amount of ultrasound images's height that
                               //is used by the ultrasound fan

using namespace std;

static inline void vtkSleep(double duration);

vtkCxxRevisionMacro(vtkDataProcessor, "$Revision$");
vtkStandardNewMacro(vtkDataProcessor);

/******************************************************************************
 * vtkDataProcessor()
 *
 *  Constructor
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 * ****************************************************************************/
vtkDataProcessor::vtkDataProcessor()
{
  this->Verbose = false;
  this->VolumeReconstructionEnabled = false;
  this->Processing = false;
  this->ProcessPeriod = 1 /(30 * 1.5);

  this->newDataBufferSize = 100;
  this->newDataBufferIndex = -1;

  this->clipRectangle[0] = 0;
  this->clipRectangle[1] = 0;
  this->clipRectangle[2] = 0;
  this->clipRectangle[3] = 0;

  this->PlayerThreader = vtkMultiThreader::New();
  this->PlayerThreadId = -1;

  this->CalibrationFileName = NULL;
  this->calibReader = vtkUltrasoundCalibFileReader::New();

  this->DataSender = NULL;

  this->StartUpTime = vtkTimerLog::GetUniversalTime();
  this->LogStream.ostream::rdbuf(cerr.rdbuf());
  this->oldVolume = NULL;
}

/******************************************************************************
 * ~vtkDataProcessor()
 *
 *  Destructor
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 * ****************************************************************************/
vtkDataProcessor::~vtkDataProcessor()
{
  this->StopProcessing();
  if(VolumeReconstructionEnabled)
    {
    this->reconstructor->Delete();

    //Delete all buffer objects
    while(!this->IsNewDataBufferEmpty())
      {
      this->DeleteData(this->GetHeadOfNewDataBuffer());
      }

    }

  this->PlayerThreader->Delete();

  this->calibReader->Delete();
  this->SetCalibrationFileName(NULL);
}
/******************************************************************************
 *  PrintSelf(ostream& os, vtkIndent indent)
 *
 *  Print information about the instance
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 * ****************************************************************************/
void vtkDataProcessor::PrintSelf(ostream& os, vtkIndent indent)
{
  //Not implemented yet
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
 *  @Date:  27.January 2009
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
  vtkDataProcessor *self = (vtkDataProcessor *)(data->UserData);
  float processPeriod = self->GetProcessPeriod();
  int lastDataSenderIndex = -2;
  int currentIndex = -1;
  bool dataAvailable = false;
  int errors = 0;
  string input;
  double sectionTime;
  double loopTime;

  #ifdef  DEBUGPROCESSOR
    self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Processor Thread started" << endl;
  #endif

  do
    {
    #ifdef DEBUGPROCESSOR
    //self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Processor Thread looks for new data" << endl;
    #endif

    if(dataAvailable || !self->IsNewDataBufferEmpty())
      {//New data to process available
      loopTime = self->GetUpTime();
      currentIndex = self->GetHeadOfNewDataBuffer();

//      if(currentIndex == 2)
//        {
//        assert(false);
//        }

      #ifdef  TIMINGPROCESSOR
        self->GetLogStream() <<  self->GetUpTime() << " |-------------------------------------" << endl;
        self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Processor Thread found new data with index:" << currentIndex
                             << " | L:" << self->GetUpTime() - loopTime << endl;
      #endif

      sectionTime = self->GetUpTime();
      if(self->CheckandUpdateVolume(currentIndex, lastDataSenderIndex) != -1)//Check if volume must be expanded
        {
        #ifdef  TIMINGPROCESSOR
          self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Check and Update DONE" << " | L:" << self->GetUpTime() - loopTime << "| S: " << self->GetUpTime() - sectionTime << endl;
        #endif

        sectionTime = self->GetUpTime();
        if(self->ReconstructVolume(currentIndex) != -1)
          {
          #ifdef  TIMINGPROCESSOR
            self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Volume Reconstruction DONE" << " | L:" << self->GetUpTime() - loopTime << "| S: " << self->GetUpTime() - sectionTime << endl;
          #endif
          sectionTime = self->GetUpTime();
          lastDataSenderIndex = self->ForwardData();
          #ifdef  TIMINGPROCESSOR
            self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Volume forwarding DONE"
                                                       << " | S: " << self->GetUpTime() - sectionTime
                                                       << " | L:" << self->GetUpTime() - loopTime
                                                       << " | FPS: " << 1 / (self->GetUpTime() - loopTime) << endl;

          #endif
          }
        else
          {
          #ifdef  DEBUGPROCESSOR
            self->GetLogStream() <<  self->GetUpTime() << " |P-WARNING: Volume Reconstruction failed" << " | L:" << self->GetUpTime() - loopTime << "| S: " << self->GetUpTime() - sectionTime << endl;
            self->ResetOldVolume(lastDataSenderIndex);
          #endif
          }
        }
      else
        {
        lastDataSenderIndex = -2;
        }
      self->DeleteData(currentIndex);
      }

      dataAvailable = !self->IsNewDataBufferEmpty();

      #ifdef  DEBUGPROCESSOR
        if(dataAvailable)
          {
          self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Data Processor wont't sleep" << " | " << self->GetUpTime()<< endl;
          }
        else
          {
          self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Data Processor sleeps now" << " | " << self->GetUpTime()<< endl;
          }
      #endif

    }
  while(vtkThreadSleep(data, vtkTimerLog::GetUniversalTime() + processPeriod, dataAvailable));

  return NULL;
}

/******************************************************************************
 *  int vtkDataProcessor::StartProcessing(vtkDataSender * sender)
 *
 *  Starts process thread
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.Januar 2009
 *
 *  @Param: vtkDataSender * sender: DataSender to which proccessed data is
 *                                  forwarded
 *
 *  @Return: 0 on success, 1 on warning, -1 on error
 *
 * ****************************************************************************/
int vtkDataProcessor::StartProcessing(vtkDataSender * sender)
{
  if(this->Processing)
    {
    #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-WARNING: Data Processor already processes data" << endl;
    #endif
    return 1;
    }

  if(sender != NULL)
    {
    this->DataSender = sender;
    }
  else
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: No data sender provided. Data processing not possible" << endl;
    #endif
    return -1;
    }

  if(!this->VolumeReconstructionEnabled)
    {//Nothing to start
    return 0;
    }

  //Start process thread
  this->PlayerThreadId =
            this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
            &vtkDataProcessorThread, this);

  //Check if thread successfully started
  if(this->PlayerThreadId != -1)
    {
    this->Processing = true;
    #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-INFO: Successfully started to process data" << endl;
    #endif
    return 0;
    }
  else
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Could not start process thread" << endl;
    #endif
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
 *  @Return: Return 0 on sucess, 1 for warning
 *
 * ****************************************************************************/
int vtkDataProcessor::StopProcessing()
{
  if(this->Processing)
    {//Stop thread
        this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    this->Processing = false;

    #ifdef  DEBUGPROCESSOR
      cout << "Stop processing" <<endl;
    #endif
    }
  else
    {
    #ifdef  DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-WARNING: Try to stop processor thread although not running" << endl;
    #endif
    return 1;
    }

  return 0;
}

/******************************************************************************
 * int vtkDataProcessor::EnableVolumeReconstruction(bool flag)
 *
 *  Turn volume reconstruction ON or OFF
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 *  @Param: bool flag - to set the state of volume reconstruction
 *
 *  @Return: 0 on success
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

    this->calibReader->SetFileName(this->CalibrationFileName);
    this->calibReader->ReadCalibFile();
    this->calibReader->GetClipRectangle(this->clipRectangle);
    this->reconstructor->SetClipRectangle(this->clipRectangle);

    }

  if(!flag && this->VolumeReconstructionEnabled)
    {//Turn of volume reconstruction
    this->reconstructor->Delete();
    }

  this->VolumeReconstructionEnabled = flag;

  #ifdef  DEBUGPROCESSOR
    //this->LogStream << this->GetUpTime()  << " |P-INFO: Reconstruction started" << endl;
  #endif

  return 0;
}

/******************************************************************************
 * int vtkDataProcessor::CheckandUpdateVolume(int index, int dataSenderIndex)
 *
 *  Check if new frame at index "index" fits into existing volume. If expand
 *  volume appropriately
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 *  @Param: int index - Buffer index of new frame and tracker matrix
 *  @Param: int dataSenderIndex: - Flag to check if this is the first time we
 *                                 reconstruct a volume
 *
 *  @Return: 0 on success, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::CheckandUpdateVolume(int index, int dataSenderIndex)
{
  int retVal = 0;

  if(!this->VolumeReconstructionEnabled)
    {
    #ifdef  ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Try to check and update a volume without reconstruction enabled" <<endl;
    #endif
    return -1;
    }

  if(this->IsIndexAvailable(index))
    {
    #ifdef  ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Try to check and update a volume with empty index: " << index <<endl;
    #endif
    return -1;
    }

  vtkImageData * newFrame = this->newFrameMap[index];
  vtkMatrix4x4 * newTrackerMatrix = this->newTrackerMatrixMap[index];

  //Calculate neccessary volume extent------------------------------------------
  double xmin = this->clipRectangle[0], ymin = this->clipRectangle[1],
         xmax = this->clipRectangle[2], ymax = this->clipRectangle[3];

  #ifdef  DEBUGPROCESSOR
    this->LogStream << this->GetUpTime()  << " |P-INFO: Check and update a volume" <<endl;
    this->LogStream <<  "         |P-INFO: Update Matrix" <<endl;
    newTrackerMatrix->Print(this->LogStream );
//    this->LogStream << "         | Frame Extent: "<< newFrame->GetExtent()[0]<< ":"<< newFrame->GetExtent()[1]<< " | "<< newFrame->GetExtent()[2]<< ":"<< newFrame->GetExtent()[3]<< " | "<< newFrame->GetExtent()[4]<< ":"<< newFrame->GetExtent()[5]<< endl
//                    << "         | Frame Origin: " << newFrame->GetOrigin()[0]<< "| " << newFrame->GetOrigin()[1]<< "|" << newFrame->GetOrigin()[2] << endl;
  #endif

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

  // Determine dimensions of reconstructed volume
  for(int j=0; j < 4; j++)
    {
    newTrackerMatrix->MultiplyPoint(imCorners[j],transformedPt);
    minX = min(transformedPt[0], minX);
    minY = min(transformedPt[1], minY);
    minZ = min(transformedPt[2], minZ);
    maxX = max(transformedPt[0], maxX);
    maxY = max(transformedPt[1], maxY);
    maxZ = max(transformedPt[2], maxZ);
    }

  vtkFloatingPointType newOrigin[3] = {minX, minY, minZ};

  double spacing[3];
  newFrame->GetSpacing(spacing);
  int newExtent[6] = { 0, (int)( (maxX - minX) / spacing[0]),
                       0, (int)( (maxY - minY) / spacing[1]),
                       0, (int)( (maxZ - minZ) / spacing[2])};

  #ifdef  DEBUGPROCESSOR
    this->LogStream << this->GetUpTime()  << " |P-INFO: Properties New Frame:" << endl
                    << "         | Origin: " << newOrigin[0] << "| " << newOrigin[1] << " |" << newOrigin[2] <<endl
                    << "         | Dimensions: " << newExtent[1] + 1<< "| "<< newExtent[3] + 1<<" | "<< newExtent[5] + 1<< endl;
  #endif

  //Check if new extent larger than old extent or if origin changed------------
  vtkFloatingPointType oldOrigin[3];
  bool originChanged = false;
  int oldExtent[6];
  bool extentChanged = false;

  if(dataSenderIndex != -2)
    { // -2 == no volume was reconstructed so far

    //Update Origin and Extent
    this->reconstructor->GetOutputOrigin(oldOrigin);
    this->reconstructor->GetOutputExtent(oldExtent);
    for(int i = 0; i < 3; ++i)
      {
      if(newOrigin[i] < oldOrigin[i])
        {
        originChanged = true;
        newExtent[2 * i + 1] = (int) (max(newOrigin[i] + newExtent[2 * i + 1], oldOrigin[i] + oldExtent[2 * i + 1]) - newOrigin[i]);
        }
      else
        {
        newExtent[2 * i + 1] = (int) (max(newOrigin[i] + newExtent[2 * i + 1], oldOrigin[i] + oldExtent[2 * i + 1]) - oldOrigin[i]);
        newOrigin[i] = oldOrigin[i];
        if(newExtent[2 * i + 1] != oldExtent[2 * i + 1])
          {
          extentChanged = true;
          }
        }
      }
    #ifdef  DEBUGPROCESSOR
    this->LogStream << this->GetUpTime()  << " |P-INFO: Old Volume: Origin: "<< oldOrigin[0]<<"| "<< oldOrigin[1]<<"| "<<  oldOrigin[2]<< endl
                            << "         |             Extent:  "<< oldExtent[0]<<"-"<<oldExtent[1] <<" | "<< oldExtent[2]<<"-"<< oldExtent[3]
                                                       <<" | "<< oldExtent[4]<<"-"<< oldExtent[5]<<" "<<endl
                            << "         | New Volume: Origin: "<< newOrigin[0]<<"| "<<newOrigin[1] <<"| "<< newOrigin[2]<<"" << endl
                            << "         |             Extent:  "<< newExtent[0]<<"-"<<newExtent[1] <<" | "<< newExtent[2]<<"-"<< newExtent[3]
                                                       <<" | "<< newExtent[4]<<"-"<< newExtent[5]<<" "<<endl;
    #endif
    }

  if(dataSenderIndex == -2 || originChanged || extentChanged)
    {

    #ifdef  DEBUGPROCESSOR
      if(dataSenderIndex == -2)
        {
        this->LogStream << this->GetUpTime()  << " |P-INFO: Create initial volume" << " | "  << endl;
        }
      else
        {
        this->LogStream << this->GetUpTime()  << " |P-INFO: Update Existing volume" << " | "  << endl;
        }
      #endif
    //Create new volume-------------------------------------------------------
    this->reconstructor->SetOutputExtent(newExtent);
    this->reconstructor->SetOutputSpacing(spacing);
    this->reconstructor->SetOutputOrigin(newOrigin);

    this->reconstructor->SetSlice(newFrame);
    this->reconstructor->GetOutput()->Update();

    this->reconstructor->SetSliceAxes(newTrackerMatrix);
    //Set the correct amount of the output volume's scalar components
    this->reconstructor->GetOutput()->SetNumberOfScalarComponents(newFrame->GetNumberOfScalarComponents());
    this->reconstructor->GetOutput()->AllocateScalars();

#ifdef MERGE
    if(originChanged || extentChanged)
#else
    if(false)
#endif
      {
      #ifdef  DEBUGPROCESSOR
        this->LogStream << this->GetUpTime()  << " |P-INFO: Expand volume | DataSenderIndex: "<< dataSenderIndex << endl;
      #endif

      //Copy Old Volume at correct position into new volume
      if(this->oldVolume == NULL)
        {
        #ifdef  ERRORPROCESSOR
          this->LogStream << this->GetUpTime()  << " |P-ERROR: No oldVolume available => can not expand volume" << endl;
        #endif
        retVal = -1;
        }
      else if(this->MergeVolumes(reconstructor->GetOutput(),newOrigin,newExtent,
                                 this->oldVolume, oldOrigin,oldExtent,
                                 newFrame->GetNumberOfScalarComponents())
              == -1)
        {
        #ifdef  ERRORPROCESSOR
          this->LogStream << this->GetUpTime()  << " |P-ERROR: Expand failed" << endl;
        #endif
        retVal = -1;
        }
      }
    }

  this->ResetOldVolume(dataSenderIndex);

   return retVal;
}

/******************************************************************************
 *  int vtkDataProcessor::ReconstructVolume(int index)
 *
 *  Add frame and matrix at index to reconstructed volume
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 *  @Param: int index - Buffer index of frame and matrix
 *
 *  @Return: 0 on success, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::ReconstructVolume(int index)
{
  if(!this->VolumeReconstructionEnabled)
    {
    #ifdef  ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Try to reconstruct a volume without reconstruction enabled" <<endl;
    #endif
    return -1;
    }

  if(this->IsIndexAvailable(index))
    {
    #ifdef  ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Try to reconstruct volume with empty index: " << index <<endl;
    #endif
    return -1;
    }

    this->reconstructor->SetSliceAxes(this->newTrackerMatrixMap[index]); //Set current trackingmatrix
    this->reconstructor->SetSlice(this->newFrameMap[index]);

    #ifdef  DEBUGPROCESSOR
      double reconstructionTime = this->GetUpTime();
    #endif

    this->reconstructor->InsertSlice(); //Add current slice to the reconstructor

  //Reconstruction done -> Check for errors
  if( this->reconstructor->GetPixelCount() > 0)
    {
    #ifdef  DEBUGPROCESSOR
          this->LogStream << this->GetUpTime()  << " |P-INFO: Reconstruction successfully completed" << " | " << endl;
          this->LogStream << "         | Inserted " <<     this->reconstructor->GetPixelCount() << " pixels into the output volume" <<endl
                          << "         | Reconstruction time: "<< this->GetUpTime() - reconstructionTime << endl;
    #endif
    }
  else
    {
    #ifdef  ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Reconstruction failed" << " | " << endl;
    #endif
    return -1;
    }

  #ifdef  DEBUGPROCESSOR
    reconstructionTime = this->GetUpTime();
  #endif
  //this->reconstructor->FillHolesInOutput();
  #ifdef  DEBUGPROCESSOR
    this->LogStream << "         | FillHolesInOutput took: " << this->GetUpTime() - reconstructionTime <<endl;
  #endif

  return 0;
}

/******************************************************************************
 *  int vtkDataProcessor::ForwardData()
 *
 *  Prepare and forward reconstructed volume to DataSender
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Return: Success: Index of DataSender's newDataBuffer were forwarded volume
 *                    is stored
 *           Failure: -1
 *
 * ****************************************************************************/
int vtkDataProcessor::ForwardData()
{
//Postprocess reconstructed volume
#ifdef REMOVE_ALPHA_CHANNEL
  // To remove the alpha channel of the reconstructed volume
  vtkImageExtractComponents *extract = vtkImageExtractComponents::New();
  extract->SetInput(this->reconstructor->GetOutput());
  extract->SetComponents(0);

  vtkImageData * reconstructedVolume = extract->GetOutput();

  extract->Delete();

#else
  vtkImageData * reconstructedVolume = this->reconstructor->GetOutput();

#endif
  //Generate and fill volume to be forwarded-----------------------------------
  vtkImageData* volumeToForward = vtkImageData::New();


  //Dimensions
#ifndef NEWFORWARDING
  int dimensions[3];
  reconstructedVolume->GetDimensions(dimensions);
  volumeToForward->SetDimensions(dimensions);

  //Spacing
  double ouputSpacing[3];
  reconstructedVolume->GetSpacing(ouputSpacing);
  volumeToForward->SetSpacing(ouputSpacing);

  //SetScalarType and components
  volumeToForward->SetScalarType(reconstructedVolume->GetScalarType());
  volumeToForward->SetNumberOfScalarComponents(reconstructedVolume->GetNumberOfScalarComponents());
  volumeToForward->AllocateScalars();
#endif


  #ifdef DEBUGPROCESSOR
    double copyTime = this->GetUpTime();
    int counter = 0;
  #endif

#ifdef NEWFORWARDING
  this->DuplicateImage(reconstructedVolume, volumeToForward);
#else
  //Copy reconstructed volume for forwarding
  unsigned char * pReconstructedVolume = (unsigned char *) reconstructedVolume->GetScalarPointer();
  unsigned char * pVolumeToForward = (unsigned char *) volumeToForward->GetScalarPointer();

  for(int i = 0 ; i < dimensions[2]; ++i)
    {
    for(int j = 0 ; j < dimensions[1]; ++j)
      {
      for(int k = 0 ; k < dimensions[0]; ++k)
        {
        *pVolumeToForward = *pReconstructedVolume;
        pVolumeToForward++;
        pReconstructedVolume++;
        #ifdef DEBUGPROCESSOR
          counter++;
        #endif
        }
      }
    }

#endif


  //Create and Fill Matrix for forwarding
  vtkMatrix4x4 * matrix = vtkMatrix4x4::New();
  this->GetVolumeMatrix(matrix);

  //Forward data to sender
  int retval = this->DataSender->NewData(volumeToForward, matrix);
  this->oldVolume = volumeToForward;

  if(retval != -1)
    {
    #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-INFO: Volume forwarded to data sender " << " | Copytime: " << this->GetUpTime() -  copyTime << endl
                      << "         | Pixels: "<< counter << " | Data Sender Index: " << retval << endl
                      << "         | Volume Dimensions: "<< volumeToForward->GetDimensions()[0] << " | "<< volumeToForward->GetDimensions()[1] << " | "<< volumeToForward->GetDimensions()[2] << " | " << endl
                      << "         | SenderIndex: "<< retval << " | "  << endl;
    #endif
    }
  else
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Could not forward volume to data sender" << " | " << endl;
    #endif
    }

  return retval;
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
 *  @Return: 0 on success, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::AddFrameToFrameMap(int index, vtkImageData* frame)
{
  if(this->newFrameMap.find(index) != this->newFrameMap.end())
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: new image map already has an image for index: " << index << endl;
    #endif
    return -1;
    }

  this->newFrameMap[index] = frame;
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
 *  @Return: 0 on success, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::AddMatrixToMatrixMap(int index, vtkMatrix4x4* matrix)
{
  if(this->newTrackerMatrixMap.find(index) != this->newTrackerMatrixMap.end())
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: new tracker matrix map already has a matrix for index: " << index << endl;
    #endif
    return -1;
    }

  this->newTrackerMatrixMap[index] = matrix;
  return 0;
}

/******************************************************************************
 *  int vtkDataProcessor::NewData(vtkImageData* frame, vtkMatrix4x4* trackerMatrix)
 *
 *  Adds a new frame to the index(key) - matrix(value) map
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: vtkImageData* frame - new frame to add to frame map
 *  @Param: vtkMatrix4x4* trackerMatrix - new matrix to add to tracker matrix map
 *
 *  @Return: 0 if new data is successfully stored, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::NewData(vtkImageData* frame, vtkMatrix4x4* trackerMatrix)
{
  //Check if data processing started
  if(!this->Processing)
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Data Processor is not started; Cannot accept data" << endl;
    #endif
    return -1;
    }

  //Check if volume reconstruction enabled
  if(!this->VolumeReconstructionEnabled)
    {//No, forward data to data sender
    return this->DataSender->NewData(frame, trackerMatrix);
          }

  //Check if new data buffer is full
  if(this->IsNewDataBufferFull())
    {
    #ifdef TIMINGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-WARNING: Processors new data buffer is full" << endl;
    #endif
    return -1;
    }

  //Get new index
  int index = this->IncrementBufferIndex(1);

  //Add new data to Buffer
  if(this->AddFrameToFrameMap(index, frame) == -1) //Imageframe
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Processor cannot add new frame to its frame map" << endl;
    #endif
    this->IncrementBufferIndex(-1);
    return -1;
    }
  if(this->AddMatrixToMatrixMap(index, trackerMatrix) == -1) //TrackerMatrix
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Processor cannot add new tracker matrix to its matrix map" << endl;
    #endif
    this->IncrementBufferIndex(-1);
    return -1;
    }

  this->newDataBuffer.push(index);//Add Index to new data buffer queue

  return 0;
}

/******************************************************************************
 *  bool vtkDataProcessor::IsNewDataBufferEmpty()
 *
 *  Checks if new data buffer is empty
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Return: True, if empty, otherwise false
 *
 * ****************************************************************************/
bool vtkDataProcessor::IsNewDataBufferEmpty()
{
  return this->newDataBuffer.empty();
}

/******************************************************************************
 *  bool vtkDataProcessor::IsNewDataBufferFull()
 *
 *  Checks if new data buffer is full
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Return: True, if full, otherwise false
 *
 * ****************************************************************************/
bool vtkDataProcessor::IsNewDataBufferFull()
{
  if(this->newDataBuffer.size() >= this->newDataBufferSize)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/******************************************************************************
 *  void vtkDataProcessor::GetVolumeMatrix(igtl::Matrix4x4& matrix)
 *
 *  Fills given transformation matrix with correct data
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: Matrix4x4& matrix - Matrix to fill
 *
 * ****************************************************************************/
void vtkDataProcessor::GetVolumeMatrix(vtkMatrix4x4* matrix)
{
  float position[3];
  float orientation[4];

 #ifdef USE_TRACKER_DEVICE
 //NDI tracker matrix looks like
  //  0  0 -1 90
  //  0  1  0  0
  //  1  0  0  0
  //  0  0  0  1
  matrix->Element[0][0] =   0.0;  matrix->Element[1][0] =  0.0;  matrix->Element[2][0] =  1.0; matrix->Element[3][0] = 0.0;
  matrix->Element[0][1] =   0.0;  matrix->Element[1][1] =  1.0;  matrix->Element[2][1] =  0.0; matrix->Element[3][1] = 0.0;
  matrix->Element[0][2] =  -1.0;  matrix->Element[1][2] =  0.0;  matrix->Element[2][2] =  0.0; matrix->Element[3][2] = 0.0;
  matrix->Element[0][3] =  90.0;  matrix->Element[1][3] =  0.0;  matrix->Element[2][3] =  0.0; matrix->Element[3][3] = 1.0;
#else
  //Tracker simulator matrix looks like
  // -1  0  0  0
  //  0  0  1  0
  //  0  1  0  0
  //  0  0  0  1
  matrix->Element[0][0] =  -1.0;  matrix->Element[1][0] =  0.0;  matrix->Element[2][0] =  0.0; matrix->Element[3][0] = 0.0;
  matrix->Element[0][1] =   0.0;  matrix->Element[1][1] =  0.0;  matrix->Element[2][1] =  1.0; matrix->Element[3][1] = 0.0;
  matrix->Element[0][2] =   0.0;  matrix->Element[1][2] =  1.0;  matrix->Element[2][2] =  0.0; matrix->Element[3][2] = 0.0;
  matrix->Element[0][3] =   0.0;  matrix->Element[1][3] =  0.0;  matrix->Element[2][3] =  0.0; matrix->Element[3][3] = 1.0;
#endif

#ifdef DEBUGPROCESSOR
//    this->LogStream << this->GetUpTime()  << " |P-INFO: OpenIGTLink image message matrix" << endl;
//    matrix->Print(this->LogStream);
#endif
}

/******************************************************************************
 *  int vtkDataProcessor::MergeVolumes(vtkImageData* newVolume,
 *                        vtkFloatingPointType* originNewVolume,
 *                        int* extentNewVolume,
 *                        vtkImageData* oldVolume,
 *                        vtkFloatingPointType* originOldVolume,
 *                        int* extentOldVolume)
 *
 *  Merges old Volume in new volume at correct position
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: vtkImageData* newVolume - New volume into which old volume is merged
 *  @Param: vtkFloatingPointType* originNewVolume - Origin of new volume
 *  @Param: int* extentNewVolume - Extent of new volume
 *  @Param: vtkImageData* oldVolume - Old volume which is merged in new volume
 *  @Param: vtkFloatingPointType* originOldVolume - Origin of old volume
 *  @Param: int* extentOldVolume - Old volume's extent
 *
 *  @Return: 0 on success, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::MergeVolumes(vtkImageData* newVolume,
                                   vtkFloatingPointType* originNewVolume,
                                   int* extentNewVolume,
                                   vtkImageData* oldVolume,
                                   vtkFloatingPointType* originOldVolume,
                                   int* extentOldVolume,
                                   int ScalarComponents)
{
  if(oldVolume == NULL){
    return -1;
  }

  int x, y, z;

  int counter = 0;
  vtkIdType inIncX, inIncY, inIncZ;
  vtkIdType outIncX, outIncY, outIncZ;

  //Calc x, y, z start and end coordinates of old volume in new volume
  int xStart = (int) originNewVolume[0] + ((int) originOldVolume[0] - (int) originNewVolume[0]) * ScalarComponents;
  int xEnd   = xStart + extentOldVolume[1] * ScalarComponents;
  int yStart = (int) originOldVolume[1];
  int yEnd   = yStart + extentOldVolume[3];
  int zStart = (int) originOldVolume[2];
  int zEnd   = zStart + extentOldVolume[5];

  // Get increments to march through data
  oldVolume->GetContinuousIncrements(extentNewVolume, inIncX, inIncY, inIncZ);
  newVolume->GetContinuousIncrements(extentNewVolume, outIncX, outIncY, outIncZ);

  int rowLength = (extentNewVolume[1] - extentNewVolume[0] + 1) * ScalarComponents + outIncY;
  int oldRowLength = extentOldVolume[1] - extentOldVolume[0] + 1;
  int frameSize = (extentNewVolume[3] - extentNewVolume[2] + 1) * rowLength + outIncZ;

  int spaceBeforeXStart = xStart - (int) originNewVolume[0];
  int spaceAfterXEnd = (int) originNewVolume[0] + extentNewVolume[1] * ScalarComponents - xEnd;

  int spaceBeforeYStart = (yStart - (int) originNewVolume[1] + outIncY) * rowLength;
  int spaceAfterYEnd = ((int) originNewVolume[1] + extentNewVolume[3] * ScalarComponents - yEnd) * rowLength;

  int spaceBeforeZStart = (zStart - (int) originNewVolume[2]) * frameSize;

  //Get Data pointer
  unsigned char* pDataNewVolume = (unsigned char *) newVolume->GetScalarPointer();
  unsigned char* pDataOldVolume = (unsigned char *) oldVolume->GetScalarPointer();

  try
   {
    #ifdef NEWMERGE
    //Copy Data
    pDataNewVolume += spaceBeforeZStart;
    for(z = zStart; z <= zEnd; ++z)
      {
      pDataNewVolume += spaceBeforeYStart;
      for(y = yStart; y <= yEnd; ++y)
        {
          pDataNewVolume += spaceBeforeXStart;
          memcpy(pDataNewVolume, pDataOldVolume, oldRowLength);
          pDataOldVolume += oldRowLength;
          pDataNewVolume += oldRowLength;
          counter += oldRowLength;
//          for(x = xStart; x <= xEnd; ++x){//Copy data from old to new volume
//          *pDataNewVolume = *pDataOldVolume;
//            ++pDataOldVolume;
//            ++pDataNewVolume;
//            ++counter;
//          }

          pDataNewVolume += spaceAfterXEnd;

          pDataNewVolume += outIncY;
          pDataOldVolume += inIncY;
        }
      pDataNewVolume += spaceAfterYEnd;

      pDataNewVolume += outIncZ;
      pDataNewVolume += inIncZ;
      }

    #else //NEWMERGE
    //Copy old volume into new volume
    for( z = (int) originNewVolume[2]; z <= zEnd; ++z)
      {
      for( y = (int) originNewVolume[1]; y <= originNewVolume[1] + extentNewVolume[3]; ++y)
        {

        for( x = (int)originNewVolume[0]; x <= originNewVolume[0] + extentNewVolume[1] * ScalarComponents; ++x)
          {
          if(   (x >= xStart && x <= xEnd)
             && (y >= yStart && y <= yEnd)
             && (z >= zStart && z <= zEnd))
            {
            *pDataNewVolume = *pDataOldVolume;
            ++pDataOldVolume;
            ++counter;
            }
          ++pDataNewVolume;
          }
        pDataNewVolume += outIncY;
        pDataOldVolume += inIncY;
        }
      pDataNewVolume += outIncZ;
      pDataOldVolume += inIncZ;
      }

    #endif//NEWMERGE
    }//End try
  catch (...)
    {
    throw;
    }

  if(counter != 0)
    {
    #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-INFO: Copied " << counter << " Pixel in expanded volume" << " | " << endl;
    #endif
    return 0;
    }
  else
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: No pixels in expanded volume copied"  << " | " << endl;
    #endif
    return -1;
    }
}

/******************************************************************************
 *  int vtkDataProcessor::DeleteData(int index)
 *
 *  Delete Data at at index "index"
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: int index - Delete data for this index
 *
 * ****************************************************************************/
int vtkDataProcessor::DeleteData(int index)
{
  if(this->IsIndexAvailable(index))
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Cannot delete data; Index (" << index << ") empty "<< endl;
    #endif
    return -1;
    }

  this->newFrameMap[index]->Delete();
  this->newFrameMap.erase(index);
  this->newTrackerMatrixMap[index]->Delete();
  this->newTrackerMatrixMap.erase(index);
  this->newDataBuffer.pop();

  return 0;
}

/******************************************************************************
 * int vtkDataProcessor::GetHeadOfNewDataBuffer()
 *
 *  Return index of the head of new data buffer queue
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: int index - Delete data for this index
 *
 * ****************************************************************************/
int vtkDataProcessor::GetHeadOfNewDataBuffer()
{
  int head = this->newDataBuffer.front();
  return head;
}

/******************************************************************************
 * int vtkDataProcessor::IncrementBufferIndex(int increment)
 *
 *  Icrement buffer and return new current buffer index
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: int increment - postions to increment
 *
 * ****************************************************************************/
int vtkDataProcessor::IncrementBufferIndex(int increment)
{
  this->newDataBufferIndex = (this->newDataBufferIndex + increment) % this->newDataBufferSize;

  return this->newDataBufferIndex;
}

/******************************************************************************
 * double vtkDataProcessor::GetUpTime()
 *
 *  Returns elapsed Time since program start
 *
 *  @Author:Jan Gumprecht
 *  @Date:  2.February 2009
 *
 * ****************************************************************************/
double vtkDataProcessor::GetUpTime()
{
  return vtkTimerLog::GetUniversalTime() - this->GetStartUpTime();
}

/******************************************************************************
 * bool vtkDataProcessor::IsIndexAvailable(int index)
 *
 *  Checks if given index is available
 *
 *  @Author:Jan Gumprecht
 *  @Date:  2.February 2009
 *
 *  @Return: True, if index is available
 *           False, if index is used
 *
 * ****************************************************************************/
bool vtkDataProcessor::IsIndexAvailable(int index)
{
  if(    this->newFrameMap.find(index) == this->newFrameMap.end()
      && this->newTrackerMatrixMap.find(index) == this->newTrackerMatrixMap.end())
    {
    return true;
    }
  else
    {
    return false;
    }
}

/******************************************************************************
 * void vtkDataProcessor::SetLogStream(ofstream &LogStream)
 *
 *  Redirects Logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 * ****************************************************************************/
void vtkDataProcessor::SetLogStream(ofstream &LogStream)
{
  this->LogStream.ostream::rdbuf(LogStream.ostream::rdbuf());
  this->LogStream.precision(6);
  this->LogStream.setf(ios::fixed,ios::floatfield);
}

/******************************************************************************
 * ofstream& vtkDataProcessor::GetLogStream()
 *
 *  Returns logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Return: Logstream
 *
 * ****************************************************************************/
ofstream& vtkDataProcessor::GetLogStream()
{
        return this->LogStream;
}

/******************************************************************************
 * void vtkDataProcessor::ResetOldVolume()
 *
 *  Resets the last reconstructed volume
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 * ****************************************************************************/
void vtkDataProcessor::ResetOldVolume(int dataSenderIndex)
{
  this->oldVolume = NULL;
  this->DataSender->UnlockData(dataSenderIndex, DATAPROCESSOR);
  this->DataSender->TryToDeleteData(dataSenderIndex);
}

/******************************************************************************
 * void vtkDataProcessor::DuplicateFrame(vtkImageData * original, vtkImageData * duplicate)
 *
 *  Duplicates Image inData to Image outData
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Param: vtkImageData * original - Original
 *  @Param: vtkImageData * copy - Copy
 *
 * ****************************************************************************/
void vtkDataProcessor::DuplicateImage(vtkImageData * original, vtkImageData * duplicate)
{
  int idxR, idxY, idxZ;
  int maxY, maxZ;
  vtkIdType inIncX, inIncY, inIncZ;
  vtkIdType outIncX, outIncY, outIncZ;
  int rowLength;
  int extent[6];

  //Duplicate Properties
  original->GetExtent(extent);
  duplicate->SetExtent(extent);

  duplicate->SetSpacing(original->GetSpacing());
  duplicate->SetScalarType(original->GetScalarType());
  duplicate->SetOrigin(original->GetOrigin());
  duplicate->SetNumberOfScalarComponents(original->GetNumberOfScalarComponents());

  //Reserve memory
  duplicate->AllocateScalars();

  // find the region to loop over
  rowLength = (extent[1] - extent[0]+1)*original->GetNumberOfScalarComponents();
  maxY = extent[3] - extent[2];
  maxZ = extent[5] - extent[4];

  // Get increments to march through data
  original->GetContinuousIncrements(extent, inIncX, inIncY, inIncZ);
  duplicate->GetContinuousIncrements(extent, outIncX, outIncY, outIncZ);

  unsigned char * outPtr = (unsigned char*) duplicate->GetScalarPointer();
  unsigned char * inPtr = (unsigned char*) original->GetScalarPointer();


  if(inIncY == outIncY && inIncZ == outIncZ)
    {
    long long bytesToCopy = ((rowLength + outIncY) * (maxY + 1) + outIncZ) * (maxZ + 1);
    memcpy(outPtr, inPtr, bytesToCopy);
    }
  else
    {

    //Copy image data
    for (idxZ = 0; idxZ <= maxZ; idxZ++)
      {
      for (idxY = 0; idxY <= maxY; idxY++)
        {
        for (idxR = 0; idxR < rowLength; idxR++)
          {
          // Pixel operation
          *outPtr = *inPtr;
          outPtr++;
          inPtr++;
          }
        outPtr += outIncY;
        inPtr += inIncY;
        }
      outPtr += outIncZ;
      inPtr += inIncZ;
      }
    }
}

