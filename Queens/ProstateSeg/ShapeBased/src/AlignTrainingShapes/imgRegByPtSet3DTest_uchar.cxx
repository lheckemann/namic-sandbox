#include "imgRegByPtSet3D_uchar.h"

#include "cArrayOp.h"


#include <ctime>

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

  std::cout<<"imFileName1 = "<<imFileName1<<std::endl;
  std::cout<<"imFileName2 = "<<imFileName2<<std::endl;
  std::cout<<"numOfPt = "<<numOfPt<<std::endl;
  std::cout<<"lambda = "<<lambda<<std::endl;
  std::cout<<"finalImageFileName = "<<finalImageFileName<<std::endl;


  DoubleImage3DType::Pointer image1 = douher::readImage3< double >(imFileName1);
  DoubleImage3DType::Pointer image2 = douher::readImage3< double >(imFileName2);

  CImgRegByPtSet3D_uchar imgRegisterer(image1, image2);

  imgRegisterer.setLambda(lambda);
  imgRegisterer.setNumberOfPoint(numOfPt);

  imgRegisterer.setMaxIteration(50);

  ////////////////////////////////////////////////////////////
  // begin timing
  double t1, t2;
  t1 = clock();

  imgRegisterer.gogogo();

  t2 = clock();
  t1 = (t2 - t1)/CLOCKS_PER_SEC;
  std::cerr<<t1<<std::endl;  
  // end timing
  ////////////////////////////////////////////////////////////


//   imgRegisterer.savePointSetToTextFile3D(imgRegisterer.m_fixedPointSet, "fixedPtSet.txt");
//   imgRegisterer.savePointSetToTextFile3D(imgRegisterer.m_movingPointSet, "movingPtSet.txt");

  UCHARImage3DPointerType resultImg = imgRegisterer.getRegisteredMovingImage();

  douher::writeImage3< unsigned char >(resultImg, finalImageFileName);

  return 0;
}
