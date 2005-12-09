//  author: Bryn Lloyd, Computational Radiology Laboratory, 2005
//  submitted to MICCAI open source workshop, 2005
//

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include "itkVector.h"
#include "itkRescaleIntensityImageFilter.h"


#include "itkKJetRecursiveGaussianImageFilter.h"

#include "itkGaussianCurvatureFromKJetImageFilter.h"
#include "itkLocalMaximumImageFilter.h"


#include "PointSetIO.h"
#include "itkPointSet.h"


/**
 *
 * This program implements a method to extract point landmarks. It calculates the 
 * a 2 jet at each pixel. From these first and second order derivatives it calculates 
 * the estimate of the gaussian (isointensity) curvature.
 * 
 * See J.P. Thirion et al., "Computing the differential characteristics of isointensity surfaces", 
 * Computer Vision  and Image Understanding, 1995
 *
 */


int main( int argc, char * argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << "./cornerfeature" << "  inputImageFile  landmarksFile  outputImageFile " << std::endl
              << "   [Variance=1.0]  [radiusMean=1]  [radiusMax=1]  [thresholdMax1=0.5]" << std::endl;
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

  float thresholdMax1 = 0.5;
  if (argc>7)
    { thresholdMax1 = atof(argv[7]); }



  /** standard typedefs for the image types */
  const unsigned int Dimension = 3;
  typedef   double  ValueType;
  typedef   ValueType                 InputPixelType;
  typedef   itk::Vector<ValueType, 9> VectorPixelType;

  typedef itk::Image< ValueType,   Dimension >        InputImageType;
  typedef itk::Image< VectorPixelType,  Dimension >   VectorImageType;

  
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


  /** this filter calculates the 2-Jet of the gradient of the image. 
   *   It is implemented using the itkRecursiveGaussianImageFilter. */
  typedef itk::KJetRecursiveGaussianImageFilter<InputImageType> KJetFilterType;
  KJetFilterType::Pointer kjetFilter = KJetFilterType::New();
   kjetFilter->SetInput( rescaler->GetOutput() );
   kjetFilter->SetSigma( sigma );
   kjetFilter->SetNormalizeAcrossScale(true);


   
  /** this is filter, which takes a image with Vector pixel type as input 
   *   and gives a scalar image as output. It calculates the Gaussian curvature from the derivatives. 
   *  The implementation is based on a unitary function image filter*/
  typedef itk::GaussianCurvatureFromKJetImageFilter <VectorImageType,InputImageType> GaussianCurvatureFilterType;
  GaussianCurvatureFilterType::Pointer featureExtractor = GaussianCurvatureFilterType::New();
   featureExtractor->SetInput( kjetFilter->GetOutput() );


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

