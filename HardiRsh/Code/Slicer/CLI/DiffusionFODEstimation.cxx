#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include <iostream>
#include <algorithm>
#include <string>

#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkImageSeedConnectivity.h"
#include "vtkImageConnectivity.h"
#include "vtkITKNewOtsuThresholdImageFilter.h"

#include "DiffusionFODEstimationCLP.h"

#include "itkVectorImage.h"
#include "itkImage.h"
#include "itkNrrdImageIO.h"
#include "itkMetaDataDictionary.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkVariableLengthVectorCastImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkNewOtsuThresholdImageFilter.h"
#include "itkImageMaskSpatialObject.h"


#include "itkVotingBinaryHoleFillFloodingImageFilter.h"

#include "itkFodCsdReconImageFilter.h"
#include "itkSymRealSphericalHarmonicRep.h"
#include "itkFiberImpulseResponseImageCalculator.h"

template <unsigned int order>
int runner(
  std::string inputVolume,
  std::string outputODF,
  std::string outputBaseline,
  std::string thresholdMask,
  double beltramiLambda,
  double faThreshold,
  double otsuOmegaThreshold,
  bool removeIslands,
  bool applyMask )
{
  unsigned int numberOfImages = 0;
  unsigned int numberOfGradientImages = 0;
  bool readb0 = false;
  double b0 = 0;

  typedef short                                   GradientPixelType;
  typedef double                                  PrecisionType;

  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, order >
                                                  RSHPixelType;

  //defined fod and fir filters
  typedef itk::FodCsdReconImageFilter<
    GradientPixelType,
    RSHPixelType>                                 FodReconFilterType;
    
  typedef itk::FiberImpulseResponseImageCalculator
    <GradientPixelType,
    RSHPixelType>                                           FirCalculatorType;
  

  typedef typename FodReconFilterType::GradientImagesType   GradientImagesType;
  typedef itk::Image<GradientPixelType,3>                   GradientImageType;
  typedef typename FodReconFilterType::OutputImageType      RSHImageType;

  typename itk::ImageFileReader<GradientImagesType>::Pointer reader 
    = itk::ImageFileReader<GradientImagesType>::New();
  
  typename GradientImagesType::Pointer gradientImg;
  
  // Set the properties for NrrdReader
  reader->SetFileName(inputVolume.c_str());

  // Read in the nrrd data. The file contains the reference image and the gradient
  // images.
  try
  {
    reader->Update();
    gradientImg = reader->GetOutput();
  }
  catch (itk::ExceptionObject &ex)
  {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
  }
  
  // -------------------------------------------------------------------------
  // Parse the Nrrd headers to get the B value and the gradient directions used
  // for diffusion weighting. 
  // 
  // The Nrrd headers should look like :
  // The tags specify the B value and the gradient directions. If gradient 
  // directions are (0,0,0), it indicates that it is a reference image. 
  //
  // DWMRI_b-value:=800
  // DWMRI_gradient_0000:= 0 0 0
  // DWMRI_gradient_0001:=-1.000000       0.000000        0.000000
  // DWMRI_gradient_0002:=-0.166000       0.986000        0.000000
  // DWMRI_gradient_0003:=0.110000        0.664000        0.740000
  // ...
  // 
  itk::MetaDataDictionary imgMetaDictionary = gradientImg->GetMetaDataDictionary();
  std::vector<std::string> imgMetaKeys = imgMetaDictionary.GetKeys();
  typename std::vector<std::string>::const_iterator itKey = imgMetaKeys.begin();
  std::string metaString;
  
  typename FodReconFilterType::GradientDirectionType vect3d;
  typename FodReconFilterType::GradientDirectionContainerType::Pointer 
  DiffusionVectors = FodReconFilterType::GradientDirectionContainerType::New();

  std::vector<int> b0Indexes;
  
  for (; itKey != imgMetaKeys.end(); itKey ++)
  {
    double x,y,z;

    itk::ExposeMetaData<std::string> (imgMetaDictionary, *itKey, metaString);
    if (itKey->find("DWMRI_gradient") != std::string::npos)
    { 
      sscanf(metaString.c_str(), "%lf %lf %lf\n", &x, &y, &z);
      vect3d[0] = x; vect3d[1] = y; vect3d[2] = z;

      DiffusionVectors->InsertElement( numberOfImages, vect3d );
      ++numberOfImages;
      // If the direction is 0.0, this is a reference image
      if (vect3d[0] == 0.0 &&
          vect3d[1] == 0.0 &&
          vect3d[2] == 0.0)
      {
        b0Indexes.push_back(  numberOfGradientImages );
        continue;
      }
      ++numberOfGradientImages;
    }
    else if (itKey->find("DWMRI_b-value") != std::string::npos)
    {
      readb0 = true;
      b0 = atof(metaString.c_str());
    }
  }
  
  if(!readb0)
  {
    std::cerr << "BValue not specified in header file" << std::endl;
    return EXIT_FAILURE;
  }

  //Compute the average b0 image eventually
  //now just use the first one!
  typedef itk::VectorIndexSelectionCastImageFilter
      <GradientImagesType,GradientImageType> VectorIndexSelectionFilterType;
  
  typename VectorIndexSelectionFilterType::Pointer extract = VectorIndexSelectionFilterType::New();
  extract->SetInput(gradientImg);

  // image is not b0 image
  extract->SetIndex( b0Indexes.front() );

  GradientImageType::Pointer b0Image = extract->GetOutput();
  
  //Threshold the b0Image
  typedef itk::Image< unsigned char , 3 >   ImageMaskType;

  typedef itk::ImageMaskSpatialObject< 3 >   MaskType;
  typename MaskType::Pointer  spatialObjectMask = MaskType::New();

  typedef itk::NewOtsuThresholdImageFilter<GradientImageType,ImageMaskType>
                                              OtsuFilterType;
  
  typename OtsuFilterType::Pointer otsu = OtsuFilterType::New();

  otsu->SetInput(b0Image);
  otsu->SetOmega (1 + otsuOmegaThreshold);
  otsu->SetOutsideValue(1);
  otsu->SetInsideValue(0);
  otsu->Update();
  
  ImageMaskType::Pointer imageMask;

  //THIS ISN"T WORKING very quickely!!!
  if ( removeIslands )
  {
    typedef itk::VotingBinaryHoleFillFloodingImageFilter
            <ImageMaskType,ImageMaskType>         HoleFillingFilterType;
            
    typename HoleFillingFilterType::Pointer holeFiller = HoleFillingFilterType::New();
    
    holeFiller->SetInput( otsu->GetOutput() );
    holeFiller->Update();
    imageMask = holeFiller->GetOutput();
  }
  else
  {
    imageMask = otsu->GetOutput();
  }
  spatialObjectMask->SetImage( imageMask );

  typename FirCalculatorType::Pointer firFilter =  FirCalculatorType::New();
    
  if (applyMask)
  {
    firFilter->SetImageMask( spatialObjectMask );
  }
  firFilter->SetFAThreshold( faThreshold );
  firFilter->SetGradientImage( DiffusionVectors, gradientImg);
  firFilter->SetBValue(b0);

  firFilter->Compute();
  typename FirCalculatorType::VectorType firRsh = firFilter->GetRespRSH();
  
  std::cerr << firRsh << std::endl;
  
  
  //Configure the filter
  typename FodReconFilterType::Pointer fodReconFilter = FodReconFilterType::New();

  fodReconFilter->SetGradientImage( DiffusionVectors, gradientImg);
  
  fodReconFilter->SetCalculateResidualImage(false);
  fodReconFilter->SetNormalize(true);
  fodReconFilter->SetBeltramiLambda(beltramiLambda);
  
  if (applyMask)
  {
    fodReconFilter->SetImageMask( spatialObjectMask );
  }
  
  try
  {
    fodReconFilter->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught on filter update!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  //Writer
  typedef itk::VectorImage
    <typename RSHImageType::PixelType::ComponentType, 3>
                                                          VectorImageType;

  typedef itk::VariableLengthVectorCastImageFilter<RSHImageType,VectorImageType> CasterType;
  typedef itk::ImageFileWriter< VectorImageType > OdfWriterType;

  typename OdfWriterType::Pointer odfWriter = OdfWriterType::New();
  odfWriter->SetFileName( outputODF.c_str() );

  //We need to cast the odf output to vectorImage.
  typename CasterType::Pointer caster = CasterType::New();
  caster->SetInput(fodReconFilter->GetOutput() );
  caster->Update();
  
  //TODO attach meta data (after removing measurement frame!!!)
  odfWriter->SetInput( caster->GetOutput() );
  odfWriter->Update();
  
  //Write out B0image
  typedef itk::ImageFileWriter< GradientImageType > B0WriterType;
  typename B0WriterType::Pointer b0Writer = B0WriterType::New();
  b0Writer->SetFileName( outputBaseline.c_str() );
  b0Writer->SetInput( b0Image );
  b0Writer->Update();
  
  //Write out maskFile
  typedef itk::ImageFileWriter< ImageMaskType > MaskWriterType;
  typename MaskWriterType::Pointer maskWriter = MaskWriterType::New();
  maskWriter->SetFileName( thresholdMask.c_str() );
  maskWriter->SetInput( imageMask );
  maskWriter->Update();

  return EXIT_SUCCESS;
}

int main( int argc, char * argv[] )
{

  PARSE_ARGS;
  try {
    switch ( order )
    {
      case 2:
        return runner<2>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, faThreshold, otsuOmegaThreshold, removeIslands, applyMask );
      case 4:
        return runner<4>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, faThreshold, otsuOmegaThreshold,removeIslands, applyMask );
      case 6:
        return runner<6>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, faThreshold, otsuOmegaThreshold,removeIslands, applyMask );
      case 8:
        return runner<8>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, faThreshold, otsuOmegaThreshold,removeIslands, applyMask );
      case 10:
        return runner<10>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, faThreshold, otsuOmegaThreshold,removeIslands, applyMask );
      case 12:
        return runner<12>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, faThreshold, otsuOmegaThreshold,removeIslands, applyMask );
      case 14:
        return runner<14>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, faThreshold, otsuOmegaThreshold,removeIslands, applyMask );
      case 16:
        return runner<16>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, faThreshold, otsuOmegaThreshold,removeIslands, applyMask );
      case 18:
        return runner<18>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, faThreshold, otsuOmegaThreshold,removeIslands, applyMask );
      case 20:
        return runner<20>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, faThreshold, otsuOmegaThreshold,removeIslands, applyMask );
      default:
        std::cout << "Unsupported dimension" << std::endl;
        exit( EXIT_FAILURE );
    }
  }
  catch (std::exception& e)
  {
    cout << "Standard exception: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
