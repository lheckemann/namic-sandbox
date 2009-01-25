#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"

#include "itkQuadEdgeMeshSplitFilter.h"


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

  typedef itk::QuadEdgeMesh< Coord, 3 >               MeshType;
  typedef MeshType::Pointer                           MeshPointer;
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
  
  typedef itk::QuadEdgeMeshSplitFilter< MeshType, MeshType > SplitFilterType;
  SplitFilterType::Pointer split = SplitFilterType::New();
  split->SetInput( mesh );
  split->Update();
  
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( split->GetOutput( 0 ) );
  writer->SetFileName( "split0.vtk" );
  writer->Update();
  
  writer->SetInput( split->GetOutput( 1 ) );
  writer->SetFileName( "split1.vtk" );
  writer->Update();

  return EXIT_SUCCESS;
}
