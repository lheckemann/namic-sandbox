#include <iostream>
#include <itkImage.h>
#include <itkLinearInterpolateImageFunction.h>
#include "itkTransformFactory.h"
#include <itkTransformFileReader.h>
#include "itkIO.h"
#include "itkScaleVersor3DTransform.h"
#include "itkScaleSkewVersor3DTransform.h"
#include "itkAffineTransform.h"
#include "itkVersorRigid3DTransform.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#include "ConvertToRigidAffine.h"

typedef itk::Image<float, 3>                                   ImageType;
typedef itk::LinearInterpolateImageFunction<ImageType, double> InterpolatorType;
typedef itk::ResampleImageFilter<ImageType,
  ImageType>         ResampleImageFilter;
typedef itk::AffineTransform<double, 3>                        AffineTransformType;
typedef itk::VersorRigid3DTransform<double>
VersorRigid3DTransformType;
typedef itk::ScaleVersor3DTransform<double>
ScaleVersor3DTransformType;
typedef itk::ScaleSkewVersor3DTransform<double>
ScaleSkewVersor3DTransformType;

AffineTransformType::Pointer
ReadTransform(const std::string & filename)
{
  itk::TransformFileReader::Pointer affineReader
    = itk::TransformFileReader::New();

  affineReader->SetFileName( filename.c_str() );
  affineReader->Update();
  AffineTransformType::Pointer returnval
    = AffineTransformType::New();

  //  This interpretation of a Transform List file
  //  as a single AffineTransform's Parameters
  //  is ridiculously restrictive, but there it is.
  //
  itk::TransformFileReader::TransformPointer transformFromFile
    = *( affineReader->GetTransformList()->begin() );

  // START: Copied out of BRAINSFit
  //  Choose TransformType for the itk registration class template:
  //
  // Process the initialITKTransform
  //
  AffineTransformType::Pointer initialITKTransform = AffineTransformType::New();
  initialITKTransform->SetIdentity();
    {
    try
      {
      typedef itk::TransformFileReader::TransformListType *TransformListType;
      TransformListType transforms = affineReader->GetTransformList();
      // std::cout << "Number of transforms = " << transforms->size() <<
      // std::endl;

      itk::TransformFileReader::TransformListType::const_iterator it
        = transforms->begin();
      const std::string
      transformFileType = ( *it )->GetNameOfClass();
      if ( transformFileType == "VersorRigid3DTransform" )
        {
        VersorRigid3DTransformType::Pointer tempInitializerITKTransform
          = static_cast<VersorRigid3DTransformType *>( ( *it ).GetPointer() );
        AssignRigid::AssignConvertedTransform(initialITKTransform,
          tempInitializerITKTransform);
        }
      else if ( transformFileType == "ScaleVersor3DTransform" )
        {
        ScaleVersor3DTransformType::Pointer tempInitializerITKTransform
          = static_cast<ScaleVersor3DTransformType *>( ( *it ).GetPointer() );
        AssignRigid::AssignConvertedTransform(initialITKTransform,
          tempInitializerITKTransform);
        }
      else if ( transformFileType == "ScaleSkewVersor3DTransform" )
        {
        ScaleSkewVersor3DTransformType::Pointer tempInitializerITKTransform
          = static_cast<ScaleSkewVersor3DTransformType *>( ( *it ).GetPointer() );
        AssignRigid::AssignConvertedTransform(initialITKTransform,
          tempInitializerITKTransform);
        }
      else if ( transformFileType == "AffineTransform" )
        {
        AffineTransformType::Pointer tempInitializerITKTransform
          = static_cast<AffineTransformType *>( ( *it ).GetPointer() );
        AssignRigid::AssignConvertedTransform(initialITKTransform,
          tempInitializerITKTransform);
        }
      else    //  NO SUCH CASE!!
        {
        std::cout
       <<
        "Unsupported initial transform file -- TransformBase first transform typestring, "
       << transformFileType
       <<
        " not equal to any recognized type VersorRigid3DTransform OR ScaleVersor3DTransform OR ScaleSkewVersor3DTransform OR AffineTransform"
       << std::endl;
        return NULL;
        }
      }
    catch ( itk::ExceptionObject & excp )
      {
      std::cout << "[FAILED]" << std::endl;
      std::cerr << "Error while reading the transforms" << std::endl;
      std::cerr << excp << std::endl;
      return NULL;
      }
    }
  // END Copied out of BRAINSFit

  return initialITKTransform;
}

ImageType::Pointer
Resample(ImageType::Pointer & inputImage,
  ImageType::Pointer & refImage,
  AffineTransformType::Pointer & transform)
{
  InterpolatorType::Pointer interp = InterpolatorType::New();

  interp->SetInputImage(inputImage);

  ResampleImageFilter::Pointer resample = ResampleImageFilter::New();
  resample->SetInput(inputImage);
  resample->SetTransform(transform);
  resample->SetInterpolator(interp);

  resample->SetOutputParametersFromImage(refImage);
  resample->Update();
  ImageType::Pointer returnval = resample->GetOutput();
  return returnval;
}

int main(int argc, char **argv)
{
  if ( argc != 5 )
    {
    std::cerr
   <<
    "ApplyTransform <inputimage> <outputimage> <transformfile> <referenceimage>"
   << std::endl;
    std::cerr
   <<
    "NOTE:  The reference image defines the space you want the output image to be placed in.  It is your \"fixedImage\" space used when defining the transformation.";
    exit(1);
    }
  std::string        inputFileName(argv[1]);
  std::string        outputFileName(argv[2]);
  std::string        transformFileName(argv[3]);
  std::string        refFileName(argv[4]);
  ImageType::Pointer refImage = itkUtil::ReadImage<ImageType>(
    refFileName);
  ImageType::Pointer inputImage = itkUtil::ReadImage<ImageType>(
    inputFileName);
  AffineTransformType::Pointer xfrm
    = ReadTransform( transformFileName.c_str() );

  if ( inputImage.IsNull() )
    {
    std::cerr << "Can't read " << argv[1] << std::endl;
    exit(1);
    }
  if ( xfrm.IsNull() )
    {
    std::cerr << "Can't read " << argv[3] << std::endl;
    exit(1);
    }
  ImageType::Pointer outputImage = Resample(inputImage, refImage, xfrm);
  itkUtil::WriteImage<ImageType>(outputImage, outputFileName);
  exit(0);
}
