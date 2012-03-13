#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

//#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
//#include "itkHessian3DToVesselnessMeasureImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkHessian3DToNeedleImageFilter.h"
#include "itkSymmetricSecondRankTensor.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
//#include "itkMultiplyByConstantImageFilter.h"

#include "itkPluginUtilities.h"
#include "NeedleDetectionCLP.h"

// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace {

template<class T> int DoIt( int argc, char * argv[], T )
{
  PARSE_ARGS;

  const     unsigned int        Dimension       = 3;

  typedef   T                   FileInputPixelType;
  typedef   float               InternalPixelType;
  typedef   float               OutputPixelType;
  
  typedef   itk::Image< FileInputPixelType, Dimension > FileInputImageType;
  typedef   itk::Image< InternalPixelType, Dimension >  InternalImageType;
  typedef   itk::Image< OutputPixelType, Dimension >    OutputImageType;

  //typedef   itk::CastImageFilter< FileInputImageType, InternalImageType > CastFilterType;
  typedef   itk::SmoothingRecursiveGaussianImageFilter< FileInputImageType, InternalImageType >  SmoothingFilterType;

  typedef   itk::HessianRecursiveGaussianImageFilter< 
                            InternalImageType >              HessianFilterType;
  //typedef   itk::Hessian3DToVesselnessMeasureImageFilter<
  //            OutputPixelType > VesselnessMeasureFilterType;

  typedef   itk::Hessian3DToNeedleImageFilter< InternalPixelType > NeedleFilterType;

  typedef   itk::ImageFileReader< FileInputImageType >  ReaderType;
  //typedef   itk::ImageFileWriter< OutputImageType > WriterType;
  typedef   itk::ImageFileWriter< InternalImageType > WriterType;
  
  HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
  NeedleFilterType::Pointer needleFilter = NeedleFilterType::New();

  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputVolume.c_str() );
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputVolume.c_str() );

  //typename CastFilterType::Pointer cast = CastFilterType::New();
  //cast->SetInput( reader->GetOutput() );
  typename SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
  smoothing->SetInput( reader->GetOutput() );
  smoothing->SetSigma( static_cast< double >(sigma1) );

  hessianFilter->SetInput( smoothing->GetOutput() );
  hessianFilter->SetSigma( static_cast< double >(sigma2) );

  //vesselnessFilter->SetInput( hessianFilter->GetOutput() );
  needleFilter->SetInput( hessianFilter->GetOutput() );
  //writer->SetInput( vesselnessFilter->GetOutput() );
  writer->SetInput( needleFilter->GetOutput() );
  writer->SetUseCompression(1);

  needleFilter->SetAlpha1( static_cast< double >(alpha1));
  needleFilter->SetAlpha2( static_cast< double >(alpha2));

  needleFilter->SetAngleThreshold (static_cast< double >(anglethreshold) );
  needleFilter->SetNormal (static_cast< double >(normal[0]),
                           static_cast< double >(normal[1]),
                           static_cast< double >(normal[2]));

  writer->Update();

  return EXIT_SUCCESS;
}

} // end of anonymous namespace


int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  itk::ImageIOBase::IOPixelType pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  try
    {
    itk::GetImageType (inputVolume, pixelType, componentType);

    // This filter handles all types on input, but only produces
    // signed types
    switch (componentType)
      {
      case itk::ImageIOBase::UCHAR:
        return DoIt( argc, argv, static_cast<unsigned char>(0));
        break;
      case itk::ImageIOBase::CHAR:
        return DoIt( argc, argv, static_cast<char>(0));
        break;
      case itk::ImageIOBase::USHORT:
        return DoIt( argc, argv, static_cast<unsigned short>(0));
        break;
      case itk::ImageIOBase::SHORT:
        return DoIt( argc, argv, static_cast<short>(0));
        break;
      case itk::ImageIOBase::UINT:
        return DoIt( argc, argv, static_cast<unsigned int>(0));
        break;
      case itk::ImageIOBase::INT:
        return DoIt( argc, argv, static_cast<int>(0));
        break;
      case itk::ImageIOBase::ULONG:
        return DoIt( argc, argv, static_cast<unsigned long>(0));
        break;
      case itk::ImageIOBase::LONG:
        return DoIt( argc, argv, static_cast<long>(0));
        break;
      case itk::ImageIOBase::FLOAT:
        return DoIt( argc, argv, static_cast<float>(0));
        break;
      case itk::ImageIOBase::DOUBLE:
        return DoIt( argc, argv, static_cast<double>(0));
        break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cout << "unknown component type" << std::endl;
        break;
      }
    }

  catch( itk::ExceptionObject &excep)
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
