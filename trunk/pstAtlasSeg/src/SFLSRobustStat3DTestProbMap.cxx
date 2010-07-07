#include <iostream>


#include "SFLSRobustStatSegmentor3DProbMap.h"

#include "thld3.hpp"


#include <limits>

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
  typedef newProstate::CSFLSRobustStatSegmentor3DProbMap< PixelType > SFLSRobustStatSegmentor3DProbMap_c;

  // read input image
  typedef SFLSRobustStatSegmentor3DProbMap_c::TImage Image_t;
  Image_t::Pointer img = newProstate::readImage3<Image_t::PixelType>(inputImage);

  // read input probability map
  typedef SFLSRobustStatSegmentor3DProbMap_c::TFloatImage ProbabilityMap_t;
  ProbabilityMap_t::Pointer probMap = newProstate::readImage3<ProbabilityMap_t::PixelType>(probMapName);
    
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



  /* thld level set image to get binary output */
  typedef itk::Image<unsigned char, 3> outputImage_t;

  float posInf = std::numeric_limits<SFLSRobustStatSegmentor3DProbMap_c::LSImageType::PixelType>::max();
  outputImage_t::Pointer outputBinaryImg = newProstate::thld3< SFLSRobustStatSegmentor3DProbMap_c::LSImageType, outputImage_t>(seg->mp_phi, 0, posInf, 0, 1);

  /* write out binary results */
  newProstate::writeImage3<outputImage_t::PixelType>(outputBinaryImg, outputImage);

  
  return 0;
}



