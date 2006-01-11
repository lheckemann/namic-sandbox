/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageBrainThumbSegmentation.cxx,v $
  Date:      $Date: 2006/01/06 17:56:39 $
  Version:   $Revision: 1.10 $

=========================================================================auto=*/
#include "vtkImageBrainThumbSegmentation.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPriorityQueue.h"
#include "vtkVersion.h"
#include <math.h>
#include <stdlib.h>


//----------------------------------------------------------------------------
vtkImageBrainThumbSegmentation* vtkImageBrainThumbSegmentation::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageBrainThumbSegmentation");
  if(ret)
    {
    return (vtkImageBrainThumbSegmentation*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageBrainThumbSegmentation;
}


//----------------------------------------------------------------------------
// Constructor sets default values
vtkImageBrainThumbSegmentation::vtkImageBrainThumbSegmentation()
{
}


vtkImageBrainThumbSegmentation::~vtkImageBrainThumbSegmentation()
{
}


void vtkImageBrainThumbSegmentation::InitSlice( vtkImageData* slice , unsigned int value )
{
}

void vtkImageBrainThumbSegmentation::FindNeighbors(vtkIdList *list,int id, vtkDataArray *scalars) {
  
  // find i, j, k for that node
  int *dim = GetInput()->GetDimensions();
  int numPts = dim[0] * dim[1] * dim[2];
  
  
  for(int vk = -1; vk<2; vk++) {
  for(int vj = -1; vj<2; vj++) {
  for(int vi = -1; vi<2; vi++) {         
    
  int tmpID = id + (vk * dim[1]*dim[0]) + (vj * dim[0]) + vi;
  // check we are in bounds (for volume faces)     
  if( tmpID >= 0 && tmpID < numPts && tmpID != 0) {
  float mask = scalars->GetTuple1(tmpID);
  // only add neighbor if it is in the graph
  if(mask > 0) {        
  list->InsertUniqueId(tmpID);
  }
  }
  }
  }
  }
}

//----------------------------------------------------------------------------
// This method is passed a input and output Data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the Datas data types.

void vtkImageBrainThumbSegmentation::ExecuteData(vtkDataObject *)
{
  float *inPtr;
  float *outPtr;
  
  vtkImageData *inData = this->GetInput();
  vtkImageData *outData = this->GetOutput();
  outData->SetExtent(outData->GetWholeExtent());
  outData->AllocateScalars();

  int inExt[6], outExt[6];

  outData->GetExtent(outExt);
  this->ComputeInputUpdateExtent(inExt, outExt);
  inPtr = inData->GetScalarPointerForExtent(inExt);
  outPtr = outData->GetScalarPointerForExtent(outExt);
  

  // Components turned into x, y and z
  if (this->GetInput()->GetNumberOfScalarComponents() > 1)
    {
    vtkErrorMacro("This filter can handle upto 1 components");
    return;
    }
  
  
  if (inData->GetScalarType() != VTK_SCALAR_TYPE_FLOAT)
    {
      vtkErrorMacro(<< "Error: DataType should be float.");
      return;
    }
}


void vtkImageBrainThumbSegmentation::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);
}
