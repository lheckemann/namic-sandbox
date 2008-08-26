#include "itkImageRegionIteratorWithIndex.h"
#include <iostream>
#include "sbiaOrientationDistributionFunction.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkNiftiImageIO.h"
#include "itkImageRegionIterator.h"

int testOdfReadWrite(int, char*[])
{
  typedef double                                                        OdfPrecisionType;
  typedef itk::OrientationDistributionFunction<OdfPrecisionType,4>      OdfPixelType;
  typedef itk::Image<OdfPixelType,3>                                    OdfImageType;
  typedef OdfImageType::RegionType                                      OdfRegionType;
  typedef OdfImageType::IndexType                                       OdfIndexType;
  typedef OdfImageType::SizeType                                        OdfSizeType;
  typedef OdfImageType::IndexType                                       OdfIndexType;
  double precision        = 0.01; //Pretty high.

  OdfImageType::Pointer odfImage = OdfImageType::New();
  OdfSizeType  sizeOdfImage  = {{ 3, 3, 3 }};
  OdfIndexType indexOdfImage = {{ 0, 0, 0 }};
  OdfRegionType     regionOdfImage;
  regionOdfImage.SetSize(  sizeOdfImage );
  regionOdfImage.SetIndex( indexOdfImage);
  odfImage->SetRegions( regionOdfImage );
  odfImage->Allocate();

  ///-----------------------------------------------------------------------------------  
  ///make an image....
  /// make an odf at each pixel which has the same
  typedef itk::ImageRegionIterator<OdfImageType>      IteratorType;
  IteratorType it( odfImage  , odfImage->GetLargestPossibleRegion() );

  OdfPrecisionType counter = 0.5;
  for ( it.GoToBegin(); !it.IsAtEnd(); ++it, ++counter )
  {
    OdfPixelType inpOdf(counter);
    it.Set(inpOdf);
  }
  
  bool passed = true;
  ///---------------------------------------------------------------------------------------
  typedef itk::ImageFileWriter< OdfImageType >      OdfWriterType;
//  typedef itk::NiftiImageIO                         niiIOType;
//  niiIOType::Pointer niiIOwrite = niiIOType::New();
//    niiIO->SetLegacyAnalyze75Mode(true);
  OdfWriterType::Pointer niiWriter = OdfWriterType::New();
//  niiWriter->SetImageIO(niiIOwrite);
  niiWriter->SetFileName("/tmp/niiTest_odf.nhdr");
  niiWriter->SetInput(odfImage);
  niiWriter->Update();
    
  typedef itk::ImageFileReader< OdfImageType >     OdfReaderType;
  OdfReaderType::Pointer niiReader = OdfReaderType::New();
//  niiIOType::Pointer niiIOread = niiIOType::New();
//  niiReader->SetImageIO(niiIOread);
  niiReader->SetFileName("/tmp/niiTest_odf.nhdr");
  OdfImageType::Pointer readOdfIm = niiReader->GetOutput();
  niiReader->Update();
       
  typedef itk::ImageRegionConstIterator<OdfImageType>   ConstIteratorType;
  ConstIteratorType it_orig( odfImage  , odfImage->GetLargestPossibleRegion() );
  ConstIteratorType it_read( readOdfIm , odfImage->GetLargestPossibleRegion() );
  
  for ( it_orig.GoToBegin(), it_read.GoToBegin(); !it_orig.IsAtEnd(); ++it_read,++it_orig )
  {
    std::cout << "Pixel : " << it_orig.GetIndex() << std::endl;

    for (unsigned int i=0;i<OdfPixelType::InternalDimension;i++)
    {
      std::cout << "index " << i << " : " << (it_orig.Get())[i] << " - " <<
          (it_read.Get())[i] << " = " << 
          ((it_orig.Get())[i] - (it_read.Get())[i]) << std::endl;
      
      if (fabs((it_orig.Get())[i] - (it_read.Get())[i]) > precision)
        passed = false;
    }
    std::cout << "\n";   
  }
  
  return EXIT_SUCCESS;
}

int main( int argc, char * argv[])
{
  return testOdfReadWrite(argc,argv);
}

