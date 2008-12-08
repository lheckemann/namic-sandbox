/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFibonacciSphereSource.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkFibonacciSphereSource - create a polygonal sphere centered at the origin
// .SECTION Description
// vtkFibonacciSphereSource creates a sphere (represented by polygons) of specified
// radius centered at the origin. The resolution (polygonal discretization)
// in both the latitude (phi) and longitude (theta) directions can be
// specified. It also is possible to create partial spheres by specifying
// maximum phi and theta angles. By default, the surface tessellation of
// the sphere uses triangles; however you can set LatLongTessellation to
// produce a tessellation using quadrilaterals.
// .SECTION Caveats
// Resolution means the number of latitude or longitude lines for a complete 
// sphere. If you create partial spheres the number of latitude/longitude 
// lines may be off by one. 

#ifndef __vtkFibonacciSphereSource_h
#define __vtkFibonacciSphereSource_h

#include "vtkPolyDataAlgorithm.h"

class VTK_GRAPHICS_EXPORT vtkFibonacciSphereSource : public vtkPolyDataAlgorithm 
{
public:
  vtkTypeRevisionMacro(vtkFibonacciSphereSource,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct sphere with radius=0.5 and default resolution 8 in both Phi
  // and Theta directions. Theta ranges from (0,360) and phi (0,180) degrees.
  static vtkFibonacciSphereSource *New();

  // Description:
  // Set radius of sphere. Default is .5.
  vtkSetClampMacro(Radius,double,0.0,VTK_DOUBLE_MAX);
  vtkGetMacro(Radius,double);

  // Description:
  // Set the center of the sphere. Default is 0,0,0.
  vtkSetVector3Macro(Center,double);
  vtkGetVectorMacro(Center,double,3);

  // Description:
  // Set total number of points to be generated on the surface of the sphere.
  vtkSetMacro(TotalNumberOfPoints,int);
  vtkGetMacro(TotalNumberOfPoints,int);


protected:
  vtkFibonacciSphereSource();
  ~vtkFibonacciSphereSource() {}

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  double Radius;
  double Center[3];
  int TotalNumberOfPoints;

private:
  vtkFibonacciSphereSource(const vtkFibonacciSphereSource&);  // Not implemented.
  void operator=(const vtkFibonacciSphereSource&);  // Not implemented.
};

#endif
