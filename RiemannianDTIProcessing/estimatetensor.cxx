#include <list>
#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>

#include <itkImage.h>
#include <itkVector.h>
#include <itkDiffusionTensor3D.h>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <itkDiffusionTensor3DReconstructionImageFilter.h>


typedef double RealType;
const unsigned int DIM = 3;
typedef itk::DiffusionTensor3D<RealType> TensorPixel;
typedef itk::Vector<RealType,6> VectorPixel;
typedef itk::Image<TensorPixel, DIM> TensorImageType;
typedef itk::Image<VectorPixel, DIM> VectorImageType;

typedef itk::DiffusionTensor3DReconstructionImageFilter<unsigned int, unsigned int> DiffusionEstimationFilterType;
typedef DiffusionEstimationFilterType::GradientDirectionType GradientType;

typedef itk::Image<unsigned int, DIM> ImageType;
typedef itk::Image<double, DIM> FloatImageType;
typedef itk::ImageFileReader<ImageType> FileReaderType;
typedef itk::ImageFileWriter<ImageType> FileWriterType;
typedef itk::ImageFileWriter<TensorImageType> TensorFileWriterType;

void print_vnl_vector(const GradientType &a)
{
  a.print(std::cout);
  std::cout << std::endl;
}

int main(int argc, const char* argv[])
{
  if(argc < 2) {
    //std::cerr << "Usage: " << std::endl;
    return -1;
    
  }

  const char** remainingNames = argv + 3;
  const int numNames = argc - 3;
  const char* gradientFileName = argv[1];
  const char* baselineFileName = argv[2];

  std::list<std::string> fileNames;
  std::list<GradientType > gradients;
  
  std::copy(remainingNames,remainingNames+numNames,
            std::insert_iterator<std::list<std::string> >(fileNames,fileNames.begin()));
  
  std::ifstream gradientFile(gradientFileName);
  const int dataSize  = fileNames.size() * 3;
  //  double gradData[dataSize];
  double *gradData = new double[fileNames.size() * 3];

  std::copy(std::istream_iterator<double>(gradientFile),
            std::istream_iterator<double>(),
            gradData);
  
  gradientFile.close();

  for(int i = 0; i < fileNames.size(); ++i) {
    GradientType gradient(gradData[3*i],
                          gradData[3*i+1],
                          gradData[3*i+2]);
    gradients.push_back(gradient);
  }
  

  std::copy(fileNames.begin(),fileNames.end(),
            std::ostream_iterator<std::string>(std::cout,"\n"));
  
  
  std::for_each(gradients.begin(),gradients.end(),
                &print_vnl_vector);

  FileReaderType::Pointer baseFileReader = FileReaderType::New();
  baseFileReader->SetFileName(baselineFileName);

  DiffusionEstimationFilterType::Pointer diffusionEstimator = DiffusionEstimationFilterType::New();
  try{
    baseFileReader->Update();
  } catch (itk::ExceptionObject e) {
    std::cerr << e << std::endl;
    return 0;
  }

  diffusionEstimator->SetReferenceImage(baseFileReader->GetOutput());
  diffusionEstimator->SetThreshold(100);

  std::list<std::string>::const_iterator i;
  std::list<GradientType >::const_iterator j;
  for(i = fileNames.begin(), j = gradients.begin();
      i != fileNames.end(); ++i, ++j) {

    FileReaderType::Pointer gradFileReader = FileReaderType::New();
    gradFileReader->SetFileName((*i).c_str());
    try{
      gradFileReader->Update();
    } catch (itk::ExceptionObject e) {
      std::cerr << e << std::endl;
      return 0;
    }
    diffusionEstimator->AddGradientImage(*j,
                                         gradFileReader->GetOutput());
    
  }

  try{
    std::cout << "Estimating Tensor Field" << std::endl;
    diffusionEstimator->SetNumberOfThreads(1);
    diffusionEstimator->Update();
  }
  catch (itk::ExceptionObject e) {
    std::cerr << e.what() << std::endl;
    return 0;
    
  }
  
  try{
    std::cout << "Writing Tensor Field" << std::endl;
    TensorFileWriterType::Pointer tensorWriter = TensorFileWriterType::New();
    tensorWriter->SetFileName("output.nrrd");
    tensorWriter->SetInput(diffusionEstimator->GetOutput());
    tensorWriter->Update();
    
  }
  catch (itk::ExceptionObject e) {
    std::cerr << e << std::endl;
    return 0;
  }

  delete[] gradData;

  return 0;
}
