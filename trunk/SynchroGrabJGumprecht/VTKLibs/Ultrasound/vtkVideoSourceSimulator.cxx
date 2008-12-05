/*=========================================================================

  Module:    $RCSfile: vtkVideoSourceSimulator.cxx,v $
  Author:  Siddharth Vikal, Queens School Of Computing  

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.

Author:  Jan Gumprecht, Harvard Medical School
Copyright (c) 2008, Brigham and Women's Hospital, Boston, MA
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

#include "vtkVideoSourceSimulator.h"

#include "vtkImageData.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnsignedCharArray.h"
#include "vtksys/SystemTools.hxx"

#include <ctype.h>

#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"

// because of warnings in windows header push and pop the warning level
#ifdef _MSC_VER
#pragma warning (push, 3)
#endif

#include <vector>
#include <string>

#ifdef _MSC_VER
#pragma warning (pop)
#endif



vtkCxxRevisionMacro(vtkVideoSourceSimulator, "$Revision: 1.0$");
//vtkStandardNewMacro(vtkWin32VideoSource);
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkVideoSourceSimulator);

//----------------------------------------------------------------------------

vtkVideoSourceSimulator* vtkVideoSourceSimulator::Instance = 0;

//----------------------------------------------------------------------------
vtkVideoSourceSimulator::vtkVideoSourceSimulator()
{
  this->Initialized = 0;

  this->FrameSize[0] = SLICE_X_LENGTH;
  this->FrameSize[1] = SLICE_Y_LENGTH;
  this->FrameSize[2] = SLICE_Z_LENGTH;
  
  this->FrameRate = 30; // in fps
  this->AcquisitionDataType = 0x00000004; //corresponds to type: BPost 8-bit  
  this->ImagingMode = 0; //corresponds to BMode imaging  

  //note, input data i.e. data from UltraSound machine is always uncompressed rgb 
  //so, by default we set the output format as rgb
  this->OutputFormat = VTK_LUMINANCE;
  this->NumberOfScalarComponents = 1;
  this->FrameBufferBitsPerPixel = 8;
  this->FlipFrames = 0;
  this->FrameBufferRowAlignment = 1; 
  
  this->PlayerThreader = vtkMultiThreader::New();
  this->PlayerThreadId = -1;
 
}

//----------------------------------------------------------------------------
vtkVideoSourceSimulator::~vtkVideoSourceSimulator()
{ 
  this->vtkVideoSourceSimulator::ReleaseSystemResources();
  
  this->PlayerThreader->Delete();
  
}


//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkVideoSourceSimulator* vtkVideoSourceSimulator::New()
{
  vtkVideoSourceSimulator* ret = vtkVideoSourceSimulator::GetInstance();
  ret->Register(NULL);
  return ret;
}


//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkVideoSourceSimulator* vtkVideoSourceSimulator::GetInstance()
{
  if(!vtkVideoSourceSimulator::Instance)
    {
    // Try the factory first
   vtkVideoSourceSimulator::Instance = (vtkVideoSourceSimulator*)vtkObjectFactory::CreateInstance("vtkVideoSourceSimulator");    
   if(!vtkVideoSourceSimulator::Instance)
     {
     vtkVideoSourceSimulator::Instance = new vtkVideoSourceSimulator();     
     }
   if(!vtkVideoSourceSimulator::Instance)
     {
     int error = 0;
     }
    }
  // return the instance
  return vtkVideoSourceSimulator::Instance;
}

//----------------------------------------------------------------------------
void vtkVideoSourceSimulator::SetInstance(vtkVideoSourceSimulator* instance)
{
  if (vtkVideoSourceSimulator::Instance==instance)
    {
    return;
    }
  // preferably this will be NULL
  if (vtkVideoSourceSimulator::Instance)
    {
    vtkVideoSourceSimulator::Instance->Delete();;
    }
  vtkVideoSourceSimulator::Instance = instance;
  if (!instance)
    {
    return;
    }
  // user will call ->Delete() after setting instance
  instance->Register(NULL);
}

//----------------------------------------------------------------------------
void vtkVideoSourceSimulator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Imaging mode: " << this->ImagingMode << "\n";
  os << indent << "Frame rate: " << this->FrameRate << "fps\n";
  
}


//----------------------------------------------------------------------------
void vtkVideoSourceSimulator::Initialize()
{
  if (this->Initialized)
    {
    return;
    }

  this->FrameBufferMutex->Lock();
  this->DoFormatSetup();
  this->FrameBufferMutex->Unlock();

#ifndef NEW_SIMULATOR
  this->FillFrameBuffer();
#endif

  this->UpdateFrameBuffer();

  this->Initialized = 1;
}

//----------------------------------------------------------------------------
void vtkVideoSourceSimulator::ReleaseSystemResources()
{
// Nothing to do here
}

//----------------------------------------------------------------------------
//Grab a single frame
void vtkVideoSourceSimulator::Grab()
{
  if (this->Recording)
    {
    return;
    }

  // ensure that the frame buffer is properly initialized
  this->Initialize();
  if (!this->Initialized)
    {
    return;
    }

  // just do the grab, the callback does the rest
  this->SetStartTimeStamp(vtkTimerLog::GetUniversalTime());

}


static inline void vtkSleep(double duration)
{
  duration = duration; // avoid warnings
  // sleep according to OS preference
#ifdef _WIN32
  Sleep((int)(1000*duration));
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi)
  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = (int)duration;
  sleep_time.tv_nsec = (int)(1000000000*(duration-sleep_time.tv_sec));
  nanosleep(&sleep_time,&dummy);
#endif
}

//----------------------------------------------------------------------------
// Sleep until the specified absolute time has arrived.
// You must pass a handle to the current thread.  
// If '0' is returned, then the thread was aborted before or during the wait.
static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time)
{
  // loop either until the time has arrived or until the thread is ended
  for (int i = 0;; i++)
    {
      double remaining = time - vtkTimerLog::GetUniversalTime();

      // check to see if we have reached the specified time
      if (remaining <= 0)
        {
        if (i == 0)
          {
          vtkGenericWarningMacro("Dropped a video frame.");
          }
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

//----------------------------------------------------------------------------
// this function runs in an alternate thread to asyncronously grab frames
static void *vtkVideoSourceSimulatorRecordThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkVideoSourceSimulator *self = (vtkVideoSourceSimulator *)(data->UserData);
  
  double startTime = vtkTimerLog::GetUniversalTime();
  double rate = self->GetFrameRate();
  int frame = 0;

  do
    {
      self->InternalGrab();
      frame++;
    }    
  while (vtkThreadSleep(data, startTime + frame/rate));

  return NULL;
}

//----------------------------------------------------------------------------
//Record images
void vtkVideoSourceSimulator::Record()
{
  this->Initialize();
  if (!this->Initialized)
    {
    return;
    }

  if (this->Playing)
    {
    this->Stop();
    }

  if (!this->Recording)
    {
    this->Recording = 1;
    this->Modified();
    }

#ifdef NEW_SIMULATOR    
  this->PlayerThreadId =  
            this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
            &vtkVideoSourceSimulatorRecordThread,this);
#endif
     
}
    
//----------------------------------------------------------------------------
void vtkVideoSourceSimulator::Play()
{
  this->vtkVideoSource::Play();
}
    
//----------------------------------------------------------------------------
void vtkVideoSourceSimulator::Stop()
{
  if (this->Recording)
    {
    this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    this->Recording = 0;
    this->Modified();
    }

 if (this->Playing)
    {
    this->vtkVideoSource::Stop();
    }
}


//----------------------------------------------------------------------------
int vtkVideoSourceSimulator::RequestInformation(
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  int i;
  int extent[6];

  // ensure that the hardware is initialized.
  this->Initialize();

  for (i = 0; i < 3; i++)
    {
    // initially set extent to the OutputWholeExtent
    extent[2*i] = this->OutputWholeExtent[2*i];
    extent[2*i+1] = this->OutputWholeExtent[2*i+1];
    // if 'flag' is set in output extent, use the FrameBufferExtent instead
    if (extent[2*i+1] < extent[2*i])
      {
      extent[2*i] = 0; 
      extent[2*i+1] = \
        this->FrameBufferExtent[2*i+1] - this->FrameBufferExtent[2*i];
      }
    this->FrameOutputExtent[2*i] = extent[2*i];
    this->FrameOutputExtent[2*i+1] = extent[2*i+1];
    }

  int numFrames = this->NumberOfOutputFrames;
  if (numFrames < 1)
    {
    numFrames = 1;
    }
  if (numFrames > this->FrameBufferSize)
    {
    numFrames = this->FrameBufferSize;
    }

  // multiply Z extent by number of frames to output
  extent[5] = extent[4] + (extent[5]-extent[4]+1) * numFrames - 1;

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);
  
  outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),extent,6);

  // set the spacing
  outInfo->Set(vtkDataObject::SPACING(),this->DataSpacing,3);

  // set the origin.
  outInfo->Set(vtkDataObject::ORIGIN(),this->DataOrigin,3);

  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR, 
    this->NumberOfScalarComponents);

  return 1;
}


//----------------------------------------------------------------------------
int vtkVideoSourceSimulator::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *vtkNotUsed(outputVector))
{
  vtkImageData *data = this->AllocateOutputData(this->GetOutput());
  int i,j;

  int outputExtent[6];     // will later be clipped in Z to a single frame
  int saveOutputExtent[6]; // will possibly contain multiple frames
  data->GetExtent(outputExtent);
  for (i = 0; i < 6; i++)
    {
    saveOutputExtent[i] = outputExtent[i];
    }
  // clip to extent to the Z size of one frame  
  outputExtent[4] = this->FrameOutputExtent[4]; 
  outputExtent[5] = this->FrameOutputExtent[5]; 

  int frameExtentX = this->FrameBufferExtent[1]-this->FrameBufferExtent[0]+1;
  int frameExtentY = this->FrameBufferExtent[3]-this->FrameBufferExtent[2]+1;
  int frameExtentZ = this->FrameBufferExtent[5]-this->FrameBufferExtent[4]+1;

  int extentX = outputExtent[1]-outputExtent[0]+1;
  int extentY = outputExtent[3]-outputExtent[2]+1;
  int extentZ = outputExtent[5]-outputExtent[4]+1;

  // if the output is more than a single frame,
  // then the output will cover a partial or full first frame,
  // several full frames, and a partial or full last frame

  // index and Z size of the first frame in the output extent
  int firstFrame = (saveOutputExtent[4]-outputExtent[4])/extentZ;
  int firstOutputExtent4 = saveOutputExtent[4] - extentZ*firstFrame;

  // index and Z size of the final frame in the output extent
  int finalFrame = (saveOutputExtent[5]-outputExtent[4])/extentZ;
  int finalOutputExtent5 = saveOutputExtent[5] - extentZ*finalFrame;

  char *outPtr = (char *)data->GetScalarPointer();
  char *outPtrTmp;

  int inIncY = (frameExtentX*this->FrameBufferBitsPerPixel + 7)/8;
  inIncY = ((inIncY + this->FrameBufferRowAlignment - 1)/
            this->FrameBufferRowAlignment)*this->FrameBufferRowAlignment;
  int inIncZ = inIncY*frameExtentY;

  int outIncX = this->FrameBufferBitsPerPixel/8;
  int outIncY = outIncX*extentX;
  int outIncZ = outIncY*extentY;

  int inPadX = 0;
  int inPadY = 0;
  int inPadZ; // do inPadZ later

  int outPadX = -outputExtent[0];
  int outPadY = -outputExtent[2];
  int outPadZ;  // do outPadZ later

  if (outPadX < 0)
    {
    inPadX -= outPadX;
    outPadX = 0;
    }

  if (outPadY < 0)
    {
    inPadY -= outPadY;
    outPadY = 0;
    }

  int outX = frameExtentX - inPadX; 
  int outY = frameExtentY - inPadY; 
  int outZ; // do outZ later

  if (outX > extentX - outPadX)
    {
    outX = extentX - outPadX;
    }
  if (outY > extentY - outPadY)
    {
    outY = extentY - outPadY;
    }

  // if output extent has changed, need to initialize output to black
  for (i = 0; i < 3; i++)
    {
    if (saveOutputExtent[i] != this->LastOutputExtent[i])
      {
      this->LastOutputExtent[i] = saveOutputExtent[i];
      this->OutputNeedsInitialization = 1;
      }
    }

  // ditto for number of scalar components
  if (data->GetNumberOfScalarComponents() != 
      this->LastNumberOfScalarComponents)
    {
    this->LastNumberOfScalarComponents = data->GetNumberOfScalarComponents();
    this->OutputNeedsInitialization = 1;
    }

  // initialize output to zero only when necessary
  if (this->OutputNeedsInitialization)
    {
    memset(outPtr,0,
           (saveOutputExtent[1]-saveOutputExtent[0]+1)*
           (saveOutputExtent[3]-saveOutputExtent[2]+1)*
           (saveOutputExtent[5]-saveOutputExtent[4]+1)*outIncX);
    this->OutputNeedsInitialization = 0;
    } 

  // we have to modify the outputExtent of the first frame,
  // because it might be complete (it will be restored after
  // the first frame has been copied to the output)
  int saveOutputExtent4 = outputExtent[4];
  outputExtent[4] = firstOutputExtent4;

  this->FrameBufferMutex->Lock();

  int index = this->FrameBufferIndex;
  this->FrameTimeStamp = 
    this->FrameBufferTimeStamps[index % this->FrameBufferSize];

  int frame;
  for (frame = firstFrame; frame <= finalFrame; frame++)
    {
    if (frame == finalFrame)
      {
      outputExtent[5] = finalOutputExtent5;
      } 
    
    vtkDataArray *frameBuffer = reinterpret_cast<vtkDataArray *>(this->FrameBuffer[(index + frame) % this->FrameBufferSize]);

    char *inPtr = reinterpret_cast<char*>(frameBuffer->GetVoidPointer(0));
    char *inPtrTmp ;

    extentZ = outputExtent[5]-outputExtent[4]+1;
    inPadZ = 0;
    outPadZ = -outputExtent[4];
    
    if (outPadZ < 0)
      {
      inPadZ -= outPadZ;
      outPadZ = 0;
      }

    outZ = frameExtentZ - inPadZ;

    if (outZ > extentZ - outPadZ)
      {
      outZ = extentZ - outPadZ;
      }

    if (this->FlipFrames)
      { // apply a vertical flip while copying to output
      outPtr += outIncZ*outPadZ+outIncY*outPadY+outIncX*outPadX;
      inPtr += inIncZ*inPadZ+inIncY*(frameExtentY-inPadY-outY);

      for (i = 0; i < outZ; i++)
        {
        inPtrTmp = inPtr;
        outPtrTmp = outPtr + outIncY*outY;
        for (j = 0; j < outY; j++)
          {
          outPtrTmp -= outIncY;
          if (outX > 0)
            {
            this->UnpackRasterLine(outPtrTmp,inPtrTmp,inPadX,outX);
            }
          inPtrTmp += inIncY;
          }
        outPtr += outIncZ;
        inPtr += inIncZ;
        }
      }
    else
      { // don't apply a vertical flip
      outPtr += outIncZ*outPadZ+outIncY*outPadY+outIncX*outPadX;
      inPtr += inIncZ*inPadZ+inIncY*inPadY;

      for (i = 0; i < outZ; i++)
        {
        inPtrTmp = inPtr;
        outPtrTmp = outPtr;
        for (j = 0; j < outY; j++)
          {
          if (outX > 0) 
            {
            this->UnpackRasterLine(outPtrTmp,inPtrTmp,inPadX,outX);
            }
          outPtrTmp += outIncY;
          inPtrTmp += inIncY;
          }
        outPtr += outIncZ;
        inPtr += inIncZ;
        }
      }
    // restore the output extent once the first frame is done
    outputExtent[4] = saveOutputExtent4;
    }

  this->FrameBufferMutex->Unlock();

  return 1;
}

//----------------------------------------------------------------------------
void vtkVideoSourceSimulator::SetOutputFormat(int format)
{
  if (format == this->OutputFormat)
    {
    return;
    }

  this->OutputFormat = format;

  // convert color format to number of scalar components
  int numComponents;

  switch (this->OutputFormat)
    {
    case VTK_RGBA:
      numComponents = 4;
      break;
    case VTK_RGB:
      numComponents = 3;
      break;
    case VTK_LUMINANCE:
      numComponents = 1;
      break;
    default:
      numComponents = 0;
      vtkErrorMacro(<< "SetOutputFormat: Unrecognized color format.");
      break;
    }
  this->NumberOfScalarComponents = numComponents;

  if (this->FrameBufferBitsPerPixel != numComponents*8)
    {
    this->FrameBufferMutex->Lock();
    this->FrameBufferBitsPerPixel = numComponents*8;
    if (this->Initialized)
      {
    this->UpdateFrameBuffer();    
      this->DoFormatSetup();
    }
    this->FrameBufferMutex->Unlock();
    }

  this->Modified();
}


// check the current video format and set up the VTK video framebuffer to match
void vtkVideoSourceSimulator::DoFormatSetup()
{

    this->FrameBufferBitsPerPixel = BITS_PER_PIXEL;
    this->FrameSize[0] = SLICE_Y_LENGTH;
    this->FrameSize[1] = SLICE_X_LENGTH;      
    this->OutputFormat = VTK_LUMINANCE;
    this->NumberOfScalarComponents = 1;
    this->Modified();
    this->UpdateFrameBuffer();

}

#ifdef NEW_SIMULATOR
void vtkVideoSourceSimulator::InternalGrab()
{
  //to do
  // 1) Do frame buffer indices maintenance
  // 2) Do time stamping
  // 3) decode the data according to type
  // 4) copy data to the local vtk frame buffer
    
  
  // get the pointer to data
  // use the information about data type and frmnum to do cross checking that you are maintaining correct frame index, & receiving
  // expected data type


  // get a thread lock on the frame buffer
  this->FrameBufferMutex->Lock();
  
    // 1) Do the frame buffer indices maintenance
    if (this->AutoAdvance)
      {
      this->AdvanceFrameBuffer(1);
      if (this->FrameIndex + 1 < this->FrameBufferSize)
        {
        this->FrameIndex++;
        }
      }
    int index = this->FrameBufferIndex;

    // 2) Do the time stamping
    this->FrameBufferTimeStamps[index] = vtkTimerLog::GetUniversalTime();

    if (this->FrameCount++ == 0)
      {
      this->StartTimeStamp = this->FrameBufferTimeStamps[index];
      }

  
    // 3) read the data, based on the data type and clip region information, which is reflected in frame buffer extents
    // this is necessary as there would be cases when there is a clip region defined i.e. only data from the desired extents should be copied 
    // to the local buffer, which demands necessary advancement of deviceDataPtr
  
    // get the pointer to actual incoming data on to a local pointer
    char* deviceDataPtr = new char[SLICE_X_LENGTH * SLICE_Y_LENGTH];  
    
    for (int j = 0 ; j < SLICE_Y_LENGTH ; j++)
      {
      for (int k = 0 ; k < SLICE_X_LENGTH ; k++)
        {
        deviceDataPtr[j * SLICE_Y_LENGTH + k] = k;        
         }            
      }

    // get the pointer to the correct location in the frame buffer, where this data needs to be copied
    unsigned char *frameBufferPtr = (unsigned char *)((reinterpret_cast<vtkUnsignedCharArray*>(this->FrameBuffer[index]))->GetPointer(0));

   
    int outBytesPerRow = ((this->FrameBufferExtent[1]- this->FrameBufferExtent[0]+1)* this->FrameBufferBitsPerPixel + 7)/8;
    outBytesPerRow += outBytesPerRow % this->FrameBufferRowAlignment;

    int inBytesPerRow = this->FrameSize[0] * this->FrameBufferBitsPerPixel/8;
  
    int rows = this->FrameBufferExtent[3]-this->FrameBufferExtent[2]+1;


    // 4) copy data to the local vtk frame buffer
    if (outBytesPerRow == inBytesPerRow)
      {
      memcpy(frameBufferPtr,deviceDataPtr,inBytesPerRow*rows);
      }
    else
      {
      while (--rows >= 0)
        {
        memcpy(frameBufferPtr,deviceDataPtr,outBytesPerRow);
        frameBufferPtr += outBytesPerRow;
        deviceDataPtr += (unsigned char) inBytesPerRow;
        }
      }
 
    this->Modified();
    
    delete deviceDataPtr;
  
  this->FrameBufferMutex->Unlock();

}
#else //NEW_SIMULATOR
void vtkVideoSourceSimulator::FillFrameBuffer()
{
  //to do
  // 1) Do frame buffer indices maintenance
  // 2) Do time stamping
  // 3) decode the data according to type
  // 4) copy data to the local vtk frame buffer
    
  
  // get the pointer to data
  // use the information about data type and frmnum to do cross checking that you are maintaining correct frame index, & receiving
  // expected data type


  // get a thread lock on the frame buffer
  this->FrameBufferMutex->Lock();
  
  int limit = 100;
  
  //for testing
//  limit = 3;
 
  for (int n = 0; n < limit; ++n)
 
    {
    // 1) Do the frame buffer indices maintenance
    if (this->AutoAdvance)
      {
      this->AdvanceFrameBuffer(1);
      if (this->FrameIndex + 1 < this->FrameBufferSize)
        {
        this->FrameIndex++;
        }
      }
    int index = this->FrameBufferIndex;

    // 2) Do the time stamping
//    this->FrameBufferTimeStamps[index] = vtkTimerLog::GetUniversalTime();
    //We need times that correspond to the tracking matrices, therefore the every Frame has its index as timestamp
    this->FrameBufferTimeStamps[index] = index;

    if (this->FrameCount++ == 0)
      {
      this->StartTimeStamp = this->FrameBufferTimeStamps[index];
      }

  
    // 3) read the data, based on the data type and clip region information, which is reflected in frame buffer extents
    // this is necessary as there would be cases when there is a clip region defined i.e. only data from the desired extents should be copied 
    // to the local buffer, which demands necessary advancement of deviceDataPtr
  
    // get the pointer to actual incoming data on to a local pointer
    char* deviceDataPtr = new char[SLICE_X_LENGTH * SLICE_Y_LENGTH * SLICE_Z_LENGTH];  
  
    int R = 50; //Radius of globe
    double r; //Radius of circle in slice
    int c = limit / 2; //Center of globe
    double a;// Start to write at this position in the actual line
    double b;// Stop to write at this position in the actual line
  
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

    // get the pointer to the correct location in the frame buffer, where this data needs to be copied
    unsigned char *frameBufferPtr = (unsigned char *)((reinterpret_cast<vtkUnsignedCharArray*>(this->FrameBuffer[index]))->GetPointer(0));

   
    int outBytesPerRow = ((this->FrameBufferExtent[1]- this->FrameBufferExtent[0]+1)* this->FrameBufferBitsPerPixel + 7)/8;
    outBytesPerRow += outBytesPerRow % this->FrameBufferRowAlignment;

    int inBytesPerRow = this->FrameSize[0] * this->FrameBufferBitsPerPixel/8;
  
    int rows = this->FrameBufferExtent[3]-this->FrameBufferExtent[2]+1;


    // 4) copy data to the local vtk frame buffer
    if (outBytesPerRow == inBytesPerRow)
      {
      memcpy(frameBufferPtr,deviceDataPtr,inBytesPerRow*rows);
      }
    else
      {
      while (--rows >= 0)
        {
        memcpy(frameBufferPtr,deviceDataPtr,outBytesPerRow);
        frameBufferPtr += outBytesPerRow;
        deviceDataPtr += (unsigned char) inBytesPerRow;
        }
      }
 
    this->Modified();
  
  }
  
  delete deviceDataPtr;

  this->FrameBufferMutex->Unlock();

}
#endif //NEW_SIMULATOR
