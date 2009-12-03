/*
  GAC and shape based segmentation. The feature image is given from a file.
*/

#include <iostream>
#include <string>

//douher
#include "lib/cArrayOp.h"
#include "lib/cArray3D.h"
#include "lib/txtIO.h"


#include "shapeBasedGAC.h"
#include "shapeBasedSeg.h"


douher::cArray3D< double >::Pointer connection(douher::cArray3D< double >::Pointer phi);

int main(int argc, char** argv)
{
  /* Assume the mean and eigen shapes are stored in the _seg/shapeSpaceZflip/ */


  if (argc != 9)
    {
      std::cerr<<"Parameters: inputImage outputImage lx ly lz rx ry rz\n";
      exit(-1);
    }

  const char* inputImage = argv[1];  
  const char* outputImage = argv[2];

  const char* meanShapeName = "../shapeSpaceZflip/meanImage.nrrd";
  const char* eigenList = "../shapeSpaceZflip/eigenShapeWithStdList.txt";


  long lx = atol(argv[3]);
  long ly = atol(argv[4]);
  long lz = atol(argv[5]);

  long rx = atol(argv[6]);
  long ry = atol(argv[7]);
  long rz = atol(argv[8]);


  typedef double PixelType;
  typedef douher::cArray3D< PixelType > ImageType;
  typedef douher::cArray3D< unsigned char > MaskImageType;




  // 1. GAC
  ImageType::Pointer img = douher::readImageToArray3< PixelType >(inputImage);

  //  ImageType::Pointer featureImg = douher::readImageToArray3< PixelType >("feature.nrrd");


  CShapeBasedGAC::Pointer gac = CShapeBasedGAC::New();
  gac->setImage(img);
  gac->setLeftAndRightPointsInTheMiddleSlice(lx, ly, lz, rx, ry, rz);


  // mean shape
  gac->setMeanShape(douher::readImageToArray3< double >( meanShapeName ) );


  long numiter = 10;
  gac->setNumIter(numiter);

  // do segmentation
  gac->doShapeBasedGACSegmenation();

  //debug//
  douher::saveAsImage3< double >(gac->mp_phi, "gacResult.nrrd");
  //DEBUG//

  //debug//
  douher::saveAsImage3< double >(connection(gac->mp_phi), "gacResultBin.nrrd");
  //DEBUG//


  // 2. Tsai's shape based
  CShapeBasedSeg c;
  c.setInputImage(douher::cArray3ToItkImage< double >(connection(gac->mp_phi)) );

  c.setMeanShape(douher::readImage3< double >( meanShapeName ));


  // eigen shapes
  long numEigen = 8;
  std::vector< std::string > nameOfShapes = douher::readTextLineToListOfString(eigenList);

  if (numEigen > (long)nameOfShapes.size())
    {
      numEigen = nameOfShapes.size();
    }

  std::vector< douher::cArray3D< double >::Pointer > eigenShapes;

  for (int i = 0; i < numEigen; ++i)
    {
      c.addEigenShape(douher::readImage3< double >( nameOfShapes[i].c_str() ));
    }

//   for (int i = 0; i < numEigen; ++i)
//     {
//       c.addEigenShape( douher::cArray3ToItkImage< double >(gac->m_eigenShapes[i]) );
//     }


  //debug//
  //  c.setMaxIteration(0);
  //DEBUG//

  c.gogogo();

  std::cout<<"current param = "<<c.getParameters()<<std::endl;

  std::cout<<"current cost = "<<c.getCost()<<std::endl;


  typedef CShapeBasedSeg::DoubleImageType DoubleImageType;
  DoubleImageType::Pointer segRslt = c.getSegmentation();

  douher::writeImage3< double >(segRslt, outputImage);

  
  return 0;
}


douher::cArray3D< double >::Pointer connection(douher::cArray3D< double >::Pointer phi)
{
  douher::cArray3D< double >::Pointer bin(new douher::cArray3D< double >(phi));

  long nx = phi->getSizeX();
  long ny = phi->getSizeY();
  long nz = phi->getSizeZ();
  
  long n = nx*ny*nz;

  for (long i = 0; i < n; ++i)
    {
      (*bin)[i] = (*bin)[i]<=0?1:0; // use 2.5, instead of 0, to enclose more
    }

  return bin;
}
