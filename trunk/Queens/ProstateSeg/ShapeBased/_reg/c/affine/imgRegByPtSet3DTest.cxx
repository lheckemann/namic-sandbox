#include "imgRegByPtSet3D.h"

#include "cArrayOp.h"

int main(int argc, char** argv)
{
  if( argc != 6 )
    {
      std::cerr << "Arguments Missing. " << std::endl;
      std::cerr << "Usage:  "<<argv[0]<<" fixedImageFile movingImageFile numOfPts lambda finalImageFileName\n";
      exit(-1);
    }


  const char* imFileName1 = argv[1];
  const char* imFileName2 = argv[2];
  unsigned long numOfPt = atoi(argv[3]);
  double lambda = atof(argv[4]);
  const char* finalImageFileName = argv[5];

  DoubleImage3DType::Pointer image1 = douher::readImage3< double >(imFileName1);
  DoubleImage3DType::Pointer image2 = douher::readImage3< double >(imFileName2);

  CImgRegByPtSet3D imgRegisterer(image1, image2);

  imgRegisterer.setLambda(lambda);
  imgRegisterer.setNumberOfPoint(numOfPt);

  imgRegisterer.gogogo();

  DoubleImage3DType::Pointer resultImg = imgRegisterer.getRegisteredMovingImage();

  douher::writeImage3< double >(resultImg, finalImageFileName);

  return 0;
}
