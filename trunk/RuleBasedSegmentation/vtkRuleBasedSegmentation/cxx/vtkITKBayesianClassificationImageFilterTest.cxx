#include "vtkITKBayesianClassificationImageFilterTest.h"
#include "vtkImageCast.h"
#include "vtkImageMathematics.h"

int main( int argc, char * argv [] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage arguments: InputImage ClassifiedImage numberOfClasses numberOfSmoothingIterations [Optional: MaskImage MaskValue]" << std::endl;
    return -1;
    }

  //
  // read image
  std::cerr << "Reading image...";
  vtkPNGReader * reader = vtkPNGReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();
  std::cerr << "Done." << std::endl;

  //
  // create vtk filter
  // voxel type will be cast to itk type automatically
  vtkITKBayesianClassificationImageFilter * filter =
    vtkITKBayesianClassificationImageFilter::New();

  //
  // set inputs
  // NB: vtkITK abuses the vtk pipeline architecture by overriding
  // SetInput thus, we must use SetInput
  filter->SetInput( reader->GetOutput() );
  filter->SetNumberOfClasses( atoi(argv[3]) );
  filter->SetNumberOfSmoothingIterations( atoi(argv[4]) );

  //
  // set mask
  if (argc > 5)
  {
    std::cerr << "Reading mask image...";
    vtkPNGReader * maskReader = vtkPNGReader::New();
    maskReader->SetFileName( argv[5] );
    maskReader->Update();
    std::cerr << "Done." << std::endl;
    filter->SetMaskImage( maskReader->GetOutput() );
    filter->SetMaskValue( atoi(argv[6]) );
  }

  //
  // execute filter
  std::cerr << "Executing filter...";
  filter->Update();
  std::cerr << "Done." << std::endl;

  //
  // rescale [0,1] to [0,255] and convert to unsigned char
  vtkImageMathematics* intensityRescaler = vtkImageMathematics::New();
  intensityRescaler->SetInput1( filter->GetOutput() );
  intensityRescaler->SetOperationToMultiplyByK();
  intensityRescaler->SetConstantK(255);

  vtkImageCast* toUC = vtkImageCast::New();
  toUC->SetInput( intensityRescaler->GetOutput() );
  toUC->SetOutputScalarTypeToUnsignedChar();

  // write tube to file
  std::cerr << "Writing output...";
  vtkPNGWriter *writer = vtkPNGWriter::New();
  writer->SetInput( toUC->GetOutput() );
  writer->SetFileName(argv[2]);
  writer->Write();
  std::cerr << "Done." << std::endl;

  return 0;
}
