/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkLTSPolynomialIT.h,v $
  Date:      $Date: 2005/12/20 22:54:53 $
  Version:   $Revision: 1.1.1.1.8.1 $

=========================================================================auto=*/
#ifndef __vtkLTSPolynomialIT_h
#define __vtkLTSPolynomialIT_h

#include <vtkAGConfigure.h>

#include <vtkPolynomialIT.h>

class VTK_AG_EXPORT vtkLTSPolynomialIT : public vtkPolynomialIT
{
public:
  static vtkLTSPolynomialIT* New();
  vtkTypeMacro(vtkLTSPolynomialIT,vtkPolynomialIT);
  void PrintSelf(ostream& os, vtkIndent indent);
      
  vtkSetMacro(Ratio, float);
  vtkGetMacro(Ratio, float);
      
  vtkSetMacro(UseBias, int);
  vtkGetMacro(UseBias, int);
  vtkBooleanMacro(UseBias, int);

protected:
  vtkLTSPolynomialIT();
  ~vtkLTSPolynomialIT();
  vtkLTSPolynomialIT(const vtkLTSPolynomialIT&);
  void operator=(const vtkLTSPolynomialIT&);

  void InternalUpdate();
  
  float Ratio;
  int UseBias;
};

#endif


