/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageBrainThumbSegmentation.h,v $
  Date:      $Date: 2006/01/06 17:56:40 $
  Version:   $Revision: 1.5 $

=========================================================================auto=*/

// .NAME vtkImageBrainThumbSegmentation - Segment brain "Thumbs" from imagery using the Fast Marching algorithm.
// .SECTION Description
// vtkImageBrainThumbSegmentation - 


#ifndef __vtkImageBrainThumbSegmentation_h
#define __vtkImageBrainThumbSegmentation_h

#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkIdList.h"
#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkPriorityQueue.h"
#include "vtkRuleBasedSegmentationConfigure.h"


class VTK_RULEBASEDSEGMENTATION_EXPORT vtkImageBrainThumbSegmentation : public vtkImageToImageFilter
{
public:
  static vtkImageBrainThumbSegmentation *New();
  vtkTypeMacro(vtkImageBrainThumbSegmentation,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Seed points and input data do not need anything here???

  vtkSetObjectMacro(SeedPointList,vtkIntArray);
  vtkGetObjectMacro(SeedPointList,vtkIntArray);

  vtkSetMacro(numberOfStructures,int);
  vtkGetMacro(numberOfStructures,int);

protected:
  // This contains the seed points inputted by the user
  vtkIntArray *SeedPointList;

  // This is the number of structures
  int numberOfStructures;

  void InitIntSlice( vtkImageData *slice , int value );
  void InitFloatSlice( vtkImageData *slice , float value );
  void ExtractSlice( vtkImageData *inData, vtkImageData *inDataSlice , int iSlice );
  void ComputeCosts( vtkImageData *inDataSlice, vtkImageData *costsSlice );
  void FindNeighbors(vtkIdList *list,int id, vtkDataArray *scalars);

  vtkImageBrainThumbSegmentation();
  ~vtkImageBrainThumbSegmentation();
  vtkImageBrainThumbSegmentation(const vtkImageBrainThumbSegmentation&) {};
  void operator=(const vtkImageBrainThumbSegmentation&) {};
 
  //void Execute(vtkImageData *inData, vtkImageData *outData);
  void ExecuteData(vtkDataObject *);
  void Execute() { this->vtkImageToImageFilter::Execute(); };
  void Execute(vtkImageData *outData) { this->vtkImageToImageFilter::Execute(outData); };

};

#endif
