#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "set"

#include "itkPoint.h"
#include "itkImage.h"
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

#include "itkGrayscaleConnectedOpeningImageFilter.h"
#include "itkGrayscaleFunctionDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkSignedMaurerDistanceMapImageFilter.h" 
#include "itkSphereSpatialFunction.h"
#include "itkFloodFilledSpatialFunctionConditionalIterator.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkVotingBinaryIterativeHoleFillingImageFilter.h" 
#include "itkBinaryThresholdImageFunction.h"

#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBinaryBallStructuringElement.h"

#include "itkScalarImageToListAdaptor.h"
#include "itkListSampleToHistogramGenerator.h"

#include "itkFuzzyClassificationImageFilter.h"

#include "TissueClassificationCLP.h"

typedef itk::OrientedImage<float, 3> ImageType;
typedef itk::ImageFileReader< ImageType  > ImageReaderType;

typedef itk::OrientedImage<unsigned char, 3> LabelImageType;

LabelImageType::Pointer BinaryErodeFilter3D ( LabelImageType::Pointer & img , unsigned int ballsize )
{
  typedef itk::BinaryBallStructuringElement<unsigned char, 3> KernalType;
  typedef itk::BinaryErodeImageFilter<LabelImageType, LabelImageType, KernalType> ErodeFilterType;
  ErodeFilterType::Pointer erodeFilter = ErodeFilterType::New();
  erodeFilter->SetInput( img );

  KernalType ball;
  KernalType::SizeType ballSize;
  for (int k = 0; k < 3; k++)
    {
    ballSize[k] = ballsize;
    }
  ball.SetRadius(ballSize);
  ball.CreateStructuringElement();
  erodeFilter->SetKernel( ball );
  erodeFilter->Update();
  return erodeFilter->GetOutput();

}

LabelImageType::Pointer BinaryDilateFilter3D ( LabelImageType::Pointer & img , unsigned int ballsize )
{
  typedef itk::BinaryBallStructuringElement<unsigned char, 3> KernalType;
  typedef itk::BinaryDilateImageFilter<LabelImageType, LabelImageType, KernalType> DilateFilterType;
  DilateFilterType::Pointer dilateFilter = DilateFilterType::New();
  dilateFilter->SetInput( img );
  KernalType ball;
  KernalType::SizeType ballSize;
  for (int k = 0; k < 3; k++)
    {
    ballSize[k] = ballsize;
    }
  ball.SetRadius(ballSize);
  ball.CreateStructuringElement();
  dilateFilter->SetKernel( ball );
  dilateFilter->Update();
  return dilateFilter->GetOutput();
}

LabelImageType::Pointer BinaryOpeningFilter3D ( LabelImageType::Pointer & img , unsigned int ballsize )
{
  LabelImageType::Pointer imgErode = BinaryErodeFilter3D( img, ballsize );
  return BinaryDilateFilter3D( imgErode, ballsize );
}

LabelImageType::Pointer BinaryClosingFilter3D ( LabelImageType::Pointer & img , unsigned int ballsize )
{
  LabelImageType::Pointer imgDilate = BinaryDilateFilter3D( img, ballsize );
  return BinaryErodeFilter3D( imgDilate, ballsize );
}

