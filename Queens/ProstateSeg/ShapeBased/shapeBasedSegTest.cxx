#include "shapeBasedSeg.h"

//std
#include <string>

//douher
#include "lib/cArrayOp.h"
#include "lib/txtIO.h"

int main( int argc, char * argv[] )
{
  if (argc != 5)
    {
      std::cerr<<"Usage: "<<argv[0]<<" inputImg outputShape meanShape eigenShapeList\n";
      exit(-1);
    }

  const char* inputImageName = argv[1];
  const char* outputShapeName = argv[2];
  const char* meanShapeName = argv[3];
  const char* eigenShapeImgListName = argv[4];


  typedef CShapeBasedSeg::DoubleImageType DoubleImageType;


  DoubleImageType::Pointer inputImage = douher::readImage3< double >(inputImageName);
  DoubleImageType::Pointer meanShape = douher::readImage3< double >(meanShapeName);

  std::vector< std::string > eigenShapeNameList = douher::readTextLineToListOfString(eigenShapeImgListName);


  CShapeBasedSeg c;
  c.setInputImage(inputImage);

  c.setMeanShape(meanShape);

  for (int i = 0; i < (int)eigenShapeNameList.size(); ++i)
    {
      c.addEigenShape(douher::readImage3< double >( eigenShapeNameList[i].c_str() ) );
    }

  c.gogogo();

  std::cout<<"current param = "<<c.getParameters()<<std::endl;

  std::cout<<"current cost = "<<c.getCost()<<std::endl;

  
  DoubleImageType::Pointer segRslt = c.getSegmentation();

  douher::writeImage3< double >(segRslt, outputShapeName);

  return EXIT_SUCCESS;
}
