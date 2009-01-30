#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"

#include "VNLIterativeSparseSolverTraits.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"

#include "itkQuadEdgeMeshRayTracingFilter.h"
// #include "itkQuadEdgeMeshToSphereFilter.h"
#include "itkQuadEdgeMeshSphericalParameterization.h"

#include <time.h>

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cout <<"It requires 1 arguments" <<std::endl;
    std::cout <<"1-Input FileName" <<std::endl;
    return EXIT_FAILURE;
    }
    // ** TYPEDEF **
  typedef double Coord;

  typedef itk::QuadEdgeMesh< Coord, 3 >             MeshType;
  typedef MeshType::Pointer                         MeshPointer;
  typedef itk::VTKPolyDataReader< MeshType >        ReaderType;
  typedef itk::VTKPolyDataWriter< MeshType >        WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update( );
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while reading the input file " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  MeshPointer mesh = reader->GetOutput();

  itk::OnesMatrixCoefficients< MeshType >        coeff0;

  typedef VNLIterativeSparseSolverTraits< Coord >  SolverTraits;

//   typedef itk::QuadEdgeMeshToSphereFilter<
//     MeshType, MeshType, SolverTraits > FilterType;
//   typedef itk::QuadEdgeMeshGaussMapFilter< MeshType, MeshType >
    typedef itk::QuadEdgeMeshRayTracingFilter< MeshType, MeshType >
    FilterType;

  typedef itk::QuadEdgeMeshSphericalParameterization<
    MeshType, MeshType, FilterType > SphericalParameterizationType;

  clock_t start = clock();
  SphericalParameterizationType::Pointer filter =
    SphericalParameterizationType::New();
  filter->SetInput( mesh );
  filter->SetCoefficientsMethod( &coeff0 );
  filter->Update();
  clock_t end = clock();

  std::cout <<"Time: "
    <<static_cast<double>( end - start ) /
      static_cast<double>(CLOCKS_PER_SEC) <<" s" <<std::endl;

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput( ) );
  writer->SetFileName( "sphere.vtk" );
  writer->Update();

  return EXIT_SUCCESS;
}
