/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageTransformIntensity.h,v $
  Date:      $Date: 2005/12/20 22:54:52 $
  Version:   $Revision: 1.2.8.1 $

=========================================================================auto=*/
// .NAME vtkImageTransformIntensity - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkImageTransformIntensity_h
#define __vtkImageTransformIntensity_h

#include <vtkAGConfigure.h>

#include <stdio.h>
#include <vtkImageToImageFilter.h>
#include <vtkIntensityTransform.h>

class VTK_AG_EXPORT vtkImageTransformIntensity : public vtkImageToImageFilter {
public:
  static vtkImageTransformIntensity* New();
  vtkTypeMacro(vtkImageTransformIntensity,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
      
  vtkSetObjectMacro(IntensityTransform,vtkIntensityTransform);
  vtkGetObjectMacro(IntensityTransform,vtkIntensityTransform);
      
protected:
  vtkImageTransformIntensity();
  ~vtkImageTransformIntensity();
  vtkImageTransformIntensity(const vtkImageTransformIntensity&) {}
  void operator=(const vtkImageTransformIntensity&) {}
  void ThreadedExecute(vtkImageData *inDatas, vtkImageData *outData,
               int extent[6], int id);
      
private:
  vtkIntensityTransform* IntensityTransform;
};
#endif


