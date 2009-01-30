#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"

#include "VNLIterativeSparseSolverTraits.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"

#include "itkQuadEdgeMeshGaussMapFilter.h"
#include "itkQuadEdgeMeshRayTracingFilter.h"
#include "itkQuadEdgeMeshToSphereFilter.h"
#include "itkQuadEdgeMeshSphericalParameterization.h"

#include <time.h>

int main( int argc, char** argv )
{
  if( argc != 4 )
    {
    std::cout <<"It requires 1 arguments" <<std::endl;
    std::cout <<"1-Input FileName" <<std::endl;
    std::cout <<"2-Type of initialization" <<std::endl;
    std::cout <<"  * 0: Gauss Map" <<std::endl;
    std::cout <<"  * 1: Ray Casting" <<std::endl;
    std::cout <<"  * 2: MeshToSphere" <<std::endl;
    std::cout <<"3-Output FileName" <<std::endl;
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

  MeshPointer output;

  clock_t start = clock();
  switch( atoi( argv[2] ) )
  {
    case 0:
      {
      typedef itk::QuadEdgeMeshGaussMapFilter< MeshType, MeshType > FilterType;
      typedef itk::QuadEdgeMeshSphericalParameterization<
        MeshType, MeshType, FilterType > SphericalParameterizationType;

      SphericalParameterizationType::Pointer filter =
        SphericalParameterizationType::New();
      filter->SetInput( mesh );
      filter->SetCoefficientsMethod( &coeff0 );
      filter->Update();
      output = filter->GetOutput();
      break;
      }
    case 1:
      {
      typedef itk::QuadEdgeMeshRayTracingFilter< MeshType, MeshType > FilterType;
      typedef itk::QuadEdgeMeshSphericalParameterization<
        MeshType, MeshType, FilterType > SphericalParameterizationType;

      SphericalParameterizationType::Pointer filter =
        SphericalParameterizationType::New();
      filter->SetInput( mesh );
      filter->SetCoefficientsMethod( &coeff0 );
      filter->Update();

      output = filter->GetOutput();
      break;
      }
    case 2:
      {
      typedef itk::QuadEdgeMeshToSphereFilter< MeshType, MeshType, SolverTraits >
        FilterType;
      typedef itk::QuadEdgeMeshSphericalParameterization<
        MeshType, MeshType, FilterType > SphericalParameterizationType;

      SphericalParameterizationType::Pointer filter =
        SphericalParameterizationType::New();
      filter->SetInput( mesh );
      filter->SetCoefficientsMethod( &coeff0 );
      filter->Update();
      output = filter->GetOutput();
      break;
      }
    default:
      return EXIT_FAILURE;
  }
  clock_t end = clock();

  std::cout <<"Time: "
    <<static_cast<double>( end - start ) /
      static_cast<double>(CLOCKS_PER_SEC) <<" s" <<std::endl;

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( output );
  writer->SetFileName( argv[3] );
  writer->Update();

  return EXIT_SUCCESS;
}
