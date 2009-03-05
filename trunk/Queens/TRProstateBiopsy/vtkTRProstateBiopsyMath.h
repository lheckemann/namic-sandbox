/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/
// .NAME vtkTRProstateBiopsyMath - performs common math operations
// .SECTION Description
// vtkTRProstateBiopsyMath provides mathematical operations that are
// relevant to the TRProstateBiopsy module in Slicer.

#ifndef __vtkTRProstateBiopsyMath_h
#define __vtkTRProstateBiopsyMath_h

#include "vtkObject.h"
#include "vtkTRProstateBiopsyWin32Header.h"
#include "itkPoint.h"

class vtkMatrix4x4;

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyMath :
  public vtkObject
{
public:
  static vtkTRProstateBiopsyMath *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyMath,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  static void ComputePermutationFromOrientation(vtkMatrix4x4 *matrix,
                                                int permutation[3],
                                                int flip[3]);

  // Description
  // Calculate distance between a line (defined by two points) and a point
  static double ComputeDistanceLinePoint(double LinePoint1[3], double LinePoint2[3], double TargetPoint[3]);

  //BTX
  static double VMagnitudeM( itk::Point<double,3> Point1, double Point2[3] );  
  static double PointDistanceFromLineM(itk::Point<double,3>, double L[3], double v[3]);
  //ETX

protected:
  vtkTRProstateBiopsyMath() {};
  ~vtkTRProstateBiopsyMath() {};

private:
  vtkTRProstateBiopsyMath(const vtkTRProstateBiopsyMath&);  // Not implemented.
  void operator=(const vtkTRProstateBiopsyMath&);  // Not implemented.
};

#endif
