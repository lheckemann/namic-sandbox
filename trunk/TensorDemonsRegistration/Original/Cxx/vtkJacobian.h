/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkJacobian.h,v $
  Date:      $Date: 2005/12/20 22:54:53 $
  Version:   $Revision: 1.2.8.1 $

=========================================================================auto=*/
// .NAME vtkJacobian - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkJacobian_h
#define __vtkJacobian_h

#include <vtkAGConfigure.h>

#include <stdio.h>
#include <vtkImageToImageFilter.h>

class VTK_AG_EXPORT vtkJacobian : public vtkImageToImageFilter {
public:
  static vtkJacobian* New();
  vtkTypeMacro(vtkJacobian,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkJacobian();
  ~vtkJacobian();
  vtkJacobian(const vtkJacobian&) {}
  void operator=(const vtkJacobian&) {}
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
               int extent[6], int id);

  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
};
#endif


