#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include <iostream>
#include <algorithm>
#include <string>

#include "DiffusionODFEstimationCLP.h"

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
#include "itkHoleFillingFrontPropagationLabelImageFilter.h"


#include "itkSymRealSphericalHarmonicRep.h"
#include "itkOdfReconImageFilter.h"

template <unsigned int order>
int runner(
  std::string inputVolume,
  std::string outputODF,
  std::string outputBaseline,
  std::string thresholdMask,
  double beltramiLambda,
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

  typedef itk::OdfReconImageFilter<
    GradientPixelType,
    RSHPixelType>                                 OdfReconFilterType;

  typedef typename OdfReconFilterType::GradientImagesType  GradientImagesType;
  typedef itk::Image<GradientPixelType,3>
                                                  GradientImageType;
  typedef typename OdfReconFilterType::OutputImageType     OdfImageType;

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
  
  typename OdfReconFilterType::GradientDirectionType vect3d;
  typename OdfReconFilterType::GradientDirectionContainerType::Pointer 
  DiffusionVectors = OdfReconFilterType::GradientDirectionContainerType::New();

  // Apply measurement frame if it exists
  //Adapted from dtiestim code.
  
  vnl_matrix<double> transform(3,3);
  transform.set_identity();
  
  if(imgMetaDictionary.HasKey("NRRD_measurement frame"))
  {
    std::cout << "Reorienting gradient directions to image coordinate frame" << std::endl;

    // measurement frame
    vnl_matrix<double> mf(3,3);
    // imaging frame
    vnl_matrix<double> imgf(3,3);
    std::vector<std::vector<double> > nrrdmf;
    itk::ExposeMetaData<std::vector<std::vector<double> > >(imgMetaDictionary,"NRRD_measurement frame",nrrdmf);

    imgf = gradientImg->GetDirection().GetVnlMatrix();
    std::cout << "Image frame: " << std::endl;
    std::cout << imgf << std::endl;

    for(unsigned int i = 0; i < 3; ++i)
    {
      for(unsigned int j = 0; j < 3; ++j)
      {
        mf(i,j) = nrrdmf[j][i];

        nrrdmf[j][i] = imgf(i,j);
      }
    }

    std::cout << "Meausurement frame: " << std::endl;
    std::cout << mf << std::endl;

    itk::EncapsulateMetaData<std::vector<std::vector<double> > >(imgMetaDictionary,"NRRD_measurement frame",nrrdmf);
    // prevent slicer error
    transform = vnl_svd<double>(imgf).inverse()*mf;

    std::cout << "Transform: " << std::endl;
    std::cout << transform << std::endl;

  }

  std::vector<int> b0Indexes;
  
  for (; itKey != imgMetaKeys.end(); itKey ++)
  {
    double x,y,z;

    itk::ExposeMetaData<std::string> (imgMetaDictionary, *itKey, metaString);
    if (itKey->find("DWMRI_gradient") != std::string::npos)
    { 
      sscanf(metaString.c_str(), "%lf %lf %lf\n", &x, &y, &z);
      vect3d[0] = x; vect3d[1] = y; vect3d[2] = z;
      DiffusionVectors->InsertElement( numberOfImages, transform * vect3d );
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

  if ( removeIslands )
  {

    typedef itk::HoleFillingFrontPropagationLabelImageFilter 
            <ImageMaskType,ImageMaskType>         HoleFillingFilterType;
    
    //~ typedef itk::VotingBinaryHoleFillFloodingImageFilter
            //~ <ImageMaskType,ImageMaskType>         HoleFillingFilterType;
            
    typename HoleFillingFilterType::Pointer holeFiller = HoleFillingFilterType::New();
    
    holeFiller->SetInput( otsu->GetOutput() );
    typename ImageMaskType::SizeType indexRadius;

    indexRadius[0] = 5; // radius along x
    indexRadius[1] = 5; // radius along y
    indexRadius[2] = 5; // radius along z

    holeFiller->SetRadius( indexRadius );
    holeFiller->SetBackgroundValue(  0 );
    holeFiller->SetForegroundValue(  1 );

    holeFiller->Update();
    imageMask = holeFiller->GetOutput();
  }
  else
  {
    imageMask = otsu->GetOutput();
  }
  spatialObjectMask->SetImage( imageMask );
  
  //Configure the filter
  typename OdfReconFilterType::Pointer odfReconFilter = OdfReconFilterType::New();

  odfReconFilter->SetGradientImage( DiffusionVectors, gradientImg);
  
  odfReconFilter->SetCalculateResidualImage(false);
  odfReconFilter->SetNormalize(true);
  odfReconFilter->SetBeltramiLambda(beltramiLambda);
  
  if (applyMask)
  {
    odfReconFilter->SetImageMask( spatialObjectMask );
  }
  
  try
  {
    odfReconFilter->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught on filter update!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  //Writer
  typedef itk::VectorImage
    <typename OdfImageType::PixelType::ComponentType, 3>
                                                          VectorImageType;

  typedef itk::VariableLengthVectorCastImageFilter<OdfImageType,VectorImageType> CasterType;
  typedef itk::ImageFileWriter< VectorImageType > OdfWriterType;

  typename OdfWriterType::Pointer odfWriter = OdfWriterType::New();
  odfWriter->SetFileName( outputODF.c_str() );

  //We need to cast the odf output to vectorImage.
  typename CasterType::Pointer caster = CasterType::New();
  caster->SetInput(odfReconFilter->GetOutput() );
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
        return runner<2>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, otsuOmegaThreshold,removeIslands, applyMask );
      case 4:
        return runner<4>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, otsuOmegaThreshold,removeIslands, applyMask );
      case 6:
        return runner<6>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, otsuOmegaThreshold,removeIslands, applyMask );
      case 8:
        return runner<8>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, otsuOmegaThreshold,removeIslands, applyMask );
      case 10:
        return runner<10>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, otsuOmegaThreshold,removeIslands, applyMask );
      case 12:
        return runner<12>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, otsuOmegaThreshold,removeIslands, applyMask );
      case 14:
        return runner<14>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, otsuOmegaThreshold,removeIslands, applyMask );
      case 16:
        return runner<16>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, otsuOmegaThreshold,removeIslands, applyMask );
      case 18:
        return runner<18>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, otsuOmegaThreshold,removeIslands, applyMask );
      case 20:
        return runner<20>( inputVolume, outputODF, outputBaseline, thresholdMask, beltramiLambda, otsuOmegaThreshold,removeIslands, applyMask );
      default:
        std::cout << "Unsupported dimension" << std::endl;
        exit( EXIT_FAILURE );
    }
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught on filter update!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
