#include "itkOrientedImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkCastImageFilter.h"

int main(int argc, char** argv){
  if(argc<4){
    std::cerr << "Usage: " << argv[0] << " variance input_image output_image" << std::endl;
    return -1;
  }

  typedef itk::OrientedImage< float,3> ImageType; 
  typedef itk::OrientedImage< short,3> OutputImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typedef itk::ImageFileWriter<OutputImageType> WriterType;
  typedef itk::CastImageFilter<ImageType,OutputImageType> CastType;
  typedef itk::DiscreteGaussianImageFilter<ImageType,ImageType> GaussianType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  CastType::Pointer caster = CastType::New();
  GaussianType::Pointer gaussian = GaussianType::New();

  float var = atof(argv[1]);
  reader->SetFileName(argv[2]);
  writer->SetFileName(argv[3]);

  reader->Update();

  ImageType::Pointer input = reader->GetOutput();

  ImageType::SpacingType spacing = input->GetSpacing();
  GaussianType::ArrayType varArray;
  varArray[0] = spacing[0]*var;
  varArray[1] = spacing[1]*var;
  varArray[2] = spacing[2]*var;

  gaussian->SetInput(input);
  gaussian->SetUseImageSpacing(1);
  gaussian->SetVariance(varArray);

  caster->SetInput(gaussian->GetOutput());
  writer->SetInput(caster->GetOutput());
  writer->SetUseCompression(1);

  writer->Update();

  return 0;
}
