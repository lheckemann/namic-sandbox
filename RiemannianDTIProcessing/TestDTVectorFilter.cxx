#include "itkSymmetricSecondRankTensor.h"
#include "itkDiffusionTensorToEuclideanVectorImageFilter.h"
#include "itkEuclideanVectorToDiffusionTensorImageFilter.h"
#include "itkImageFileReader.h"

typedef itk::SymmetricSecondRankTensor<double,3> TensorPixelType;
typedef itk::Image<TensorPixelType,3> TensorImageType;
typedef itk::Vector<double,6> VectorPixelType;
typedef itk::Image<VectorPixelType,3> VectorImageType;
typedef itk::DiffusionTensorToEuclideanVectorImageFilter<TensorImageType, VectorImageType> WeightFilterType;
typedef itk::EuclideanVectorToDiffusionTensorImageFilter<VectorImageType, TensorImageType> InverseFilterType;
typedef itk::ImageFileReader<TensorImageType> TensorImageReaderType;

int main(int argc, char ** argv)
{
  TensorImageReaderType::Pointer imageReader = TensorImageReaderType::New();
  imageReader->SetFileName(argv[1]);
  imageReader->Update();
  WeightFilterType::Pointer filter = WeightFilterType::New();
  InverseFilterType::Pointer invFilter = InverseFilterType::New();
  filter->SetInput(imageReader->GetOutput());
  invFilter->SetInput(filter->GetOutput());
  invFilter->Update();
  return 0;
}
