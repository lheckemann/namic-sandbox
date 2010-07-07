/*
  Test the registration part of the shapeProjector:

  Input:
  
  1. segmentation result: A

  2. mean shape: B

  Output:

  1. result of reg A to B


  20100408
  Yi Gao
*/



//newProstate
#include "cArrayOp.h"
#include "txtIO.h"

#include "shapeProjector.h"



int main(int argc, char** argv)
{
  if (argc < 4)
    {
      std::cerr<<"Usage: "<<argv[0]<<" newShape meanShape eigenShapeList\n";
      exit(-1);
    }

  std::string newShapeName(argv[1]);
  std::string meanShapeName = argv[2];
  std::string eigenShapeNameListName = argv[3];

  
  typedef float pixel_t;
  typedef itk::Image<pixel_t, 3> itkImage_t;


  // read new iamge
  itkImage_t::Pointer newShape = newProstate::readImage3<pixel_t>(newShapeName.c_str());

  // read mean iamge
  itkImage_t::Pointer meanShape = newProstate::readImage3<pixel_t>(meanShapeName.c_str());


  // read eigen images
  std::vector< std::string > eigenShapeNameList \
    = newProstate::readTextLineToListOfString<char>(eigenShapeNameListName.c_str());

  std::vector< itkImage_t::Pointer > eigenShapeList = newProstate::readImageSeries3<pixel_t>( eigenShapeNameList );



  typedef newProstate::shapeProjector<float> shapeProjector_t;
  shapeProjector_t::Pointer projector = shapeProjector_t::New();

  projector->setNewShape(newShape);

  projector->setMeanShape(meanShape);
  projector->setEigenShapeList(eigenShapeList);

  projector->gogogo();

//   newProstate::writeImage3<pixel_t>(projector->m_regNewShape, regedNewShapeName);
  
  return 0;
}

