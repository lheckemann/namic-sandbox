/*=========================================================================

Module:    $RCSfile: vtkTaggedImageFilter.h,v $
Author:  Siddharth Vikal, Queens School Of Computing

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

// .NAME vtkTaggedImageFilter - combine US image measurements with Certus measurements.
// .SECTION Description
// vtkTaggedImageFilter  receives US images with their acquisition time stamps from vtkSonixVideoSource;
// It requests vtkOptotrakTracker to provide with the Certus measurement at the corresponding times, incorporating
// information from temporal lag, and applying calibration
// the calibration matrix is to be provided by the vtkUltrasoundCalibFileReader class
// This filter does not modify the image itself, it just associated information with it
// This filter will work in both streaming mode, and otherwise
// .SECTION Usage
// tagger->SetVideoSource(vtkVideoSource *)
// tagger->SetTrackerTool(vtkTrackerTool *)
// tagger->SetCalibMatrix(calibFileReader->GetCalibrationMatrix())
// tagger->SetTemporalLag(lag)
// tagger->Initialize()
// imageviewer->SetInput(tagger->GetOutput())

#ifndef __vtkTaggedImageFilter_h
#define __vtkTaggedImageFilter_h

#include "vtkUltrasoundWin32Header.h"
#include "vtkImageAlgorithm.h"

class vtkVideoSource;
class vtkTrackerTool;
class vtkTransform;
class vtkMatrix4x4;

class VTK_ULTRASOUND_EXPORT vtkTaggedImageFilter : public vtkImageAlgorithm
{
public:
  static vtkTaggedImageFilter *New();
  vtkTypeRevisionMacro(vtkTaggedImageFilter, vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the input source as vtkSonixVideoSource  
  void SetVideoSource(vtkVideoSource *);
  vtkGetObjectMacro(VideoSource, vtkVideoSource);
  
  // Description:
  // Set the tracker tool for opto trak to track and report
  void SetTrackerTool (vtkTrackerTool *);
  vtkGetObjectMacro(TrackerTool, vtkTrackerTool);
  

  // Description:
  // Set the temporal lag file name
  vtkSetMacro(TemporalLag, double);
  vtkGetMacro(TemporalLag, double);

  // Description:
  // Set a calibration matrix. Calls the vtkTrackerTool's SetCalibrationMatrix  
  void SetCalibrationMatrix(vtkMatrix4x4* vmat);
  vtkMatrix4x4 *GetCalibrationMatrix();
  // Description:
  // Get the transform output
  vtkTransform *GetTransform(){return ToolTransform;};

  void Initialize();

protected:
  vtkTaggedImageFilter();
  ~vtkTaggedImageFilter();

  vtkVideoSource *VideoSource;
  vtkTrackerTool *TrackerTool;

  double TemporalLag;
  vtkMatrix4x4 *ToolMatrix;

  vtkTransform *ToolTransform;
  bool Initialized;
 
  virtual int RequestData(vtkInformation *,
                          vtkInformationVector **,
                          vtkInformationVector *);
 

  // this function reads the calibration file, temporal file
  bool ReadTemporalLag();
  bool ReadCalibration();
private:
  vtkTaggedImageFilter(const vtkTaggedImageFilter&);  // Not implemented.
  void operator=(const vtkTaggedImageFilter&);  // Not implemented.
};



#endif



