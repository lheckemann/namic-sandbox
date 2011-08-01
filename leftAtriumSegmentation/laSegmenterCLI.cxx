/**
 * Use the multi atlas to generate a prob map, then rss with
 * probability map to do the segmentation, then project the shape into
 * the feasible shape space.

 * 20101031
 * 20101031
 * Yi Gao
 */


//std
#include <string>
#include <limits>


//local
#include "utilities.hxx"
#include "atlas/atlasSegMI.h"

// by slicer
#include "laSegmenterCLICLP.h"

// config by cmake
#include "cmakeConfig.h"

int main(int argc, char** argv)
{
  PARSE_ARGS; 


  std::string dirOfCmakeList(CMAKE_CURRENT_SOURCE_DIR);
  std::string trainingImagePath = dirOfCmakeList + "/data";

//   if (argc < 4)
//     {
//       std::cerr<<"args: rawImageName outputSegmentationBinaryImageName trainingImagePath\n";
//       exit(-1);
//     }

//   std::string rawImageName(argv[1]);
//   std::string outputSegmentationBinaryImageName(argv[2]);
//   std::string trainingImagePath(argv[3]);


  typedef float pixel_t;
  typedef itk::Image<pixel_t, 3> image_t;

  typedef unsigned char labelPixel_t;
  typedef itk::Image<labelPixel_t, 3> labelImage_t;

  /**
   * Multi-atlas registration to generate probability map.
   */


  // Read in raw image
  image_t::Pointer img = laSeg::readImage<image_t>(originalImageFileName.c_str());


  // Read in training images' names
  std::string trainingImageListName = trainingImagePath + "/" + "trainingImages.txt";


  std::vector< std::string > trainingImageNameList = laSeg::readTextLineToListOfString<char>(trainingImageListName.c_str());
  for (size_t i = 0; i < trainingImageNameList.size(); ++i)
    {
      trainingImageNameList[i] = trainingImagePath + "/" + trainingImageNameList[i];
      
      //      std::cout<<trainingImageNameList[i]<<std::endl;
    }

  std::vector< image_t::Pointer > trainingImageList = laSeg::readImageSeries<image_t>(trainingImageNameList);


  //  std::cout<<"--------------------------------------------------------------------------------\n";


  // Read in training label images' names
  std::string labelImageListName = trainingImagePath + "/trainingLabelImages.txt";
  std::vector< std::string > labelImageNameList = laSeg::readTextLineToListOfString<char>(labelImageListName.c_str());
  for (size_t i = 0; i < labelImageNameList.size(); ++i)
    {
      labelImageNameList[i] = trainingImagePath + "/" + labelImageNameList[i];

      //      std::cout<<labelImageNameList[i]<<std::endl;
    }

  std::vector< labelImage_t::Pointer > labelImageList = laSeg::readImageSeries<labelImage_t>(labelImageNameList);

  
  // extract binary laSeg mask from original label map
  //laSeg::extractLaSegLabelAll<labelImage_t>(labelImageList);



  // atlas based seg
  image_t::Pointer probabilityMap                                       \
    = laSeg::atlasSegMI_ROI_weight<image_t, labelImage_t>(img, trainingImageList, labelImageList);


  // post process prob map
  image_t::PixelType c = 200;
  itk::Image<unsigned char, 3>::Pointer postProcProbabilityMap          \
    = laSeg::postProcessProbabilityMap<image_t>(probabilityMap, c);


  // write result
  laSeg::writeImage<itk::Image<unsigned char, 3> >(postProcProbabilityMap, outputProbabilityImageName.c_str());
  
  return 0;
}




