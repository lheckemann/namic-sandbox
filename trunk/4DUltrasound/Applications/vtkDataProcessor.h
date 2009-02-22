/*=========================================================================

Module:  $RCSfile: vtkDataProcessor.h,v $
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School

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

// .NAME vtkDataProcessor - manage the vtk pipeline of SynchroGrab.
// .SECTION Description
// vtkDataProcessor is responsible for the creation and configuration
// of the pipeline used by the application called SynchroGrab.
// Once, the pipeline is properly configured, it also manage the data collection.
// .SECTION Usage
// See SynchroGrab.cxx

#ifndef VTKDATAPROCESSOR_H_
#define VTKDATAPROCESSOR_H_

#include "SynchroGrabConfigure.h"

#include <queue>
#include <map>

//#include "vtkObject.h"
//#include "vtkImageData.h"

class vtkMatrix4x4;
class vtk3DPanoramicVolumeReconstructor;
class vtkDataSender;
class vtkUltrasoundCalibFileReader;
class vtkMutexLock;

struct DataStruct 
{
  vtkImageData* Frame;
  vtkMatrix4x4* Matrix;
  double TimeStamp;
  vtkFloatingPointType Origin[3];
  int VolumeExtent[6];
  double Spacing[3];
};


class vtkDataProcessor : public vtkObject
{
public:
  static vtkDataProcessor *New();
  vtkTypeRevisionMacro(vtkDataProcessor, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(Verbose, bool);
  vtkGetMacro(Verbose, bool);

  vtkSetMacro(UltraSoundTrackingEnabled, bool);
  vtkGetMacro(UltraSoundTrackingEnabled, bool);
  
  vtkSetStringMacro(CalibrationFileName);
  vtkGetStringMacro(CalibrationFileName);

  vtkSetMacro(ProcessPeriod, float);
  vtkGetMacro(ProcessPeriod, float);

  vtkSetMacro(StartUpTime, double);
  vtkGetMacro(StartUpTime, double);
  
  vtkGetMacro(DataSender, vtkDataSender*);
  
  vtkGetMacro(Processing, bool);
  
  vtkSetMacro(MaximumVolumeSize, double);
  double GetMaximumVolumeSize();

  void SetLogStream(ofstream &LogStream);
  ofstream& GetLogStream();
  
  int GetBufferSize(){return dataBufferIndexQueue.size();}
  
  vtkGetMacro(Reconstructor, vtk3DPanoramicVolumeReconstructor *);
  
  vtkSetMacro(DelayFactor, double);
  vtkGetMacro(DelayFactor, double);

  int NewData(struct DataStruct dataStruct);
  int EnableVolumeReconstruction(bool flag);
  int StartProcessing(vtkDataSender * sender);
  int StopProcessing();
  bool IsDataBufferFull();
  bool IsDataBufferEmpty();
  int DeleteData(int index);
  int GetHeadOfDataBuffer();
  int CheckandUpdateVolume(int index, int dataSenderIndex);
  int ReconstructVolume(int index);
  int ForwardData(vtkImageData * image);
  double GetUpTime();
  void ResetOldVolume(int dataSenderIndex);
  void DuplicateImage(vtkImageData * original, vtkImageData * duplicate);
  bool IsDataExpired(int index);

protected:
  vtkDataProcessor();
  ~vtkDataProcessor();

  bool Verbose;
  bool VolumeReconstructionEnabled;
  bool Processing;
  float ProcessPeriod;
  double clipRectangle[4];
  double StartUpTime;
  ofstream LogStream;
  vtkImageData* oldVolume;
  bool UltraSoundTrackingEnabled;
  double MaximumVolumeSize;
  double DelayFactor;

  std::queue<int> dataBufferIndexQueue; //Stores index of incoming objects
  int dataBufferSize; //Maximum amount of items that can be stored at the same time
  int dataBufferIndex; //Object which is currently/ was last processed

  std::map<int, struct DataStruct> dataBuffer;

  vtkDataSender* DataSender;

  int AddNewDataToBuffer(int index, struct DataStruct dataStruct);

  char *CalibrationFileName;
  vtkUltrasoundCalibFileReader *calibReader;

  vtk3DPanoramicVolumeReconstructor * Reconstructor;
  int ReconstructorLifeTime;

  //Multithreader to run a thread of collecting and sending data
  vtkMultiThreader *PlayerThreader;
  int PlayerThreadId;
  vtkMutexLock *DataBufferLock;
  vtkMutexLock *DataBufferIndexQueueLock;

  void GetVolumeMatrix(vtkMatrix4x4* matrix);
  int  MergeVolumes(vtkImageData* newVolume,
                   vtkFloatingPointType* originNewVolume,
                   int* extentNewVolume,
                   vtkImageData* oldVolume,
                   vtkFloatingPointType* originOldVolume,
                   int* extentOldVolume,
                   int ScalarComponents);
  int IncrementBufferIndex(int increment);
  bool IsIndexAvailable(int index);
  void DecreaseLifeTimeOfReconstructor(int decrease);

private:

  vtkDataProcessor(const vtkDataProcessor&);  // Not implemented.
  void operator=(const vtkDataProcessor&);  // Not implemented.
};

#endif /*VTKDATAPROCESSOR_H_*/
