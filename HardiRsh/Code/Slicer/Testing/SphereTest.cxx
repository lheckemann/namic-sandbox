#include "vtkSphereSource.h"

#include "vtkImageData.h"
#include "vtkPointData.h"

#include "vtkDelaunay3D.h"
#include "vtkGeometryFilter.h"
#include "vtkPolyDataWriter.h"

#include "vnl/vnl_math.h"

int main( int argc, char *argv[] )
{
  double x[3];
  unsigned int numPts = 300;
  vtkPoints *newPoints;

  newPoints = vtkPoints::New();
  newPoints->Allocate(numPts);

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
    x[0] = vcl_sin(theta) * vcl_cos(phi);
    x[1] = vcl_cos(theta) * vcl_cos(phi);
    x[2] = vcl_sin(phi);
    std::cerr << "Point "<<m<<" : " << x[0] << " " << x[1] << " " << x[2] << std::endl;
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
  std::cerr << "Done delny update\n";
  
  vtkGeometryFilter *geoFilter = vtkGeometryFilter::New();
  geoFilter->SetInputConnection(delny->GetOutputPort());
  geoFilter->SetOutput(polyData);
  geoFilter->Update();

  // Write the surface triangles to file as PolyData
  vtkPolyDataWriter* pWriter = vtkPolyDataWriter::New();
  pWriter->SetFileName("Testout");
  pWriter->SetInput(geoFilter->GetOutput());
  pWriter->Write();

  polyData->Delete();
  newPoints->Delete();

 return 1;
}
