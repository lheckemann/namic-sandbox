/**
 * Atlas based segmentation
 *
 * 20100521
 * Yi Gao
 */


//std
#include <vector>
#include <string>

//douher
#include "txtIO.h"

// itk
#include "itkImage.h"

// local
#include "atlasSegMI.h"

int main(int argc, char** argv)
{
  if (argc < 5)
    {
      std::cerr<<"args: rawImageName trainingImageListName labelImageListName outputLabelName\n";
      exit(-1);
    }

  std::string rawImageName(argv[1]);

  std::string trainingImageListName(argv[2]);
  std::string labelImageListName(argv[3]);

  std::string outputLabelName(argv[4]);



  typedef float pixel_t;
  typedef itk::Image<pixel_t, 3> image_t;


  /**
   * Read in raw image
   */
  image_t::Pointer img = douher::readImage3<image_t::PixelType>(rawImageName.c_str());



  /**
   * Read in training images' names
   */
  std::vector< std::string > trainingImageList = douher::readTextLineToListOfString<char>(trainingImageListName.c_str());


  /**
   * Read in label images' names
   */
  std::vector< std::string > labelImageList = douher::readTextLineToListOfString<char>(labelImageListName.c_str());



  typedef unsigned char labelPixel_t;
  typedef itk::Image<labelPixel_t, 3> labelImage_t;

  // atlas based seg
  std::cout<<"Segmenting "<<rawImageName<<"...\n"<<std::flush;
  labelImage_t::Pointer resultLabel = atlasSegMI<image_t, labelImage_t>(img, trainingImageList, labelImageList);



  // write result
  douher::writeImage3<labelPixel_t>(resultLabel, outputLabelName.c_str());
  
  return 0;
}
