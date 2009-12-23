/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSphereSource.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSbiaSphereSource.h"

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
#include "vtkDelaunay3D.h"
#include "vtkGeometryFilter.h"

#include "vtkPolyDataWriter.h"
#include "vnl/vnl_math.h"
#include <math.h>

vtkCxxRevisionMacro(vtkSbiaSphereSource, "$Revision: 1.70 $");
vtkStandardNewMacro(vtkSbiaSphereSource);

//----------------------------------------------------------------------------
// Construct sphere with radius=0.5 and default resolution 8 in both Phi
// and Theta directions. Theta ranges from (0,360) and phi (0,180) degrees.
vtkSbiaSphereSource::vtkSbiaSphereSource(int res)
{
  res = res < 4 ? 4 : res;
  this->Radius = 0.5;
  this->Center[0] = 0.0;
  this->Center[1] = 0.0;
  this->Center[2] = 0.0;

  this->ThetaResolution = res;
  this->PhiResolution = res;
  this->StartTheta = 0.0;
  this->EndTheta = 360.0;
  this->StartPhi = 0.0;
  this->EndPhi = 180.0;
  this->LatLongTessellation = 0;

  this->SetNumberOfInputPorts(0);
}

//----------------------------------------------------------------------------
int vtkSbiaSphereSource::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  double x[3],n[3],radius,norm;
  unsigned int numPts = this->PhiResolution * this->ThetaResolution + 2;
  vtkPoints *newPoints;
  vtkFloatArray *newNormals;
  
  
  //Allocate space
  newPoints = vtkPoints::New();
  newPoints->Allocate(numPts);
  newNormals = vtkFloatArray::New();
  newNormals->SetNumberOfComponents(3);
  newNormals->Allocate(3*numPts);
  newNormals->SetName("Normals");


  /**
   * uses the formula in [saff:dmp:1997] to generate equidistributed points on the sphere.
   * 
   * REFERENCE
   * @Article{saff:dmp:1997,
   * author =          {Saff, E. B. and Kuijlaars, A. B. J.},
   * title =          {Distributing Many Points on a Sphere},
   * journal =          {Math. Intell.},
   * year =          {1997},
   * volume =          {19},
   * number =          {1},
   * pages =          {5--11},
   **/

  double theta, phi, lastPhi=0.0, h;

  for (unsigned int m = 0; m < numPts; m++)
  {
    /*** Grad directions relation to theta phi
    * x = sin(theta) * cos(phi)
    * y = sin(theta) * sin(phi)
    * z = cos(theta)
    */

    h = -1 + 2*( m )/ static_cast<double>( numPts -1 );
    theta = vcl_acos(h);
    
    if (m == 0 or m == numPts -1)
    {
      phi = 0.0;
    }
    else
    {
      phi = vcl_fmod( lastPhi + 3.6 / vcl_sqrt( numPts * (1- vcl_pow(h,2) ) ) 
                                        , ( 2 * vnl_math::pi ) );
    }

    //Add point!
    radius = this->Radius * cos((double)phi);
    n[0] = radius * sin((double)theta);
    n[1] = radius * cos((double)theta);
    n[2] = this->Radius * vcl_sin(phi);
    x[0] = n[0] + this->Center[0];
    x[1] = n[1] + this->Center[1];
    x[2] = n[2] + this->Center[2];

    if ( (norm = vtkMath::Norm(n)) == 0.0 )
      {
      norm = 1.0;
      }
    n[0] /= norm; n[1] /= norm; n[2] /= norm;
    newNormals->InsertNextTuple(n);
    newPoints->InsertNextPoint(x);
    
    lastPhi = phi;
  }

  newPoints->Squeeze();
  vtkPolyData *polyData = vtkPolyData::New();
  polyData->SetPoints(newPoints);
 
  vtkDelaunay3D* delny = vtkDelaunay3D::New();
  delny->SetInput( polyData  );
  delny->SetTolerance(0.001);
  delny->Update();
  
  vtkGeometryFilter *geoFilter = vtkGeometryFilter::New();
  geoFilter->SetInputConnection(delny->GetOutputPort());
  geoFilter->Update();

  vtkPolyData *geoOut = geoFilter->GetOutput();

  newPoints->Squeeze();
  output->SetPoints(newPoints);
  newPoints->Delete();

  newNormals->Squeeze();
  output->GetPointData()->SetNormals(newNormals);
  newNormals->Delete();

  output->SetPolys(geoOut->GetPolys());

  return 1;
}

//----------------------------------------------------------------------------
void vtkSbiaSphereSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Theta Resolution: " << this->ThetaResolution << "\n";
  os << indent << "Phi Resolution: " << this->PhiResolution << "\n";
  os << indent << "Theta Start: " << this->StartTheta << "\n";
  os << indent << "Phi Start: " << this->StartPhi << "\n";
  os << indent << "Theta End: " << this->EndTheta << "\n";
  os << indent << "Phi End: " << this->EndPhi << "\n";
  os << indent << "Radius: " << this->Radius << "\n";
  os << indent << "Center: (" << this->Center[0] << ", " 
     << this->Center[1] << ", " << this->Center[2] << ")\n";
  os << indent
     << "LatLong Tessellation: " << this->LatLongTessellation << "\n";
}

//----------------------------------------------------------------------------
int vtkSbiaSphereSource::RequestInformation(
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
