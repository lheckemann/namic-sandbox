#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImage.h"
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryThresholdImageFunction.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWeightedAddImageFilter.h"
#include "itkImage.h"
#include "itkTimeProbesCollectorBase.h"
#include <math.h>



#include "vtkXMLPolyDataReader.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataPointSampler.h"
#include "vtkPoints.h"


typedef itk::OrientedImage< unsigned char, 3 > ImageType;




ImageType::Pointer BinaryErodeFilter3D ( ImageType::Pointer & img , unsigned int ballsize )
{
 typedef itk::BinaryBallStructuringElement<unsigned char, 3> KernalType;
 typedef itk::BinaryErodeImageFilter<ImageType, ImageType, KernalType> ErodeFilterType;
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

ImageType::Pointer BinaryDilateFilter3D ( ImageType::Pointer & img , unsigned int ballsize )
{
 typedef itk::BinaryBallStructuringElement<unsigned char, 3> KernalType;
 typedef itk::BinaryDilateImageFilter<ImageType, ImageType, KernalType> DilateFilterType;
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

ImageType::Pointer BinaryOpeningFilter3D ( ImageType::Pointer & img , unsigned int ballsize )
{
 ImageType::Pointer imgErode = BinaryErodeFilter3D( img, ballsize );
 return BinaryDilateFilter3D( imgErode, ballsize );
}

ImageType::Pointer BinaryClosingFilter3D ( ImageType::Pointer & img , unsigned int ballsize )
{
 ImageType::Pointer imgDilate = BinaryDilateFilter3D( img, ballsize );
 return BinaryErodeFilter3D( imgDilate, ballsize );
}

main(int argc, char* argv[])
{
  
 if(argc < 3 )
 {
  std::cout << " Usage: <filter> <inputLabelMap> <outputLabelMap>" << std::endl;
  return EXIT_FAILURE;
 }


 typedef   float           PixelType;
 const     unsigned int    Dimension = 3;

 typedef itk::Point< double, ImageType::ImageDimension > PointType;

 typedef unsigned char     OutputPixelType;
 typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

 typedef itk::ImageRegionIterator< ImageType > IteratorType;

 typedef  itk::ImageFileReader< ImageType > ReaderType;
 typedef  itk::ImageFileWriter< ImageType > WriterType;
 ReaderType::Pointer reader1 = ReaderType::New();
 WriterType::Pointer writer = WriterType::New();

 reader1->SetFileName( argv[1] );
 writer->SetFileName( argv[2] );

 reader1->Update();


 // this is how to call the function.
 int ballSize = atoi( argv[3] );
        ImageType::Pointer img = reader1->GetOutput(); 
 ImageType::Pointer openingLabel = BinaryOpeningFilter3D( img, ballSize );
 
 writer->SetInput( openingLabel );
 writer->Update();

}
