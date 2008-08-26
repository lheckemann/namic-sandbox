#include "sbiaOdfReconImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include <iostream>
#include "sbiaOrientationDistributionFunction.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkFixedArray.h"
#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkNiftiImageIO.h"
#include "itkSymmetricSecondRankTensor.h"

int testOdfImFilter(int, char*[])
{
  typedef unsigned int    ReferencePixelType;
  typedef unsigned int    GradientPixelType;
  typedef double          OdfPrecisionType;

  typedef itk::OdfReconImageFilter<
    ReferencePixelType, 
    GradientPixelType, 
    OdfPrecisionType , 4>      OdfReconstructionImageFilterType;

  typedef OdfReconstructionImageFilterType::GradientImageType GradientImageType;

  OdfReconstructionImageFilterType::Pointer odfReconstructionFilter =
    OdfReconstructionImageFilterType::New();

  //Basic parameters...
  //OdfPrecisionType BValue = 1000.0;

  const OdfPrecisionType S0     = 5000.0;
  const double precision        = 0.01; //Pretty high.

  //
  // Create a reference image  (B0 image)...
  //
  typedef OdfReconstructionImageFilterType::ReferenceImageType ReferenceImageType;
  ReferenceImageType::Pointer referenceImage = ReferenceImageType::New();
  typedef ReferenceImageType::RegionType ReferenceRegionType;
  typedef ReferenceRegionType::IndexType ReferenceIndexType;
  typedef ReferenceRegionType::SizeType  ReferenceSizeType;
  ReferenceSizeType  sizeReferenceImage  = {{ 3, 3, 3 }};
  ReferenceIndexType indexReferenceImage = {{ 0, 0, 0 }};
  ReferenceRegionType     regionReferenceImage;
  regionReferenceImage.SetSize(  sizeReferenceImage );
  regionReferenceImage.SetIndex( indexReferenceImage);
  referenceImage->SetRegions( regionReferenceImage );
  referenceImage->Allocate();
  referenceImage->FillBuffer( static_cast< ReferencePixelType >( S0 ) );

  //
  // Assign gradient directions
  //
  const unsigned int numberOfGradientImages = 64; // The bbl set!!!

  double  gradientDirections[numberOfGradientImages][3] =
  {
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

  //Make 4 Odfs to check....
      typedef OdfReconstructionImageFilterType::OdfPixelType OdfPixelType;

      OdfPrecisionType c1[15] = {0.282095, -0.0000560343, 0.000232044, 0.117268, 0.000319191, 
        -0.000774259, 0.000815502, 0.000120621, -0.0000963574, -0.000399541, 
        0.0455435, -0.000598414, -0.000123727, 0.000180633, 0.000567137
      };

      OdfPrecisionType c2[15] = {0.282095, -0.0505294, -0.00024962, 0.0296457, 5.12645E-6,
        0.000167141, 0.0173116, 0.0000183914, 0.012648, -0.000188034, 
            0.0313058, -0.000345037, 0.0000244151, 0.0000660799, 0.000292462
      };

      OdfPrecisionType c3[15] = {0.282095, 7.31844E-6, -0.000270581, 0.0000879588, 0.000108169,
        -0.0000788369, 0.0228569, 0.0000142504, -0.000028806, -0.0000898873, 
            0.0267168, -0.000307354, 6.79119E-6, 0.000182789, -0.0000287987
      };

      OdfPixelType Odf1(c1);
      OdfPixelType Odf2(c2);
      OdfPixelType Odf3(c3);

      double sig1[numberOfGradientImages] =
      {
        2744, 2744, 102, 1910, 60, 1268, 374, 1733, 1509, 2305, 1091, 292, 
            84, 1068, 65, 2119, 879, 79, 1950, 892, 2309, 363, 2129, 412, 11, 
                2190, 2234, 22, 219, 2219, 75, 24, 20, 19, 2714, 2219, 2673, 2626, 
                    417, 142, 639, 206, 492, 459, 486, 18, 1871, 1665, 117, 27, 2743, 
                        379, 584, 47, 86, 892, 236, 1169, 9, 956, 99, 16, 1464, 2633
      };
      
      double sig2[numberOfGradientImages] =
      {
        2744, 1377, 175, 1004, 317, 1570, 1435, 876, 1982, 1176, 568, 288, 
            621, 638, 1321, 1352, 1799, 298, 982, 989, 1179, 1382, 2071, 248, 
                1127, 1102, 1288, 678, 175, 1619, 1047, 1233, 1105, 1255, 2276, 2189, 
                    1352, 1458, 739, 1439, 598, 172, 827, 259, 424, 733, 974, 1247, 287, 
                        584, 1460, 286, 383, 337, 538, 479, 1087, 600, 1331, 491, 862, 1225, 
                            899, 1324
      };
      
      double sig3[numberOfGradientImages] =
      {
        1832, 1832, 1028, 794, 877, 1056, 981, 1254, 1327, 997, 841, 470, 
            651, 529, 1018, 920, 1209, 765, 1502, 683, 989, 948, 1386, 836, 1664, 
                1434, 889, 1241, 925, 1089, 849, 1222, 1273, 1325, 1522, 1464, 1185, 
                    1002, 545, 1018, 463, 930, 592, 1034, 414, 1364, 810, 848, 623, 1144, 
                        1021, 505, 472, 999, 628, 712, 775, 1023, 1802, 1235, 718, 1398, 646, 
                            1492
      };
      
  //
  // Create gradient images
  //

      typedef GradientImageType::Pointer GradientImagePointer;
      typedef OdfReconstructionImageFilterType::GradientImageType GradientImageType;
      typedef GradientImageType::RegionType  GradientRegionType;
      typedef GradientRegionType::IndexType  GradientIndexType;
      typedef GradientRegionType::SizeType   GradientSizeType;
      typedef ReferenceRegionType::IndexType ReferenceIndexType;

      typedef OdfReconstructionImageFilterType::OdfImageType OdfImageType;
      typedef OdfImageType::IndexType OdfImageIndexType;

      for ( unsigned int i=0; i < numberOfGradientImages; i++ )
      {
        GradientImageType::Pointer gradientImage = GradientImageType::New();
        GradientSizeType  sizeGradientImage  = {{ 3, 3, 3 }};
        GradientIndexType indexGradientImage = {{ 0, 0, 0 }};
        GradientRegionType     regionGradientImage;
        regionGradientImage.SetSize(  sizeGradientImage );
        regionGradientImage.SetIndex( indexGradientImage);
        gradientImage->SetRegions( regionGradientImage );
        gradientImage->Allocate();

        OdfReconstructionImageFilterType::GradientDirectionType gradientDirection;
        gradientDirection[0] = gradientDirections[i][0];
        gradientDirection[1] = gradientDirections[i][1];
        gradientDirection[2] = gradientDirections[i][2];

        itk::ImageRegionIteratorWithIndex< GradientImageType > git(
            gradientImage, regionGradientImage );
        git.GoToBegin();
        GradientPixelType fancyGradientValue = 0;
        while ( !git.IsAtEnd() )
        {
          git.Set( fancyGradientValue );
          ++git;
        }

      /// lets fill in the signal where we want to...
      ///Odf0 in {0,0,0}
        indexGradientImage[0] = 0;
        indexGradientImage[1] = 0;
        indexGradientImage[2] = 0;
        fancyGradientValue = static_cast< GradientPixelType >(sig1[i] );
        gradientImage->SetPixel(indexGradientImage,fancyGradientValue);

      ///Odf1 in {1,1,1}
        indexGradientImage[0] = 1;
        indexGradientImage[1] = 1;
        indexGradientImage[2] = 1;
        fancyGradientValue = static_cast< GradientPixelType >(sig2[i] );
        gradientImage->SetPixel(indexGradientImage,fancyGradientValue);

      ///Odf2 in {2,2,2}
        indexGradientImage[0] = 2;
        indexGradientImage[1] = 2;
        indexGradientImage[2] = 2;
        fancyGradientValue = static_cast< GradientPixelType >(sig3[i] );
        gradientImage->SetPixel(indexGradientImage,fancyGradientValue);

      ///Done computing our things
        odfReconstructionFilter->AddGradientImage( gradientDirection, gradientImage );
//    std::cout << "Gradient directions: " << gradientDirection << std::endl;
      }

      odfReconstructionFilter->SetReferenceImage( referenceImage );
  // TODO: remove this when netlib is made thread safe
      odfReconstructionFilter->SetNumberOfThreads( 1 );

  // Also see if vnl_svd is thread safe now...
      std::cout << std::endl << "This filter is using " <<
          odfReconstructionFilter->GetNumberOfThreads() << " threads " << std::endl;

      odfReconstructionFilter->Update();

      OdfImageType::Pointer odfImage = odfReconstructionFilter->GetOutput();

  ///OK lets check the pixels we set explicitly..
      OdfImageIndexType    odfImageIndex;
      GradientIndexType    gradientImageIndex;
      ReferenceIndexType   referenceImageIndex;

      for (int i=0;i<1;i++)
      {
        odfImageIndex[0] = i;
        odfImageIndex[1] = i;
        odfImageIndex[2] = i;
        gradientImageIndex[0] = i;
        gradientImageIndex[1] = i;
        gradientImageIndex[2] = i;
        referenceImageIndex[0] = i;
        referenceImageIndex[1] = i;
        referenceImageIndex[2] = i;

        std::cout << std::endl << "Pixels at index: " << odfImageIndex << std::endl;
        std::cout << "Reference pixel "
            << referenceImage->GetPixel( referenceImageIndex ) << std::endl;

        for ( unsigned int i=0; i < numberOfGradientImages; i++ )
        {
          std::cout << "Gradient image " << i << " pixel : "
              << static_cast< GradientImageType * >( const_cast< GradientImageType * >(
              odfReconstructionFilter->GetInput(i+1)))->GetPixel(gradientImageIndex)
              << std::endl;
        }
        std::cerr << odfImage->GetPixel(odfImageIndex) << std::endl;
      }
      
      bool passed = true;

      OdfPixelType testOdf;
      
      odfImageIndex[0] = 0; odfImageIndex[1] = 0; odfImageIndex[2] = 0;
      testOdf = odfImage->GetPixel(odfImageIndex);
      std::cout << "testing Odf1" << std::endl;
      for (unsigned int i=0;i<OdfPixelType::InternalDimension;i++)
      {
        std::cout << "index " << i << " : " << testOdf[i] << std::endl;
        if (fabs(testOdf[i] - Odf1[i]) > precision)
          passed = false;
      }
      
      odfImageIndex[0] = 1; odfImageIndex[1] = 1; odfImageIndex[2] = 1;
      testOdf = odfImage->GetPixel(odfImageIndex);
      std::cout << "testing Odf2" << std::endl;
      for (unsigned int i=0;i<OdfPixelType::InternalDimension;i++)
      {
        std::cout << "index " << i << " : " << testOdf[i] << std::endl;
        if (fabs(testOdf[i] - Odf2[i]) > precision)
          passed = false;
      }
      odfImageIndex[0] = 2; odfImageIndex[1] = 2; odfImageIndex[2] = 2;
      testOdf = odfImage->GetPixel(odfImageIndex);
      std::cout << "testing Odf3" << std::endl;
      for (unsigned int i=0;i<OdfPixelType::InternalDimension;i++)
      {
        std::cout << "index " << i << " : " << testOdf[i] << std::endl;
        if (fabs(testOdf[i] - Odf3[i]) > precision)
          passed = false;
      }

    if (!passed){
      std::cerr << "[FAILED]" << std::endl;
      return EXIT_FAILURE;
    }
    
    std::cerr << "[PASSED]" << std::endl;
    return EXIT_SUCCESS;
      
}

int main( int argc, char * argv[])
{
  return testOdfImFilter(argc,argv);
}
