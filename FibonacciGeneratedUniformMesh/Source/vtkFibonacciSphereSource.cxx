/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFibonacciSphereSource.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkFibonacciSphereSource.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <math.h>

vtkCxxRevisionMacro(vtkFibonacciSphereSource, "$Revision: 1.70 $");
vtkStandardNewMacro(vtkFibonacciSphereSource);

//----------------------------------------------------------------------------
// Construct sphere with radius=0.5 and default resolution 8 in both Phi
// and Theta directions. Theta ranges from (0,360) and phi (0,180) degrees.
vtkFibonacciSphereSource::vtkFibonacciSphereSource()
{
  this->Radius = 0.5;
  this->Center[0] = 0.0;
  this->Center[1] = 0.0;
  this->Center[2] = 0.0;

  this->TotalNumberOfPoints = 0;

  this->SetNumberOfInputPorts(0);
}

//----------------------------------------------------------------------------
int vtkFibonacciSphereSource::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  int i;
  vtkPoints *newPoints;
  vtkFloatArray *newNormals;
  double x[3], n[3], norm;

  newPoints = vtkPoints::New();
  newPoints->Allocate( this->TotalNumberOfPoints );
  newNormals = vtkFloatArray::New();
  newNormals->SetNumberOfComponents(3);
  newNormals->Allocate(3*this->TotalNumberOfPoints);
  newNormals->SetName("Normals");
  

  this->UpdateProgress(0.0);

  const double pi  = atan(1.0) * 4.0;
  const double pi2 = 2.0 * pi;
  const double phi = ( 3.0 - sqrt(5.0) ) * pi;

  std::cout << "pi  = " << pi  << std::endl;
  std::cout << "phi = " << phi << std::endl;
  std::cout << "TotalNumberOfPoints = " << this->TotalNumberOfPoints << std::endl;

  double angle = 0.0;

  // Generate point coordinates
  for (i=0; i <= this->TotalNumberOfPoints; i++)
    {
    const float p = (float)(i)/(float)(this->TotalNumberOfPoints);
    const double zc = (1.0-2.0*p);
    const double z2 = zc*zc;
    const double rxy = sqrt(1.0-zc*zc);
    const double xc = rxy*cos(angle);
    const double yc = rxy*sin(angle);

    n[0] = this->Radius * xc;
    n[1] = this->Radius * yc;
    n[2] = this->Radius * zc;

    x[0] = n[0] + this->Center[0];
    x[1] = n[1] + this->Center[1];
    x[2] = n[2] + this->Center[2];
    newPoints->InsertNextPoint(x);

    if ( (norm = vtkMath::Norm(n)) == 0.0 )
       {
       norm = 1.0;
       }
    n[0] /= norm; n[1] /= norm; n[2] /= norm;
    newNormals->InsertNextTuple(n);

    angle += phi;
    if ( angle > pi2 )
      {
      angle -= pi2;
      }

    this->UpdateProgress (p);
    }

  
  // Update ourselves and release memeory
  //
  newPoints->Squeeze();
  output->SetPoints(newPoints);
  newPoints->Delete();

  newNormals->Squeeze();
  output->GetPointData()->SetNormals(newNormals);
  newNormals->Delete();

  return 1;
}

//----------------------------------------------------------------------------
void vtkFibonacciSphereSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Total number of points: " << this->TotalNumberOfPoints << "\n";
  os << indent << "Radius: " << this->Radius << "\n";
  os << indent << "Center: (" << this->Center[0] << ", " 
     << this->Center[1] << ", " << this->Center[2] << ")\n";
}

//----------------------------------------------------------------------------
int vtkFibonacciSphereSource::RequestInformation(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),
               -1);

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_BOUNDING_BOX(),
               this->Center[0] - this->Radius,
               this->Center[0] + this->Radius,
               this->Center[1] - this->Radius,
               this->Center[1] + this->Radius,
               this->Center[2] - this->Radius,
               this->Center[2] + this->Radius);

  return 1;
}
