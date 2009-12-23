/*
 *      SymRSHPowerImageFilterText1.cxx
 *
 *      Copyright 2009 Luke Bloy <lbloy@Sirus>
 */


#include <iostream>
#include "itkSymRSHPowerImageFilter.h"
#include "itkSymRealSphericalHarmonicRep.h"

template <typename TOdfPercisionType>
int InitTest(int argc, char ** argv)
{

  typedef TOdfPercisionType                                                        OdfPrecisionType;
//  typedef float                                               OdfPrecisionType;
  const unsigned int OdfOrder = 4;

  typedef itk::SymRealSphericalHarmonicRep<OdfPrecisionType,OdfOrder>      OdfPixelType;
  typedef itk::Image<OdfPixelType,3>                                    OdfImageType;

  typedef itk::SymRSHPowerImageFilter<OdfImageType> FilterType;

  //type to instantiate the filter;
  typename FilterType::Pointer filter = FilterType::New();
  
  return EXIT_SUCCESS;
}

template <typename TOdfPercisionType>
int ComputeTest(int argc, char ** argv)
{

  typedef TOdfPercisionType                           OdfPrecisionType;
  const unsigned int OdfOrder = 4;

  typedef itk::SymRealSphericalHarmonicRep<OdfPrecisionType,OdfOrder>
                                                      OdfPixelType;
                                                      
  typedef itk::Image<OdfPixelType,3>                  OdfImageType;
  typedef itk::SymRSHPowerImageFilter<OdfImageType>   FilterType;
  typedef typename FilterType::OutputImageType        OutputImageType;

  //Create an Odf Image...
  typedef typename OdfImageType::RegionType           OdfRegionType;
  typedef typename OdfImageType::IndexType            OdfIndexType;
  typedef typename OdfImageType::SizeType             OdfSizeType;
  typedef typename OdfImageType::IndexType            OdfIndexType;

  typename OdfImageType::Pointer odfImage = OdfImageType::New();
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

  OdfPrecisionType counter = 1;
  for ( it.GoToBegin(); !it.IsAtEnd(); ++it, ++counter )
  {
    OdfPixelType inpOdf(counter);
    it.Set(inpOdf);
    ++counter;
  }

  //type to instantiate the filter;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(odfImage);
  
  std::cerr << filter << std::endl;
  
  filter->Update();
  
  typename OutputImageType::Pointer output = filter->GetOutput();
  
  return EXIT_SUCCESS;
}



int main(int argc, char** argv)
{
  if (InitTest<float>(argc,argv) != EXIT_SUCCESS)
    return EXIT_FAILURE;
  if (ComputeTest<float>(argc,argv) != EXIT_SUCCESS)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
