//  author: Bryn Lloyd, Computational Radiology Laboratory, 2005
//  submitted to MICCAI open source workshop, 2005
//

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include "itkSymmetricSecondRankTensor.h"
#include "itkRescaleIntensityImageFilter.h"


#include "itkDiadicProductOfGradientRecursiveGaussianImageFilter.h"

/** I had to write this file, because itkMeanImageFilter uses the ZERO member 
 *   of the NumericTraits, which is not defined for SymmetricSecondRankTensors. 
 *  I also added the code for the itkNumericTraitsTensorPixel, how it should look 
 *  to make my code work with the normal itkMeanImageFilter */
#include "itkTensorMeanImageFilter.h"

#include "itkDeterminantImageFilter.h"
#include "itkLocalMaximumImageFilter.h"


#include "PointSetIO.h"
#include "itkPointSet.h"


/********************************************************************************
 *
 * This program implements a method to extract point landmarks. It calculates the 
 * determinant of a correlation matrix estimate at each pixel. The maxima are selected as salient points.
 * 
 * See Karl Rohr, "On 3d differential operators for detecting point landmarks", 
 * Image and Vision Computing, 1997
 *
 ********************************************************************************/


int main( int argc, char * argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << "./cornerfeature" << "  inputImageFile  landmarksFile  outputImageFile " << std::endl
              << "   [Variance=1.0]  [radiusMean=1]  [radiusMax=1]  [thresholdMax1=3.0]  [thresholdMax2=0.0023]" << std::endl;
    return 1;
    }


  const char* inputImageFile  = argv[1];
  const char* landmarksFile   = argv[2];   /** the set of salient points is saved in a ASCII file */
  const char* outputImageFile = argv[3];  /** binar image, where 1 is a salient point (and 0 is not) */

  float sigma = 1.0;
  if (argc>4)
    { sigma = std::sqrt(atof(argv[4])); }

  unsigned int radiusMean = 1;
  if (argc>5)
    { radiusMean = atoi(argv[5]); }

  unsigned int radiusMax = 1;
  if (argc>6)
    { radiusMax = atoi(argv[6]); }

  float thresholdMax1 = 3.0;
  if (argc>7)
    { thresholdMax1 = atof(argv[7]); }

  float thresholdMax2 = 0.5;
  if (argc>8)
    { thresholdMax2 = atof(argv[8]); }


  /** standard typedefs for the image types */
  const unsigned int Dimension = 3;
  typedef   double  ValueType;
  typedef   ValueType                 InputPixelType;
  typedef   itk::SymmetricSecondRankTensor<ValueType, Dimension> TensorPixelType;

  typedef itk::Image< ValueType,   Dimension >        InputImageType;
  typedef itk::Image< unsigned int,   Dimension >     MaskImageType;
  typedef itk::Image< TensorPixelType,  Dimension >   TensorImageType;

  
  /** Image reader */
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
   reader->SetFileName( inputImageFile );

  
  /** we need this to rescale the images before we detect salient points */
  typedef itk::RescaleIntensityImageFilter< InputImageType,
                                     InputImageType > RescalerType;
  RescalerType::Pointer rescaler = RescalerType::New();
   rescaler->SetInput( reader->GetOutput() );
   rescaler->SetOutputMinimum(0.0);
   rescaler->SetOutputMaximum(10.0);


  /** this filter calculates the diadic product (outer product) of the gradient of the image. 
   *   It is implemented using the itkRecursiveGaussianImageFilter. The mean of this 
   *   matrix-valued image in a neighborhood around each pixel is an estimate for the 
   *   correlation matrix.*/
  typedef itk::DiadicProductOfGradientRecursiveGaussianImageFilter<InputImageType, TensorImageType> FeatureFilterType;
  FeatureFilterType::Pointer featureFilter = FeatureFilterType::New();
   featureFilter->SetInput( rescaler->GetOutput() );
   featureFilter->SetSigma( sigma );
   featureFilter->SetNormalizeAcrossScale(true);

  typedef itk::TensorMeanImageFilter <ValueType, Dimension, Dimension> MeanFilterType;
  MeanFilterType::Pointer meanfilter = MeanFilterType::New();
   meanfilter->SetInput( featureFilter->GetOutput() );
   itk::Size<Dimension> meanradius;
   for (unsigned int k=0; k<Dimension; k++)
     meanradius[k] = radiusMean;
   meanfilter->SetRadius(meanradius);

   
  /** this is filter, which takes a image with SymmetricSecondRankTensor pixel type as input 
   *   and gives a scalar image as output. It calculates the determinant from the correlation 
   *  matrix. The result is a "cornerness measure" . 
   *  The implementation is based on a unitary function image filter*/
  typedef itk::DeterminantImageFilter<TensorImageType,InputImageType> FeatureExtractorType;
  FeatureExtractorType::Pointer featureExtractor = FeatureExtractorType::New();
   featureExtractor->SetInput( meanfilter->GetOutput() );

//  typedef itk::DeterminantOverTraceNImageFilter<TensorImageType,InputImageType> Feature2ExtractorType;
//  Feature2ExtractorType::Pointer feature2Extractor = Feature2ExtractorType::New();
//   feature2Extractor->SetInput( meanfilter->GetOutput() );


  featureExtractor->Update();
  std::cout << "calculated cornerness image" << std::endl;


  
  /** typdef for PointSet type. The local maxima will be calculated and put into a pointset. */
  typedef itk::PointSet< InputPixelType,  Dimension>    OutputPointsetType;

  /** This filter extracts the local maxima in an image. It looks for points, where the intensity 
   *   is larger than the intensity at all neighboring points. The neighborhood is defined by a radius.
   *   The filter is derived from an image to mesh filter, so the GetOutput() method returns a point set. */
  typedef itk::LocalMaximumImageFilter <InputImageType,OutputPointsetType> LocalMaxFilterType;
  LocalMaxFilterType::Pointer localMaxfilter = LocalMaxFilterType::New();
   localMaxfilter->SetInput( featureExtractor->GetOutput() );
   itk::Size<Dimension> radius;
   for (unsigned int k=0; k<Dimension; k++)
     radius[k] = radiusMax;
   localMaxfilter->SetRadius( radius );
   localMaxfilter->SetThreshold( thresholdMax1 );


  localMaxfilter->Update();
  std::cout << "calculated local maxima" << std::endl;


  /** writer typedef and instantiation*/
  typedef itk::ImageFileWriter< InputImageType >      WriterType;
  WriterType::Pointer writer = WriterType::New();
   writer->SetInput( localMaxfilter->GetLocalMaximaImage() );
   writer->SetFileName( outputImageFile );
   writer->Update();

  typedef itk::PointSetIO<OutputPointsetType>  PointSetIOType;
  PointSetIOType::Pointer pswriter = PointSetIOType::New();

  pswriter->WritePointSet( localMaxfilter->GetOutput(), landmarksFile );

  return 0;
}