int main (int argc, char* argv[])
{

  PARSE_ARGS;
  
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( inputVolume.c_str() );
  ImageType::Pointer image;
  try
    {
    reader->Update();
    image = reader->GetOutput();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  ImageReaderType::Pointer maskreader = ImageReaderType::New();
  maskreader->SetFileName( maskVolume.c_str() );
  ImageType::Pointer mask;
  try
    {
    maskreader->Update();
    mask = maskreader->GetOutput();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::FuzzyClassificationImageFilter<ImageType, ImageType> ClassifierType;
  ClassifierType::Pointer classifier = ClassifierType::New();
  classifier->SetInput( image );

  classifier->SetNumberOfClasses( nClasses );
  classifier->SetBiasCorrectionOption( nBiasOption );
  classifier->SetImageMask( mask );

  try
    {
    classifier->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  ImageType::Pointer label = ImageType::New();
  label->CopyInformation( image );
  label->SetRegions( label->GetLargestPossibleRegion() );
  label->Allocate();
  label->FillBuffer( 0 );

  itk::ImageRegionIteratorWithIndex<ImageType> it( label, label->GetLargestPossibleRegion() );
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    ImageType::IndexType idx = it.GetIndex();
    ImageType::PixelType mLabel = 0.25;
    int assignedLabel = -1;
    for (int k = 0; k < nClasses; k++)
    {
      ImageType::PixelType p = classifier->GetOutput(k)->GetPixel(idx);
      if (p > mLabel)
      {
        mLabel = p;
        assignedLabel = k;
      }
    }
    if (assignedLabel >=0)
    {
      it.Set( assignedLabel+1 );
    }
  }
  itk::ImageFileWriter<ImageType>::Pointer wlabel = itk::ImageFileWriter<ImageType>::New();
  wlabel->SetInput( label );
  wlabel->SetFileName( brainLabel.c_str() );

  try
    {
    wlabel->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  ImageType::Pointer biasField = classifier->GetBiasField();

  wlabel->SetInput( biasField );
  wlabel->SetFileName( biasFieldFile.c_str() );

  try
    {
    wlabel->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  const std::vector<float>& classCenter = classifier->GetClassCentroid();
  const std::vector<float>& classStd = classifier->GetClassStandardDeviation();

  for (int k = 0; k < nClasses; k++)
  {
    std::cout << "Class " << k << ":\n\t";
    std::cout << classCenter[k] << " +/- " << classStd[k] << std::endl;
  }

  // extrapolate segmentation to entire image
  
  std::vector<ImageType::Pointer> mems (nClasses);
  for (int k = 0; k < nClasses; k++)
  {
    mems[k] = ImageType::New();
    mems[k]->CopyInformation( image );
    mems[k]->SetRegions( image->GetLargestPossibleRegion() );
    mems[k]->Allocate();
    mems[k]->FillBuffer( 0.0 );
  }
  
  std::vector<double> values(nClasses);

  typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
  typedef itk::ImageRegionIterator< ImageType > IteratorType;
  for (int k = 0; k < nClasses; k++) {
    //iterate through each pixel j:
    ConstIteratorType ity (image, image->GetRequestedRegion());
    ConstIteratorType itg (biasField, biasField->GetRequestedRegion());
    IteratorType itu (mems[k], mems[k]->GetRequestedRegion());

    for (ity.GoToBegin(), itg.GoToBegin(), itu.GoToBegin(); 
         !ity.IsAtEnd(); 
         ++ity, ++itg, ++itu) {
      //Skip background pixels.
      float img_y_j = ity.Get();
      if (img_y_j < classCenter[0])
        continue;

      float gain_field_g_j = itg.Get();

      ///double numerator = img_y[j] - centroid_v[k] * gain_field_g[j];
      double numerator = img_y_j - classCenter[k] * gain_field_g_j;
      
      if (numerator != 0)
        numerator = 1 / (numerator * numerator);
      else if (gain_field_g_j == 1) {
        //The divide-by-zero happens when img_y[j] == centroid_v[k].
        //In this case, the membership function should be 1 for this class and 
        //0 for all other classes (for normalization).
        itu.Set (1);
        continue; //Done for the current pixel.
      }
      else {
        //Keep numerator as 0 for this unlikely-to-happen case.
        ///assert (0);
      }

      double denominator = 0;
      for (int l = 0; l < nClasses; l++) {
        ///double denominator_l = img_y[j] - centroid_v[l] * gain_field_g[j];
        double denominator_l = img_y_j -classCenter[l] * gain_field_g_j;
        
        if (denominator_l != 0) 
          denominator_l = 1 / (denominator_l * denominator_l);
        else {
          //This is the case when the same pixel of other class than k has mem_fun == 1.
          //Set the membership function to 0.          
          itu.Set (0);
          continue;
        }

        denominator += denominator_l;
      }
      ///mem_fun_u[k][j] = numerator / denominator;
      assert (denominator != 0);
      itu.Set (numerator / denominator);
    }
  }
  
  size_t nLastDot = brainLabel.find_last_of('.');
  std::string fileNameStem = brainLabel.substr( 0, nLastDot );
  for (int k = 0; k < nClasses; k++)
    {
      char memFileName[1024];
      wlabel->SetInput( mems[k] );
      sprintf( memFileName, "%s-mem%d.mha", fileNameStem.c_str(), k );
      wlabel->SetFileName( memFileName );
      wlabel->Update();
    } 

  // Declare the type for the structuring element
  typedef itk::BinaryBallStructuringElement<unsigned short, 3> KernelType;
  
  // Declare the type for the morphology Filter
  typedef itk::GrayscaleFunctionDilateImageFilter<ImageType, ImageType, KernelType> GrayDialteFilterType;
  typedef itk::GrayscaleConnectedOpeningImageFilter<
                            ImageType, 
                            ImageType >  ConnectedOpeningFilterType;

  // Create the filter
  GrayDialteFilterType::Pointer gDilate = GrayDialteFilterType::New();

  // Create the structuring element
  KernelType ball;
  KernelType::SizeType ballSize;
  ballSize[0] = 1;
  ballSize[1] = 1;
  ballSize[2] = 1;
  ball.SetRadius(ballSize);
  ball.CreateStructuringElement();
  
  // Connect the input image
  gDilate->SetInput( mems[0] );
  gDilate->SetKernel( ball );
  gDilate->Update();

  itk::ImageRegionIterator<ImageType> ity (gDilate->GetOutput(), gDilate->GetOutput()->GetRequestedRegion());
  for (ity.GoToBegin(); !ity.IsAtEnd(); ++ity)
    {
      ity.Set( ity.Get()-1 );
    }

  char memFileName[1024];
  wlabel->SetInput( gDilate->GetOutput() );
  sprintf( memFileName, "%s-mem0-dilate.mha", fileNameStem.c_str() );
  wlabel->SetFileName( memFileName );
  wlabel->Update();
  
  ConnectedOpeningFilterType::Pointer  connectedOpening = ConnectedOpeningFilterType::New();
  // Setup the connected opening method
  connectedOpening->SetInput( mems[2] );

  ImageType::IndexType seed;
  seed[0] = 101;
  seed[1] = 111;
  seed[2] = 68;
  connectedOpening->SetSeed(seed);
  connectedOpening->Update();

  wlabel->SetInput( connectedOpening->GetOutput() );
  sprintf( memFileName, "%s-mem2-open.mha", fileNameStem.c_str() );
  wlabel->SetFileName( memFileName );
  wlabel->Update();

  LabelImageType::Pointer wmLabel = LabelImageType::New();
  wmLabel->CopyInformation( image );
  wmLabel->SetRegions( wmLabel->GetLargestPossibleRegion() );
  wmLabel->Allocate();

  itk::ImageRegionIteratorWithIndex<ImageType> itx (connectedOpening->GetOutput(), connectedOpening->GetOutput()->GetRequestedRegion());
  for (itx.GoToBegin(); !itx.IsAtEnd(); ++itx)
    {
      LabelImageType::IndexType idx = itx.GetIndex();
      if (itx.Get() > 0.5)
        {
          wmLabel->SetPixel( idx, 255 );
        }
    }

  LabelImageType::Pointer wmOpen = BinaryOpeningFilter3D ( wmLabel, 1 );
  itk::ImageFileWriter<LabelImageType>::Pointer lWriter = itk::ImageFileWriter<LabelImageType>::New();
 
  lWriter->SetInput( wmOpen );
  sprintf( memFileName, "%s-wmhard.mha", fileNameStem.c_str() );
  lWriter->SetFileName( memFileName );
  lWriter->Update();

  return EXIT_SUCCESS;
}


