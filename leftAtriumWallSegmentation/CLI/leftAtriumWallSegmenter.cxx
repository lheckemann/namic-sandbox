/**
 * Given the blood pool of the left atrium, extract the la-wall
 */

#include "leftAtriumWallSegmenterCLP.h"


#include <iostream>
#include <string>


#include "../localChanVeseWallSegmenter3D.h"
#include "../utilities.hxx"


//itk
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


//tst
#ifndef NDEBUG
#include <fstream>
std::ofstream leftAtriumWallSegmentGlobalOutputFile("leftAtriumWallSegmentGlobalOutputFile.txt");
#endif
//tst//


int main(int argc, char** argv)
{
  PARSE_ARGS; 


//   if (argc < 4)
//     {
//       std::cerr<<"Parameters: inputImage interiorMask outputImage\n";
//       exit(-1);
//     }

//   std::string inputImage(argv[1]);
//   std::string interiorMask(argv[2]);
//   std::string outputImage(argv[3]);

  unsigned long numiter = 100;

  typedef short PixelType;
  typedef CLocalChanVeseWallSegmenter3D< PixelType > segmenter_t;

  typedef itk::Image< PixelType, 3 > ImageType;

  /* read novel image */
  ImageType::Pointer img;
  img = readImage<ImageType>(originalImageFileName.c_str());

  /* read interiro mask */
  typedef segmenter_t::MaskImageType MaskImageType;
  MaskImageType::Pointer mask;
  mask = readImage<MaskImageType>(labelImageFileName.c_str());


  /* do segmentation */
  segmenter_t cv;
  cv.setImage(img);
  cv.setMask(mask);

  cv.setNumIter(numiter);

  cv.setNBHDSize(10, 10, 3);

  cv.doSegmenation();


  /* get final mask */
  writeImage<segmenter_t::MaskImageType>(cv.getWallMask(), segmentedImageFileName.c_str());


  //tst
#ifndef NDEBUG
  writeImage<segmenter_t::floatImage_t>(cv.mp_metricFromDistanceMapOfInterior, "mp_metricFromDistanceMapOfInterior.nrrd");
  writeImage<segmenter_t::MaskImageType>(cv.mp_maskOfInterior, "mp_maskOfInterior.nrrd");
  writeImage<segmenter_t::MaskImageType>(cv.mp_mask, "mp_mask.nrrd");
  writeImage<segmenter_t::floatImage_t>(cv.mp_distanceMapOfInterior, "mp_distanceMapOfInterior.nrrd");

  leftAtriumWallSegmentGlobalOutputFile.close();
#endif
  //tst//


  
  return 0;
}

