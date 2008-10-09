/*=========================================================================

Module:    $RCSfile: vtkTaggedImageFilter.cxx,v $
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

#include "vtkTaggedImageFilter.h"

#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include "vtkVideoSource.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkTransform.h"

vtkCxxRevisionMacro(vtkTaggedImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkTaggedImageFilter);

//----------------------------------------------------------------------------
vtkTaggedImageFilter::vtkTaggedImageFilter()
{
  this->Initialized = false;
 
  this->TemporalLag = 0;
 
  this->ToolMatrix = vtkMatrix4x4::New();
  this->ToolTransform = vtkTransform::New();

  this->VideoSource = NULL;
  this->TrackerTool = NULL;

}

//----------------------------------------------------------------------------
vtkTaggedImageFilter::~vtkTaggedImageFilter()
{
  this->ToolMatrix->Delete();
  this->ToolTransform->Delete();
  this->SetTrackerTool(NULL);
}

//----------------------------------------------------------------------------
void vtkTaggedImageFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Temporal lag : "
     << this->TemporalLag << endl;

  os << indent << "Calibration : (" 
      << this->TrackerTool->GetCalibrationMatrix() << endl; 
}


//-----------------------------------------------------------------------------
void vtkTaggedImageFilter::SetCalibrationMatrix(vtkMatrix4x4 *vmat)
{
  //set the tool calib matrix of tracker tool
  if(this->TrackerTool != NULL)
    this->TrackerTool->SetCalibrationMatrix(vmat);
}

//------------------------------------------------------------------------------
vtkMatrix4x4 *vtkTaggedImageFilter::GetCalibrationMatrix()
{
  return this->TrackerTool->GetCalibrationMatrix();
}

//-------------------------------------------------------------------
void vtkTaggedImageFilter::Initialize()
{
 
  if (!VideoSource)
    {
  // video source not set
  vtkErrorMacro("Video source not set");
  return;
  }

  if (!TrackerTool)
    {
  // tracker tool not set
  vtkErrorMacro("Tracker tool not set");
  return;
  }

  //set the tool calib matrix of tracker tool
  //this->TrackerTool->SetCalibrationMatrix(ToolCalibMatrix);
  this->Initialized = true;
}

// set the video source
void vtkTaggedImageFilter::SetVideoSource(vtkVideoSource *source)
{
  this->VideoSource = source;
  this->SetInput(this->VideoSource->GetOutput());
}

// set the tracker tool
void vtkTaggedImageFilter::SetTrackerTool(vtkTrackerTool *tool)
{
  this->TrackerTool = tool; 
}



// This method simply copies by reference the input data to the output.
int vtkTaggedImageFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  if(VideoSource != NULL)
    {
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkImageData *inData = vtkImageData::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkImageData *outData = vtkImageData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
    int extent[6];
    inData->GetExtent(extent);
    outData->SetExtent(extent);
    outData->GetPointData()->PassData(inData->GetPointData());
    }
  else
    {
    vtkErrorMacro("VideoSource == NULL, member variable not initialized");
    return 0;
    }

  // get the correct corresponding measurement
  // steps: 
  //  1) get the frame time stamp of the image from video source
  //  2) add lag to it, to get new time
  //  3) get the interpolated transform at new time from vtkTrackerTool's buffer
  //  4) get the transform from the matrix to set the output transform

  if(this->TrackerTool != NULL)
    {
    // 1) get the frame time stamp
    double frmAcqTime = this->VideoSource->GetFrameTimeStamp(0);

    // 2) new time
    double transformTime = frmAcqTime + this->TemporalLag;

    // 3) get the interpolated matrix
    this->TrackerTool->GetBuffer()->GetFlagsAndMatrixFromTime(this->ToolMatrix, transformTime);

    // 4) Get the transform from the matrix to set the output transform
    this->ToolTransform->SetMatrix(this->ToolMatrix);
    }
  else
    this->ToolTransform->Identity();
  
  
  return 1;
}

