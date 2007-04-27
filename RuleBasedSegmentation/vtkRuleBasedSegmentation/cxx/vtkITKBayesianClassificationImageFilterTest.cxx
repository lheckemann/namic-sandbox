#include "vtkITKBayesianClassificationImageFilterTest.h"

int main( int argc, char * argv [] )
{
  if( argc < 2 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: InputFile  OutputFile" << std::endl;
    return -1;
    }

  vtkPNGReader * reader = vtkPNGReader::New();

  reader->SetFileName( argv[1] );
  reader->Update();
  
  vtkITKBayesianClassificationImageFilter * filter =
    vtkITKBayesianClassificationImageFilter::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetNumberOfClasses( 2 );
  filter->SetNumberOfSmoothingIterations( 2 );
  

  // write tube to file
  vtkPNGWriter *writer = vtkPNGWriter::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName(argv[2]);
  writer->Write();

  return 0;
}
