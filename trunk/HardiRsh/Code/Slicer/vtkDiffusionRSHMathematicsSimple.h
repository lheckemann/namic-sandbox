/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkDiffusionRSHMathematicsSimple.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkDiffusionRSHMathematicsSimple - Simple example of an image-image filter.
// .SECTION Description
// This is an example of a simple image-image filter. It copies it's input
// to it's output (point by point). It shows how templates can be used
// to support various data types.
// .SECTION See also
// vtkDiffusionRSHMathematicsSimple

#ifndef __vtkDiffusionRSHMathematicsSimple_h
#define __vtkDiffusionRSHMathematicsSimple_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkTeemConfigure.h"
#include "vtkThreadedImageAlgorithm.h"

class vtkMatrix4x4;
class vtkImageData;

class VTK_Teem_EXPORT vtkDiffusionRSHMathematicsSimple : public vtkSimpleImageToImageFilter
{
public:
  static vtkDiffusionRSHMathematicsSimple *New();
  vtkTypeRevisionMacro(vtkDiffusionRSHMathematicsSimple,vtkSimpleImageToImageFilter);

  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the Operation to perform.
  vtkGetMacro(Operation,int);
  vtkSetClampMacro(Operation,int, VTK_RSH_POWER_0, VTK_RSH_POWER_10);


  // Operation options.
  //BTX
  enum
  {
    VTK_RSH_POWER_0,
    VTK_RSH_POWER_2,
    VTK_RSH_POWER_4,
    VTK_RSH_POWER_6,
    VTK_RSH_POWER_8,
    VTK_RSH_POWER_10,
  };
  //ETX


  // Description:
  // Output the trace (sum of eigenvalues = sum along diagonal)
  void SetOperationToPowerSpect(int order)
  {
    switch (order)
    {
      case 0:
        this->SetOperation(VTK_RSH_POWER_0);
        break;
      case 2:
        this->SetOperation(VTK_RSH_POWER_2);
        break;
      case 4:
        this->SetOperation(VTK_RSH_POWER_4);
        break;
      case 6:
        this->SetOperation(VTK_RSH_POWER_6);
        break;
      case 8:
        this->SetOperation(VTK_RSH_POWER_8);
        break;
      case 10:
        this->SetOperation(VTK_RSH_POWER_10);
        break;
      default:
        vtkGenericWarningMacro(<<"Not recognized Order!");
    }
  };

  // Description:
  // Specify scale factor to scale output (float) scalars by.
  // This is not used when the output is RGBA (char color data).
  vtkSetMacro(ScaleFactor,double);
  vtkGetMacro(ScaleFactor,double);

  static double computePowerInOrder0(double *coeffs,int nComps);
  static double computePowerInOrder2(double *coeffs,int nComps);
  static double computePowerInOrder4(double *coeffs,int nComps);
  static double computePowerInOrder6(double *coeffs,int nComps);
  static double computePowerInOrder8(double *coeffs,int nComps);
  static double computePowerInOrder10(double *coeffs,int nComps);

protected:

  vtkDiffusionRSHMathematicsSimple();
  ~vtkDiffusionRSHMathematicsSimple();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output);

  virtual int RequestInformation (
                                  vtkInformation * vtkNotUsed(request),
                                  vtkInformationVector **inputVector,
                                  vtkInformationVector *outputVector);

  int Operation; // math operation to perform
  double ScaleFactor; // Scale factor for output scalars

  int FillInputPortInformation(int port, vtkInformation* info);


private:
  vtkDiffusionRSHMathematicsSimple(const vtkDiffusionRSHMathematicsSimple&);  // Not implemented.
  void operator=(const vtkDiffusionRSHMathematicsSimple&);  // Not implemented.
};

#endif







