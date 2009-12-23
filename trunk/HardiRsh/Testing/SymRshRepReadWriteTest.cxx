#include "itkImageRegionIteratorWithIndex.h"
#include "itkSymRealSphericalHarmonicRep.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkNiftiImageIO.h"
#include "itkImageRegionIterator.h"

#include "itkVariableLengthVectorCastImageFilter.h"
#include "itkVectorImage.h"

#include <iostream>

int testOdfReadWrite(int, char*[])
{
  typedef double                                                        OdfPrecisionType;
  const unsigned int OdfOrder = 4;

  typedef itk::SymRealSphericalHarmonicRep<OdfPrecisionType,OdfOrder>   OdfPixelType;
  typedef itk::Image<OdfPixelType,3>                                    OdfImageType;
  typedef OdfImageType::RegionType                                      OdfRegionType;
  typedef OdfImageType::IndexType                                       OdfIndexType;
  typedef OdfImageType::SizeType                                        OdfSizeType;
  typedef OdfImageType::IndexType                                       OdfIndexType;
  double precision        = 0.01; //Pretty high.

  OdfImageType::Pointer odfImage = OdfImageType::New();
  OdfSizeType  sizeOdfImage  = {{ 5, 5, 5 }};
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
    //OdfPixelType inpOdf;
    it.Set(inpOdf);
    }

  bool passed = true;
  ///---------------------------------------------------------------------------------------
  
  typedef itk::VectorImage <OdfImageType::PixelType::ComponentType, 3>
                                                            VectorImageType;

  typedef itk::VariableLengthVectorCastImageFilter<OdfImageType,VectorImageType> CasterType;
  
  typedef itk::ImageFileWriter< VectorImageType > OdfWriterType;

//  typedef itk::ImageFileWriter< OdfImageType >      OdfWriterType;

  OdfWriterType::Pointer niiWriter = OdfWriterType::New();
  CasterType::Pointer caster = CasterType::New();
  caster->SetInput(odfImage);
  
  niiWriter->SetFileName("/tmp/niiTest_odf.nii");
  niiWriter->SetInput(caster->GetOutput());
  niiWriter->Update();

  typedef itk::ImageFileReader< OdfImageType >     OdfReaderType;
  OdfReaderType::Pointer niiReader = OdfReaderType::New();
  niiReader->SetFileName("/tmp/niiTest_odf.nii");
  OdfImageType::Pointer readOdfIm = niiReader->GetOutput();
  niiReader->Update();

  typedef itk::ImageRegionConstIterator<OdfImageType>   ConstIteratorType;
  ConstIteratorType it_orig( odfImage  , odfImage->GetLargestPossibleRegion() );
  ConstIteratorType it_read( readOdfIm , odfImage->GetLargestPossibleRegion() );

  for ( it_orig.GoToBegin(), it_read.GoToBegin(); !it_orig.IsAtEnd(); ++it_read,++it_orig )
    {
    std::cout << "Pixel : " << it_orig.GetIndex() << std::endl;

    for (unsigned int i=0;i<OdfPixelType::Dimension;i++)
      {
      std::cout << "index " << i << " : " << (it_orig.Get())[i] << " - " <<
          (it_read.Get())[i] << " = " <<
          ((it_orig.Get())[i] - (it_read.Get())[i]) << std::endl;

      if (fabs((it_orig.Get())[i] - (it_read.Get())[i]) > precision)
        {
        passed = false;
        }
      }
    std::cout << "\n";
    }

  return EXIT_SUCCESS;
}

int main( int argc, char * argv[])
{
  return testOdfReadWrite(argc,argv);
}

