/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkPolynomialIT.h,v $
  Date:      $Date: 2005/12/20 22:54:54 $
  Version:   $Revision: 1.2.8.1 $

=========================================================================auto=*/
// .NAME vtkPolynomialIT - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkPolynomialIT_h
#define __vtkPolynomialIT_h

#include <vtkAGConfigure.h>

#include <vtkIntensityTransform.h>

class VTK_AG_EXPORT vtkPolynomialIT : public vtkIntensityTransform
{
public:
  static vtkPolynomialIT* New();
  vtkTypeMacro(vtkPolynomialIT,vtkIntensityTransform);
  void PrintSelf(ostream& os, vtkIndent indent);
      
  void SetNumberOfFunctions(int n);
  int FunctionValues(vtkFloatingPointType* x, vtkFloatingPointType* f);

  virtual void SetDegree(int d);
  vtkGetMacro(Degree, int);

  void SetAlpha(int i, int j, float v);
  float GetAlpha(int i, int j);
  
protected:
  vtkPolynomialIT();
  ~vtkPolynomialIT();
  vtkPolynomialIT(const vtkPolynomialIT&);
  void operator=(const vtkPolynomialIT&);

  void DeleteAlphas();
  void BuildAlphas();

  int Degree;
  float** Alphas;
};

#endif


