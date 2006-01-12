/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageBrainThumbSegmentation.cxx,v $
  Date:      $Date: 2006/01/06 17:56:39 $
  Version:   $Revision: 1.10 $

=========================================================================auto=*/
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPriorityQueue.h"
#include "vtkVersion.h"
#include <math.h>
#include <stdlib.h>
#include "vtkImageBrainThumbSegmentation.h"


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


void vtkImageBrainThumbSegmentation::InitIntSlice( vtkImageData *slice , int value )
{
  int *dim = slice->GetDimensions();
  vtkDataArray *scalars = slice->GetPointData()->GetScalars();
  int id = 0;
  
  for (int j = 0; j <dim[1]; j++) {
  for (int i = 0; i <dim[0]; i++) {
  id = (j*dim[0]) + i;
  scalars->SetTuple1(id, value);
  }
  }
}


void vtkImageBrainThumbSegmentation::InitFloatSlice( vtkImageData *slice , float value )
{
  int *dim = slice->GetDimensions();
  vtkDataArray *scalars = slice->GetPointData()->GetScalars();
  int id = 0;
  
  for (int j = 0; j <dim[1]; j++) {
  for (int i = 0; i <dim[0]; i++) {
  id = (j*dim[0]) + i;
  scalars->SetTuple1(id, value);
  }
  }
}


void vtkImageBrainThumbSegmentation::ExtractSlice( vtkImageData *inData, vtkImageData *inDataSlice , int iSlice )
{
  int *dim = inDataSlice->GetDimensions();
  vtkDataArray *inDataScalars = inData->GetPointData()->GetScalars();
  vtkDataArray *inDataSliceScalars = inDataSlice->GetPointData()->GetScalars();
  int inDataId = 0;
  int inDataSliceId = 0;
    
  for (int j = 0; j <dim[1]; j++) {
  for (int i = 0; i <dim[0]; i++) {
  inDataId = iSlice*(dim[1]*dim[0]) + j*dim[0] + i;
  inDataSliceId = j*dim[0] + i;
  inDataSliceScalars->SetTuple1( inDataSliceId, inDataScalars->GetTuple1( inDataId ) );
  }
  }
}


void vtkImageBrainThumbSegmentation::ComputeCosts( vtkImageData *inDataSlice, vtkImageData *costsSlice )
{
  int *dim = inDataSlice->GetDimensions();
  vtkDataArray *inDataSliceScalars = inDataSlice->GetPointData()->GetScalars();
  vtkDataArray *costsSliceScalars = costsSlice->GetPointData()->GetScalars();
  int inDataSliceId = 0;
  int costsSliceId = 0;
  
  for (int j = 0; j <dim[1]; j++) {
  for (int i = 0; i <dim[0]; i++) {
  int id = j*dim[0] + i;
  costsSliceScalars->SetTuple1( id, pow(inDataSliceScalars->GetTuple1( id ), 6 ) );
  }
  }
}


void vtkImageBrainThumbSegmentation::FindNeighbors(vtkIdList *list,int id, vtkDataArray *scalars)
{
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
//  float *inPtr;
//  float *outPtr;
//  inPtr = (float *)inData->GetScalarPointerForExtent(inExt);
//  outPtr = (float *)outData->GetScalarPointerForExtent(outExt);
  
  // Setup Input and Output
  vtkImageData *inData = this->GetInput();

  int inExt[6], outExt[6];
  this->ComputeInputUpdateExtent(inExt, outExt);

  vtkImageData *outData = this->GetOutput();
  outData->SetExtent(outData->GetWholeExtent());
  outData->AllocateScalars();
  outData->GetExtent(outExt);
  
  // Setup general variables
  int *dim = inData->GetDimensions();

  // inDataSlice holds the input on one slice
  vtkImageData *inDataSlice = vtkImageData::New();
  inDataSlice->SetDimensions(dim[1], dim[2], 0);
  inDataSlice->SetScalarTypeToFloat();
  inDataSlice->AllocateScalars();
  InitFloatSlice( inDataSlice , 0.0 );

  // thumbMaskSlice holds the thumb mask on one slice
  vtkImageData *thumbMaskSlice = vtkImageData::New();
  thumbMaskSlice->SetDimensions(dim[1], dim[2], 0);
  thumbMaskSlice->SetScalarTypeToInt();
  thumbMaskSlice->AllocateScalars();

  // costsSlice holds the cost function on one slice
  vtkImageData *costsSlice = vtkImageData::New();
  costsSlice->SetDimensions(dim[1], dim[2], 0);
  costsSlice->SetScalarTypeToFloat();
  costsSlice->AllocateScalars();
  InitFloatSlice( costsSlice , 0.0 );

  // Iterate through each slice
  for( int iSlice = 0; iSlice < dim[2]; iSlice++ )
    {
    // Initialize this slice iteration
    ExtractSlice( inData, inDataSlice, iSlice );
    InitIntSlice( thumbMaskSlice , 0 );
    ComputeCosts( inDataSlice, costsSlice );

    // Iterate through each structure
    for( int iStructure = 0; iStructure < numberOfStructures; iStructure++ )
      {
      }
    }
    
  // Components turned into x, y and z
  if (this->GetInput()->GetNumberOfScalarComponents() > 1)
    {
    vtkErrorMacro("This filter can handle upto 1 components");
    return;
    }
  
  
  if (inData->GetScalarType() != VTK_FLOAT)
    {
    vtkErrorMacro(<< "Error: DataType should be float.");
    return;
    }
}


void vtkImageBrainThumbSegmentation::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);
}
