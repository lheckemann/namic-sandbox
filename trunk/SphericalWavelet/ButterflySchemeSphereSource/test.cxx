// basic test file //
#include <iostream>

//
// ITK Headers
// 
#include "itkMesh.h"
#include "itkLineCell.h"
#include "itkTriangleCell.h"
#include "itkButterflySchemeSphereSource.h"
#include "itkButterFlyStructure.h"

int main( int argc, char * argv [] )
{

   if( argc < 2 )
    {
      std::cerr << "Missing arguments" << std::endl;
      std::cerr << "Usage: vtkPolyDataToITKMesh   vtkPolyDataInputFile" << std::endl;
      return -1;
    }

      typedef int Type;
   const unsigned int Dimension = 3;

 typedef itk::ButterflySchemeSphereSource<Type, Dimension> SphereSource;
 SphereSource::Pointer myButterflySchemeSphereSource= SphereSource::New();
 

 itk::ButterflySchemeSphereSource<int, 3>::MeshType::PointType center;
 center[0]=0;
 center[1]=0;
 center[2]=0;
 myButterflySchemeSphereSource->SetCenter(center);
 itk::ButterflySchemeSphereSource<int, 3>::MeshType::PointType::VectorType scale;
 scale=1;
 myButterflySchemeSphereSource->SetScale(scale);
 int subdiv=atof(argv[1]);
 myButterflySchemeSphereSource->SetSubdivisionLevel(subdiv);
 //myButterflySchemeSphereSource->GenerateData();
 

 return 0;
}
