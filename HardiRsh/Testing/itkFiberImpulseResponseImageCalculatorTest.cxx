
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>
#include "itkImage.h"
#include "itkDiffusionTensor3D.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNumericTraits.h"
#include "itkImageMaskSpatialObject.h"

#include "itkFiberImpulseResponseImageCalculator.h"


template <typename FilterType>
void
addGradImage(FilterType *filter)
{
  const unsigned int numberOfGradientImages = 65; // The bbl set!!!

  //Make 3 Signals
  unsigned int sig1[numberOfGradientImages] =
  {
    5000,2744,2744,102,1910,60,1268,374,1733,1509,2305,1091,292,84,1068,65,2119,879,79,1950,892,2309,363,2129,412,11,2190,2234,
   22,219,2219,75,24,20,19,2714,2219,2673,2626,417,142,639,206,492,459,486,18,1871,1665,117,27,2743,379,584,47,86,892,236,
   1169,9,956,99,16,1464,2633
  };

  unsigned int sig2[numberOfGradientImages] =
  {
    5000,2744,9,248,97,573,1872,2496,20,2454,47,46,284,1158,208,2577,585,2719,517,14,1087,49,2402,2013,83,2243,15,343,1333,130,
   1018,2019,2442,2190,2492,1839,2159,30,290,1062,2735,558,137,1163,58,362,1447,77,830,456,1142,178,193,182,627,990,66,1938,
   32,2653,27,1625,2434,333,14
  };

  unsigned int sig3[numberOfGradientImages] =
  {
    5000,2744,1376,175,1004,316,1570,1435,876,1982,1176,568,288,621,638,1321,1352,1799,298,982,990,1179,1382,2071,248,1127,1102,
   1288,678,174,1618,1047,1233,1105,1256,2276,2189,1352,1458,740,1438,598,172,828,258,424,732,974,1248,286,584,1460,286,383,
   337,538,479,1087,600,1331,492,862,1225,898,1324
  };
  typedef typename FilterType::GradientImagesType          GradImageType;
  typedef typename GradImageType::Pointer                  GradientImagePointer;
  typedef typename GradImageType::RegionType               GradientRegionType;
  typedef typename GradientRegionType::IndexType           GradientIndexType;
  typedef typename GradientRegionType::SizeType            GradientSizeType;
  typedef typename GradImageType::PixelType                GradPixelType;

  GradientImagePointer gradientImage = GradImageType::New();
  
  gradientImage->SetVectorLength(numberOfGradientImages);
  
  GradientSizeType  sizeGradientImage  = {{ 1, 1, 3 }};
  GradientIndexType indexGradientImage = {{ 0, 0, 0 }};
  GradientRegionType     regionGradientImage;
  regionGradientImage.SetSize(  sizeGradientImage );
  regionGradientImage.SetIndex( indexGradientImage);
  gradientImage->SetRegions( regionGradientImage );
  gradientImage->Allocate();

  itk::ImageRegionIteratorWithIndex< GradImageType > git(
        gradientImage, regionGradientImage );

  git.GoToBegin();

  /// lets fill in the signal where we want to...
  ///Odf0 in {0,0,0}
  GradPixelType pix(numberOfGradientImages);
  pix.Fill(0);
  for (unsigned int i=0;i<numberOfGradientImages;++i)
    pix[i] = sig1[i];
  indexGradientImage[0] = 0;
  indexGradientImage[1] = 0;
  indexGradientImage[2] = 0;
  gradientImage->SetPixel(indexGradientImage,pix);

  ///Odf1 in {0,0,1}
  indexGradientImage[0] = 0;
  indexGradientImage[1] = 0;
  indexGradientImage[2] = 1;
  pix.Fill(0);
  for (unsigned int i=0;i<numberOfGradientImages;++i)
    pix[i] = sig2[i];
  gradientImage->SetPixel(indexGradientImage,pix);

  ///Odf2 in {0,0,2}
  indexGradientImage[0] = 0;
  indexGradientImage[1] = 0;
  indexGradientImage[2] = 2;
  pix.Fill(0);
  for (unsigned int i=0;i<numberOfGradientImages;++i)
    pix[i] = sig3[i];
  gradientImage->SetPixel(indexGradientImage,pix);

  //Set up Gradient Contatiner...
  typedef typename FilterType::GradientDirectionContainerType
                                          GradientDirectionContainerType;

  typedef typename FilterType::GradientDirectionType
                                          GradientDirectionType;
  
  typename GradientDirectionContainerType::Pointer gradContainer = GradientDirectionContainerType::New();

  GradientDirectionType dir;
  double  gradientDirections[numberOfGradientImages][3] =
  {
    {0.000000,0.000000,0.000000},
    {1.000000,0.000000,0.000000},
    {0.000000,1.000000,0.000000},
    {-0.026007,0.649170,0.760199},
    {0.591136,-0.766176,0.252058},
    {-0.236071,-0.524158,0.818247},
    {-0.893021,-0.259006,0.368008},
    {0.796184,0.129030,0.591137},
    {0.233964,0.929855,0.283956},
    {0.935686,0.139953,0.323891},
    {0.505827,-0.844710,-0.174940},
    {0.346220,-0.847539,-0.402256},
    {0.456968,-0.630956,-0.626956},
    {-0.486997,-0.388997,0.781995},
    {-0.617845,0.672831,0.406898},
    {-0.576984,-0.104997,-0.809978},
    {-0.826695,-0.520808,0.212921},
    {0.893712,-0.039987,-0.446856},
    {0.290101,-0.541189,-0.789276},
    {0.115951,-0.962591,-0.244896},
    {-0.800182,0.403092,-0.444101},
    {0.513981,0.839970,0.173994},
    {-0.788548,0.152912,-0.595659},
    {0.949280,-0.233069,0.211062},
    {0.232964,0.782880,0.576911},
    {-0.020999,-0.187990,-0.981946},
    {0.216932,-0.955701,0.198938},
    {0.774003,-0.604002,0.190001},
    {-0.160928,0.355840,0.920587},
    {-0.147035,0.731173,-0.666158},
    {0.888141,0.417066,0.193031},
    {-0.561971,0.231988,-0.793959},
    {-0.380809,0.142928,0.913541},
    {-0.306000,-0.199000,-0.931001},
    {-0.332086,-0.130034,0.934243},
    {-0.963226,-0.265062,0.044010},
    {-0.959501,0.205107,0.193101},
    {0.452965,-0.888932,0.067995},
    {-0.773133,0.628108,0.088015},
    {0.709082,0.408047,0.575066},
    {-0.692769,0.023992,0.720760},
    {0.681659,0.528735,-0.505747},
    {-0.141995,-0.724976,0.673978},
    {-0.740168,0.388088,0.549125},
    {-0.103006,0.822044,0.560030},
    {0.584037,-0.596038,0.551035},
    {-0.088008,-0.335031,0.938088},
    {-0.552263,-0.792377,0.259123},
    {0.838158,-0.458086,-0.296056},
    {0.362995,-0.560993,0.743990},
    {-0.184062,0.392133,-0.901306},
    {-0.720938,-0.692941,0.008999},
    {0.433101,0.682159,-0.589137},
    {0.502114,0.690157,0.521119},
    {-0.170944,-0.508833,-0.843722},
    {0.462968,0.422971,0.778946},
    {0.385030,-0.809064,0.444035},
    {-0.713102,-0.247035,0.656094},
    {0.259923,0.884737,-0.386885},
    {0.001000,0.077002,-0.997030},
    {0.037002,-0.902057,0.430027},
    {0.570320,-0.303170,-0.763428},
    {-0.282105,0.145054,-0.948354},
    {0.721098,0.608082,0.332045},
    {0.266985,0.959945,-0.084995}
  };
  
  for (unsigned int g = 0; g<numberOfGradientImages;++g)
  {
    dir[0] = gradientDirections[g][0];
    dir[1] = gradientDirections[g][1];
    dir[2] = gradientDirections[g][2];
    gradContainer->InsertElement(g,dir);
  }

  filter->SetGradientImage( gradContainer, gradientImage );
}


