/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageWarpForce.h,v $
  Date:      $Date: 2005/12/20 22:54:52 $
  Version:   $Revision: 1.1.1.1.8.1 $

=========================================================================auto=*/
// .NAME vtkImageWarpForce - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkImageWarpForce_h
#define __vtkImageWarpForce_h

#include <vtkAGConfigure.h>

#include <vtkImageMultipleInputFilter.h>

class VTK_AG_EXPORT vtkImageWarpForce : public vtkImageMultipleInputFilter
{
public:
  static vtkImageWarpForce* New();
  vtkTypeMacro(vtkImageWarpForce,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void SetTarget(vtkImageData *input)
  {
    this->SetInput(0, input);
  }
  vtkImageData* GetTarget();
  
  virtual void SetSource(vtkImageData *input)
  {
    this->SetInput(1, input);
  }
  vtkImageData* GetSource();
  
  virtual void SetDisplacement(vtkImageData *input)
  {
    this->SetInput(2, input);
  }
  vtkImageData* GetDisplacement();

  virtual void SetMask(vtkImageData *input)
  {
    this->SetInput(3, input);
  }
  vtkImageData* GetMask();

protected:
  vtkImageWarpForce();
  ~vtkImageWarpForce();
  vtkImageWarpForce(const vtkImageWarpForce&);
  void operator=(const vtkImageWarpForce&);
  void ExecuteInformation(vtkImageData **inDatas, vtkImageData *outData);
};
#endif
