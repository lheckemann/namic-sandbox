//std
#include <string>

//local
#include "reg_3d_affine_mse.h"
#include "reg_3d_demons.h"
#include "transformImage.h"
#include "utilities.h"


int main(int argc, char** argv)
{
  if (argc < 7)
    {
      std::cerr<<"args: postMRI preMRI postEndo preEndo postToPreMRI postToPreEndo\n";
      exit(-1);
    }

  std::string postMRIFileName(argv[1]);
  std::string preMRIFileName(argv[2]);
  std::string postEndoFileName(argv[3]);
  std::string preEndoFileName(argv[4]);
  std::string postToPreMRIFileName(argv[5]);
  std::string postToPreEndoFileName(argv[6]);

  
  typedef float PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image<PixelType, Dimension> ImageType;

  typedef unsigned char LabelPixelType;
  typedef itk::Image<LabelPixelType, Dimension> LabelImageType;

  // Read in images
  ImageType::Pointer postMRI = afibReg::readImage<ImageType>(postMRIFileName.c_str());
  ImageType::Pointer preMRI = afibReg::readImage<ImageType>(preMRIFileName.c_str());

  ImageType::Pointer postEndo = afibReg::readImage<ImageType>(postEndoFileName.c_str());
  ImageType::Pointer preEndo = afibReg::readImage<ImageType>(preEndoFileName.c_str());


  // crop the masks for faster registration
  ImageType::Pointer preEndoROIMask = afibReg::cropROIFromImage<ImageType>(preEndo);
  ImageType::Pointer postEndoROIMask = afibReg::cropROIFromImage<ImageType>(postEndo);

  // affine register
  typedef itk::AffineTransform<double, Dimension> AffineTransformType;
  double finalRegCost = 0.0;
  //AffineTransformType::Pointer trans = afibReg::affineMSERegistration<ImageType, ImageType>(preEndo, postEndo, finalRegCost);
  AffineTransformType::Pointer trans = afibReg::affineMSERegistration<ImageType, ImageType>(preEndoROIMask, postEndoROIMask, finalRegCost);

  double fillInValue = 0.0;
  ImageType::Pointer postToPreEndo = afibReg::transformImage<ImageType, ImageType>(trans, postEndo, preEndo, fillInValue);
  ImageType::Pointer postToPreMRI = afibReg::transformImage<ImageType, ImageType>(trans, postMRI, preMRI, fillInValue);

  // output affine results
  std::string extName = ".nrrd";
  std::string postToPreMRIAffineFileName = postToPreMRIFileName;
  std::size_t found = postToPreMRIAffineFileName.rfind(extName);
  if (found != std::string::npos)
    {
      postToPreMRIAffineFileName.replace(found, extName.length(),"_affine.nrrd");
    }
    
  std::string postToPreEndoAffineFileName = postToPreEndoFileName;
  found = postToPreEndoAffineFileName.rfind(extName);
  if (found != std::string::npos)
    {
      postToPreEndoAffineFileName.replace(found, extName.length(),"_affine.nrrd");
    }
  
  afibReg::writeImage<ImageType>(postToPreEndo, postToPreEndoAffineFileName.c_str());
  afibReg::writeImage<ImageType>(postToPreMRI, postToPreMRIAffineFileName.c_str());
  // output affine results, done


  // demons register
  std::cout<<"Demons registration\n"<<std::flush;

  typedef itk::Image< itk::Vector< float, Dimension >, Dimension > DeformationFieldType;
  DeformationFieldType::Pointer vectorField = afibReg::reg_3d_demons<ImageType, ImageType, ImageType>(preEndo, postToPreEndo, fillInValue);

  char interpType = 1;
  ImageType::Pointer postToPreEndoDemons                                \
    = afibReg::warpImage<ImageType, ImageType, DeformationFieldType>( vectorField, postToPreEndo, preEndo, fillInValue, interpType);

  ImageType::Pointer postToPreMRIDemons                                 \
    = afibReg::warpImage<ImageType, ImageType, DeformationFieldType>( vectorField, postToPreMRI, preMRI, fillInValue, interpType);


  // output 
  afibReg::writeImage<ImageType>(postToPreEndoDemons, postToPreEndoFileName.c_str());
  afibReg::writeImage<ImageType>(postToPreMRIDemons, postToPreMRIFileName.c_str());

  
  return 0;
}

