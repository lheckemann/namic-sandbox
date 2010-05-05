#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"

#include "itkQuadEdgeMeshSplitFilter.h"


int main( int argc, char** argv )
{
  if( argc != 4 )
    {
    std::cout <<"It requires 3 arguments" <<std::endl;
    std::cout <<"1-Input FileName" <<std::endl;
    std::cout <<"split0 FileName; split1 FileName" <<std::endl;
    return EXIT_FAILURE;
    }
    // ** TYPEDEF **
  typedef double Coord;

  typedef itk::QuadEdgeMesh< Coord, 3 >               MeshType;
  typedef MeshType::Pointer                           MeshPointer;
  typedef MeshType::CellIdentifier                    CellIdentifier;

  typedef itk::VTKPolyDataReader< MeshType >          ReaderType;
  typedef itk::VTKPolyDataWriter< MeshType >          WriterType;
  
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

  //set the seeds to be the first and last cell
  typedef MeshType::CellsContainerConstPointer CellsContainerConstPointer;
  CellsContainerConstPointer cells = mesh->GetCells();

  typedef MeshType::CellsContainerConstIterator CellsContainerConstIterator;
  CellsContainerConstIterator c_It = cells->begin();

  std::vector <CellIdentifier> seedList;
  seedList.push_back(c_It.Index());
  c_It = cells->end();
  --c_It;

  seedList.push_back(c_It.Index());
  
  typedef itk::QuadEdgeMeshSplitFilter< MeshType, MeshType > SplitFilterType;
  SplitFilterType::Pointer split = SplitFilterType::New();
  split->SetInput( mesh );
  split->SetSeedFaces(seedList);
  split->Update();
  
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( split->GetOutput( 0 ) );
  writer->SetFileName( argv[2] );
  writer->Update();
  
  writer->SetInput( split->GetOutput( 1 ) );
  writer->SetFileName( argv[3] );
  writer->Update();

  return EXIT_SUCCESS;
}
