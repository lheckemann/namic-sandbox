#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"

#include "VNLIterativeSparseSolverTraits.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"

#include "itkQuadEdgeMeshToSphereFilter.h"


int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cout <<"It requires 2 arguments" <<std::endl;
    std::cout <<"1-Input FileName" <<std::endl;
    std::cout <<"1-Output FileName" <<std::endl;
    return EXIT_FAILURE;
    }
    // ** TYPEDEF **
  typedef double Coord;

  typedef itk::QuadEdgeMesh< Coord, 3 >                 MeshType;
  typedef MeshType::Pointer                             MeshPointer;
  typedef MeshType::CellIdentifier                      CellIdentifier;

  typedef itk::VTKPolyDataReader< MeshType >            ReaderType;
  typedef itk::VTKPolyDataWriter< MeshType >            WriterType;

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

  //choose the seeds to be the first and last cell
  typedef MeshType::CellsContainerConstPointer CellsContainerConstPointer;
  CellsContainerConstPointer cells = mesh->GetCells();

  typedef MeshType::CellsContainerConstIterator CellsContainerConstIterator;
  CellsContainerConstIterator c_It = cells->begin();

  std::vector <CellIdentifier> seedList;
  seedList.push_back(c_It.Index());
  c_It = cells->end();
  --c_It;

  seedList.push_back(c_It.Index());

  itk::OnesMatrixCoefficients< MeshType >    coeff0;

  typedef VNLIterativeSparseSolverTraits< Coord >  SolverTraits;

  typedef itk::QuadEdgeMeshToSphereFilter<
    MeshType, MeshType, SolverTraits > FilterType;

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( mesh );
  filter->SetCoefficientsMethod( &coeff0 );
  filter->SetSeedFaces(seedList);
  filter->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();

  return EXIT_SUCCESS;
}