template <typename FilterType>
void
addMask(FilterType *filter,unsigned int i)
{
  //We need to make a mask.
  typedef itk::Image< unsigned char , 3 >   ImageMaskType;
  typename ImageMaskType::Pointer maskImage = ImageMaskType::New();
  typedef ImageMaskType::RegionType MaskRegionType;
  typedef MaskRegionType::IndexType MaskIndexType;
  typedef MaskRegionType::SizeType  MaskSizeType;
  MaskSizeType  sizeMaskImage  = {{ 1, 1, 3 }};
  MaskIndexType indexMaskImage = {{ 0, 0, 0 }};
  MaskRegionType     regionMaskImage;
  regionMaskImage.SetSize(  sizeMaskImage );
  regionMaskImage.SetIndex( indexMaskImage);
  maskImage->SetRegions( regionMaskImage );
  maskImage->Allocate();
  maskImage->FillBuffer( static_cast< ImageMaskType::InternalPixelType >( 0 ) );

  MaskIndexType index;
  ImageMaskType::InternalPixelType one = static_cast< ImageMaskType::InternalPixelType >(1);

  if ( i<3 )
  {
    index[0] = 0; index[1] = 0; index[2] = i;
    maskImage->SetPixel(index, one);
  }

  typedef itk::ImageMaskSpatialObject< 3 >   MaskType;
  MaskType::Pointer  spatialObjectMask = MaskType::New();

  spatialObjectMask->SetImage( maskImage );
  filter->SetImageMask( spatialObjectMask );
}

int itkPrintTests(int, char* [] )
{
  std::cout << "itkFiberImpulseResponseImageCalculator Init/Print Test:";
  
  typedef unsigned int    GradientPixelType;
                                              
  typedef itk::FiberImpulseResponseImageCalculator<GradientPixelType>
                                              CaluculatorType;

  CaluculatorType::Pointer firFilter = CaluculatorType::New();
  std::cout << firFilter << std::endl;
  
  std::cout << "itkFiberImpulseResponseImageCalculator Init/Print Test: PASSED" << std::endl;
  return EXIT_SUCCESS; 
}

int itkFirTest(int, char* [] )
{

  typedef unsigned int    GradientPixelType;
                                        
  typedef itk::SymRealSphericalHarmonicRep<double,4>
                                            RSHPixelType;      
  typedef itk::FiberImpulseResponseImageCalculator<GradientPixelType,RSHPixelType>
                                              CaluculatorType;


  CaluculatorType::Pointer firFilter = CaluculatorType::New();

  //Attach the grads and signal!
  addGradImage<CaluculatorType>(firFilter);
  firFilter->SetBValue(3000.0);

  //Test each Pixel!
  
  for (unsigned int i=0;i<3;++i)
  {
    addMask<CaluculatorType>(firFilter,i);
    firFilter->Compute();
  }
  
  return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{

  if ( itkPrintTests(argc,argv) == EXIT_FAILURE )
  {
    std::cout << "Init/Print Test Failed!"<<std::endl;
    return EXIT_FAILURE;
  }
  
  if ( itkFirTest(argc,argv) == EXIT_FAILURE )
  {
    std::cout << "FIR Failed!"<<std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
