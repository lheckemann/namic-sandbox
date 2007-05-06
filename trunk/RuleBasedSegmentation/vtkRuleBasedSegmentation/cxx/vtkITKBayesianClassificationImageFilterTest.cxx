#include "vtkITKBayesianClassificationImageFilterTest.h"
#include "vtkImageCast.h"
#include "vtkImageMathematics.h"
#include "vtkImageFlip.h"
#include "vtkPNGWriter.h"
#include "vtkPNGReader.h"

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
  vtkPNGReader* reader = vtkPNGReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();
  std::cerr << "Done." << std::endl;

  // flip image
  vtkImageFlip* imageFlip = vtkImageFlip::New();
  imageFlip->SetFilteredAxis(1);
  imageFlip->SetInput(reader->GetOutput());

  //
  // create vtk filter
  // voxel type will be cast to itk type automatically
  vtkITKBayesianClassificationImageFilter * filter =
    vtkITKBayesianClassificationImageFilter::New();

  //
  // set inputs
  // NB: vtkITK abuses the vtk pipeline architecture by overriding
  // SetInput thus, we must use SetInput
  filter->SetInput( imageFlip->GetOutput() );
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

    vtkImageFlip* maskFlip = vtkImageFlip::New();
    maskFlip->SetFilteredAxis(1);
    maskFlip->SetInput(maskReader->GetOutput());

    filter->SetMaskImage( maskFlip->GetOutput() );
    filter->SetMaskValue( atoi(argv[6]) );
  }

  //
  // execute filter
  std::cerr << "Executing filter...";
  filter->Update();
  std::cerr << "Done." << std::endl;

  // flip image
  vtkImageFlip* outFlip = vtkImageFlip::New();
  outFlip->SetFilteredAxis(1);
  outFlip->SetInput(filter->GetOutput());

  //
  // rescale [0,1] to [0,255] and convert to unsigned char
  vtkImageMathematics* intensityRescaler = vtkImageMathematics::New();
  intensityRescaler->SetInput1( outFlip->GetOutput() );
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
