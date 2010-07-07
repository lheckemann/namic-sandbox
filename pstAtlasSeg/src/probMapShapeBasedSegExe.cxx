/*
  Use the rss with probability map to do the segmentation, then
  project the shape into the feasible shape space.

  20100705
  20100705
  Yi Gao
*/


//std
#include <string>
#include <limits>


// local
#include "cArrayOp.h"
#include "txtIO.h"
#include "thld3.hpp"

#include "probMapShapeBasedSeg.h"


int main(int argc, char** argv)
{
  if (argc < 6)
    {
      std::cerr<<"args: inputImage outputImageName probabilityMapName meanShape eigenShapeList"\
               <<" [numIter=10] [intensityHomo=0.1] [curvatureWeight=0.1]\n";
      exit(-1);
    }

  std::string originalImageFileName(argv[1]);
  std::string outputImageName(argv[2]);

  std::string probMapName(argv[3]);
  std::string meanShapeName = argv[4];
  std::string eigenShapeNameListName = argv[5];


  /**
   * decide number of iterations
   */
  long numIter = 10;
  if (argc >= 7)
    {
      numIter = atol(argv[6]);
    }


  /**
   * decide intensity homo
   */
  double intensityHomo = 0.1;
  if (argc >= 8)
    {
      intensityHomo = atof(argv[7]);
    }


  /**
   * decide curvature weight/smoothness:
   */
  double curvatureWeight = 0.1;
  if (argc >= 9)
    {
      curvatureWeight = atof(argv[8]);
    }


  typedef short pixel_t;
  typedef newProstate::probMapShapeBasedSeg< short, float > shapeBasesSegmenter_t;

  typedef shapeBasesSegmenter_t::TImage image_t;

  /**
   * Read in target image
   */
  image_t::Pointer img = newProstate::readImage3<image_t::PixelType>(originalImageFileName.c_str());


  /**
   * Read in prob image
   */
  typedef shapeBasesSegmenter_t::TFloatImage floatImage_t;
  typedef floatImage_t ProbabilityMap_t;
  ProbabilityMap_t::Pointer probMap = newProstate::readImage3<ProbabilityMap_t::PixelType>(probMapName.c_str());

  /** 
      read mean shape
  */
  floatImage_t::Pointer meanShape = newProstate::readImage3<floatImage_t::PixelType>(meanShapeName.c_str());


  /** 
      read eigen shapes
  */
  std::vector< std::string > eigenShapeNameList \
    = newProstate::readTextLineToListOfString<char>(eigenShapeNameListName.c_str());

  std::vector< floatImage_t::Pointer > eigenShapeList = newProstate::readImageSeries3<floatImage_t::PixelType>( eigenShapeNameList );



  /**
     do the shape based rss with prob map
  */
  shapeBasesSegmenter_t::Pointer seg = shapeBasesSegmenter_t::New();
  seg->setImage(img);

  /**
   * Feed parameters to shape based segmenter
   * RSS part
   */
  seg->setProbabilityMap(probMap);
  seg->setNumIter(numIter);
  seg->setIntensityHomogeneity(intensityHomo);
  seg->setCurvatureWeight(curvatureWeight);


  /**
   * Feed parameters to shape based segmenter
   * shape part
   */
  seg->setMeanShape(meanShape);
  seg->setEigenShapeList(eigenShapeList);


  // run
  seg->doShapeBasesSeg();


  /**
   * save data driven segmentation result
   */
  //saveDataDrivenSegResult(seg->mp_phi, originalImageFileName, numIter, inflation, curvatureWeight);


  // threshold to binary and write result
  typedef itk::Image<unsigned char, 3> ucharImage_t;
  float posInf = std::numeric_limits<floatImage_t::PixelType>::max();
  ucharImage_t::Pointer outputBinaryImg                                 \
    = newProstate::thld3< floatImage_t, ucharImage_t>(seg->m_regNewShapeProjectedBack, 0.5, posInf, 1, 0);


  newProstate::writeImage3<unsigned char>(outputBinaryImg, outputImageName.c_str());
  
  return 0;
}
