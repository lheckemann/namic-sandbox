#include <iostream>


#include "SFLSRobustStatSegmentor3DProbMap.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "walltime.h"

int main(int argc, char** argv)
{
  if (argc != 7)
    {
      std::cerr<<"Parameters: inputImage probabilityMapName outputImage numIter intensityHomo[0~1] lambda[0~1]\n";
      exit(-1);
    }

  const char* inputImage = argv[1];
  const char* probMapName = argv[2];
  const char* outputImage = argv[3];
  unsigned long numiter = atol(argv[4]);
  double intensityHomo = atof(argv[5]);
  double lambda = atof(argv[6]);


  typedef short PixelType;
  typedef CSFLSRobustStatSegmentor3DProbMap< PixelType > SFLSRobustStatSegmentor3DProbMap_c;

  // read input image
  typedef SFLSRobustStatSegmentor3DProbMap_c::TImage Image_t;

  typedef itk::ImageFileReader< Image_t > ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(inputImage);
  Image_t::Pointer img;
    
  try
    {
      reader->Update();
      img = reader->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cerr<< "ExceptionObject caught !" << std::endl; 
      std::cerr<< err << std::endl; 
      raise(SIGABRT);
    }



  // read input label image
  typedef SFLSRobustStatSegmentor3DProbMap_c::TFloatImage ProbabilityMap_t;

  typedef itk::ImageFileReader< ProbabilityMap_t > ProbabilityMapReader_t;
  ProbabilityMapReader_t::Pointer readerLabel = ProbabilityMapReader_t::New();
  readerLabel->SetFileName(probMapName);
  ProbabilityMap_t::Pointer probMap;
    
  try
    {
      readerLabel->Update();
      probMap = readerLabel->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cerr<< "ExceptionObject caught !" << std::endl; 
      std::cerr<< err << std::endl; 
      raise(SIGABRT);
    }


  // do seg
  SFLSRobustStatSegmentor3DProbMap_c::Pointer seg = SFLSRobustStatSegmentor3DProbMap_c::New();
  seg->setImage(img);

  seg->setProbabilityMap(probMap);

  seg->setNumIter(numiter);

  seg->setIntensityHomogeneity(intensityHomo);
  seg->setCurvatureWeight(lambda);




//   double timeZero = 0.0;
//   double startTime = 0.0;
//   double ellapseTime = 0.0;

  //startTime = walltime(&timeZero); // this is necessary
  seg->doSegmenation();


  //ellapseTime = walltime(&startTime); 

  //std::cout<<"wall time = "<<ellapseTime<<std::endl;



  typedef itk::ImageFileWriter< SFLSRobustStatSegmentor3DProbMap_c::LSImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImage );
  writer->SetInput(seg->mp_phi);

  try
    {
      writer->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      raise(SIGABRT);   
    }

  
  return 0;
}



