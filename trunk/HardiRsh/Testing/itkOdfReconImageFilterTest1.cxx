#include <iostream>

#include "itkSymRealSphericalHarmonicRep.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkFixedArray.h"
#include "itkCastImageFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkImage.h"
#include "itkOdfReconImageFilter.h"

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
addMask(FilterType *filter)
{
  //We need to make a mask.
  typedef itk::Image< unsigned char , 3 >   ImageMaskType;
  ImageMaskType::Pointer maskImage = ImageMaskType::New();
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
  maskImage->FillBuffer( static_cast< ImageMaskType::InternalPixelType >( 1 ) );

  typedef itk::ImageMaskSpatialObject< 3 >   MaskType;
  MaskType::Pointer  spatialObjectMask = MaskType::New();

  spatialObjectMask->SetImage( maskImage );
  filter->SetImageMask( spatialObjectMask );
}


int OdfReconImTest1(int,char*[])
{
  //Just try to instantiate the filter...

  typedef unsigned int    ReferencePixelType;
  typedef unsigned int    GradientPixelType;
  typedef double          FopdfPrecisionType;
  typedef itk::SymRealSphericalHarmonicRep< FopdfPrecisionType, 4 > FopdfPixelType;

  typedef itk::OdfReconImageFilter<
    GradientPixelType,
    FopdfPixelType , 3 >      FopdfReconImageFilterType;

  try
  {
    FopdfReconImageFilterType::Pointer symReconFilter =
    FopdfReconImageFilterType::New();

    std::cerr << symReconFilter << std::endl;
    return EXIT_SUCCESS;
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

}

int OdfReconImTest2(int, char*[])
{

  //Test with no laplace Beltrami smoothing ( filter default).
  
  typedef float           PrecisionType;

  PrecisionType c1[64] = {0.2820947917738782,0.0000662022700076336,-0.000014659220255996013,0.11649020787092487,-0.00005906765826131736,
   -0.00015567324165590235,0.00010327863765311019,-6.98602199761233e-6,0.00003381000008347576,-0.0000608209221409902,
   0.04613392768076604,0.00004753205257045622,-0.000053484305892485514,-0.000020293482161009812,0.00015095620864381313,
   -0.00014599753155196076,-0.000054540661389365614,0.00008790841411744113,-0.000013369832355424572,0.000019498836726350768,
   0.00017807464515361287,0.015317723114115106,-0.00012577904094155564,-0.000018886634055525106,0.00003237598596226608,
   0.00015647498134700773,-0.00010415953282093508,-0.00016365397403072694,0.00038533691786267704,-8.650195493572214e-6,
   -0.00007169584578448682,0.00008246281963320314,0.0001340679266918295,-0.0001369781501467667,0.00010680563907940152,
   0.00015897473856044033,0.004084258470181101,-0.00016078764547737448,-0.00019442117745608763,-0.0002210463306432133,
   0.00020556337083901225,-0.00011481445987961134,-0.00014606063794853557,-0.00001649297960101878,-0.00004043087889889071
  };

  PrecisionType c2[64] = {0.2820947917738782,-0.10108104916900795,0.00005646747999565575,-0.058258233602363464,-7.549467661683423e-7,
   0.00007527046242088532,0.034067682494092526,-0.000011555596763501444,0.02586433947193797,4.834851583279102e-6,
   0.01724897905984826,-0.00004898998382817548,0.00001646670419524772,-0.0000638753019633644,-0.00001903028794095315,
   -0.010302080312204113,0.000085862509947628,-0.00765696331181104,-0.000017329733230897398,-0.0069814081337033805,
   0.000025520634801754564,-0.004865941468215873,0.000010591599271643838,0.00001111024872518543,-0.00017529152022774763,
   -0.00005225196594140273,0.0000773426615009902,-0.00006324642555393927,0.002719129560463265,-0.00004431750419950821,
   0.001981282231718484,-0.00028746112982699527,0.0016579260686297016,-0.00009180781980140274,0.0015346204595941305,
   -0.000044972380362769326,0.0011031287299952135,0.00016205050452870824,0.0002421174385272168,0.00006727759108494908,
   0.0001228072895959255,-0.00016339407339663914,-0.00007638129324593982,-0.00027844006886820077,-0.00012336342240513044
  };

  PrecisionType c3[64] = {0.2820947917738782,-0.05051175550059886,0.000020095232615881408,0.029117486561759307,-0.00002267370930510597,
   -0.0000335577596812752,0.01708557552244859,-9.414553128895146e-6,0.012953316927144258,-0.00003086079377268195,
   0.03169278418944491,-9.210848366538869e-6,-0.000013111709597421226,-0.000047631932231300535,0.00006095382901198086,
   -0.005229413549233157,0.000012433659097135326,-0.0037821927480013932,-0.000015936152879570637,-0.003480165656653003,
   0.00010504898581506816,0.005221368815970859,-0.000058466367074404244,-4.043796006010325e-6,-0.00007330146750357761,
   0.00005491224024070811,-0.000011592675509332798,-0.00011768467773303483,0.0015502005433233363,-0.000023782029126604012,
   0.0009460043232425837,-0.00010488423082716348,0.0008934882745600267,-0.00010982160170514248,0.0008287953174218732,
   0.00005769616440012497,0.002593052726984353,1.12356284422675e-6,0.000019145910741340606,-0.00007097389630139754,
   0.00016392016130426738,-0.00013546999272521688,-0.0001070594046020907,-0.0001481829850597436,-0.00008206293153824486
  };

  typedef unsigned int    GradientPixelType;
  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, 8 >
                                                          PixelType;


  typedef itk::OdfReconImageFilter<
    GradientPixelType,
    PixelType,     3 >                                    ReconImageFilterType;


  ReconImageFilterType::Pointer reconFilter =
                                                    ReconImageFilterType::New();

  //Basic parameters...
  const double precision     = 0.0001;

  //Attach the grads and signal!
  addGradImage<ReconImageFilterType>(reconFilter);

  //Attach a mask
  addMask<ReconImageFilterType>(reconFilter);
  
  //Make 3 RSHs to check....
  typedef ReconImageFilterType::OutputPixelType OutputPixelType;

  OutputPixelType pix1(c1);
  OutputPixelType pix2(c2);
  OutputPixelType pix3(c3);

  // Also see if vnl_svd is thread safe now...
  std::cout << std::endl << "This filter is using " <<
    reconFilter->GetNumberOfThreads() << " threads " << std::endl;

  try
  {
    reconFilter->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught on filter update!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  typedef ReconImageFilterType::OutputImageType           OutputImageType;
  typedef OutputImageType::IndexType                      OutputImageIndexType;
  
  OutputImageType::Pointer outputImage = reconFilter->GetOutput();
 ///OK lets check the pixels we set explicitly..
  OutputImageIndexType    outputImageIndex;

  bool passed = true;

  OutputPixelType testPixel;

  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 0;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel1" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix1[i],fabs(testPixel[i] - pix1[i]));
    if (fabs(testPixel[i] - pix1[i]) > precision)
      passed = false;
  }

  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 1;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel2" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix2[i],fabs(testPixel[i] - pix2[i]));
    if (fabs(testPixel[i] - pix2[i]) > precision)
      passed = false;
  }
  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 2;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel3" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix3[i],fabs(testPixel[i] - pix3[i]));
    if (fabs(testPixel[i] - pix3[i]) > precision)
      passed = false;
  }

  if (!passed){
    std::cerr << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }

  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}

int OdfReconImTest3(int, char*[])
{
  
  //Test with a Bltrami-smoothing of 0.006
  
  typedef float           PrecisionType;

  PrecisionType c1[64] = {0.28209479177387814,0.00001616573642127397,-0.00011077701849402299,0.11147051203953025,0.00007456494520774856,
   -0.0005721361732166056,0.0003957678660086435,-0.00027314006151611187,0.00016218247792343707,-0.00005584332123662979,
   0.031244413351904757,-0.00009201033288739562,-0.00018722868299270317,0.00012380520129391174,0.0006936598612923346,
   -0.0001947734494502836,-0.0001465079139641947,0.00012015479335029567,-0.000049314177256934136,0.00015631152378323707,
   -0.0000579341756806078,0.0047624987152312765,-0.00010243659428753903,-0.000013814645110538177,-3.84933911443029e-6,
   0.00037338850186737757,0.00019315228602135028,-0.00024221550679947006,0.000264426514414173,-0.000010689697201192773,
   -0.0000789556165487314,-0.00009949781831236534,0.00001435622708201489,-0.00002732817311792041,0.000023647217349188817,
   -0.000019107712765457535,0.0004937054832980204,-0.000027318823151706867,-3.3999411898103336e-6,-0.000029613609934715116,
   0.00012004408731478815,0.000010500046904996755,-0.00012397450289947936,-0.000013713525070531576,-0.00006393082575708339
  };

  PrecisionType c2[64] = {0.2820947917738782,-0.09700049901273557,-0.0008333982605429987,-0.05579023766909326,-0.00025171285651172154,
   0.0002555776577201779,0.022998871408375723,-0.0001309657772630873,0.017356951857997138,0.00016200483237388603,
   0.0119382058524138,-4.614998712614455e-6,0.00007159953446250015,-0.000028458064796479652,-0.00018085609238549787,
   -0.003114095105608433,0.0001222108530188019,-0.002383859978754757,0.0000962691565465757,-0.002265264159000443,
   0.00012887381727259893,-0.001510907404309538,8.944550526601956e-6,0.0002037208152216763,0.00008146595244851667,
   -0.000069678531823951,0.0000503263689689456,0.0001492166732843454,0.00022756356873651612,-0.00009974826978105334,
   0.0002792751683569632,0.000018142050958757454,0.00022573412282176546,-0.00005692203123806219,0.000058684309683386874,
   -0.000039132480094607275,0.0003064791154878835,-0.00001995030043778763,0.0000749137759170151,-0.00006155124146353925,
   -0.00014663107414999296,0.0000564327375864636,0.00013006314571549282,-0.00014134459348238567,-0.000028796449640332844
  };

  PrecisionType c3[64] = {0.28209479177387814,-0.048504424490299944,-0.00047199220895327717,0.027828819250973833,-0.0000823469119752192,
   -0.0001507119295850668,0.011698969592197619,-0.00020230193317310937,0.008762709152605788,0.00005113247700077914,
   0.021590837069111163,-0.00005431595750742186,-0.00005358631810829558,0.00004361960880031902,0.0002525194212526876,
   -0.0016560358958121809,-0.000013265673498445112,-0.0011315998012024907,0.000022917326199486775,-0.0010546196694896563,
   0.00003660454031792783,0.0016240351325422378,-0.000046913216131270016,0.0000950468846695291,0.000038294218581863035,
   0.00015262840395047965,0.00012217848128107166,-0.00004756775964369436,0.00024556580986170797,-0.00005479971602778104,
   0.00009902903241268502,-0.000041062880738696784,0.00011981945858268689,-0.000041553579641518784,0.000042197506764572345,
   -0.000028999571508644718,0.0003999309013144311,-0.000023705890813824416,0.00003521360437735358,-0.00004472212349916598,
   -0.00001324463027018321,0.00003388102464807434,3.7372513712464236e-6,-0.00007770907313702878,-0.00004657746900438242
  };


  typedef unsigned int    GradientPixelType;
  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, 8 >
                                                          PixelType;


  typedef itk::OdfReconImageFilter<
    GradientPixelType,
    PixelType,     3 >                                    ReconImageFilterType;


  ReconImageFilterType::Pointer reconFilter =
                                                    ReconImageFilterType::New();

  reconFilter->SetBeltramiLambda(0.006);
  if (reconFilter->GetBeltramiLambda() != 0.006)
  {
    std::cerr << "Beltrami setting failed!" << std::endl;
    return EXIT_FAILURE;
  }
  
  //Basic parameters...
  const double precision     = 0.0001;

  //Attach the grads and signal!
  addGradImage<ReconImageFilterType>(reconFilter);

  //Attach a mask
  addMask<ReconImageFilterType>(reconFilter);
  
  //Make 3 RSHs to check....
  typedef ReconImageFilterType::OutputPixelType OutputPixelType;

  OutputPixelType pix1(c1);
  OutputPixelType pix2(c2);
  OutputPixelType pix3(c3);

  // Also see if vnl_svd is thread safe now...
  std::cout << std::endl << "This filter is using " <<
    reconFilter->GetNumberOfThreads() << " threads " << std::endl;

  try
  {
    reconFilter->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught on filter update!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  typedef ReconImageFilterType::OutputImageType           OutputImageType;
  typedef OutputImageType::IndexType                      OutputImageIndexType;
  
  OutputImageType::Pointer outputImage = reconFilter->GetOutput();
 ///OK lets check the pixels we set explicitly..
  OutputImageIndexType    outputImageIndex;

  bool passed = true;

  OutputPixelType testPixel;

  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 0;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel1" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix1[i],fabs(testPixel[i] - pix1[i]));
    if (fabs(testPixel[i] - pix1[i]) > precision)
      passed = false;
  }

  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 1;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel2" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix2[i],fabs(testPixel[i] - pix2[i]));
    if (fabs(testPixel[i] - pix2[i]) > precision)
      passed = false;
  }
  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 2;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel3" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix3[i],fabs(testPixel[i] - pix3[i]));
    if (fabs(testPixel[i] - pix3[i]) > precision)
      passed = false;
  }

  if (!passed){
    std::cerr << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }

  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}

int OdfReconImTest4(int, char*[])
{
  
  //Test with a Bltrami-smoothing of 0.006 and no normalization
  
  typedef float           PrecisionType;

  PrecisionType c1[64] = {0.28209479177387814,0.00001616573642127397,-0.00011077701849402299,0.11147051203953025,0.00007456494520774856,
   -0.0005721361732166056,0.0003957678660086435,-0.00027314006151611187,0.00016218247792343707,-0.00005584332123662979,
   0.031244413351904757,-0.00009201033288739562,-0.00018722868299270317,0.00012380520129391174,0.0006936598612923346,
   -0.0001947734494502836,-0.0001465079139641947,0.00012015479335029567,-0.000049314177256934136,0.00015631152378323707,
   -0.0000579341756806078,0.0047624987152312765,-0.00010243659428753903,-0.000013814645110538177,-3.84933911443029e-6,
   0.00037338850186737757,0.00019315228602135028,-0.00024221550679947006,0.000264426514414173,-0.000010689697201192773,
   -0.0000789556165487314,-0.00009949781831236534,0.00001435622708201489,-0.00002732817311792041,0.000023647217349188817,
   -0.000019107712765457535,0.0004937054832980204,-0.000027318823151706867,-3.3999411898103336e-6,-0.000029613609934715116,
   0.00012004408731478815,0.000010500046904996755,-0.00012397450289947936,-0.000013713525070531576,-0.00006393082575708339
  };

  PrecisionType c2[64] = {0.2820947917738782,-0.09700049901273557,-0.0008333982605429987,-0.05579023766909326,-0.00025171285651172154,
   0.0002555776577201779,0.022998871408375723,-0.0001309657772630873,0.017356951857997138,0.00016200483237388603,
   0.0119382058524138,-4.614998712614455e-6,0.00007159953446250015,-0.000028458064796479652,-0.00018085609238549787,
   -0.003114095105608433,0.0001222108530188019,-0.002383859978754757,0.0000962691565465757,-0.002265264159000443,
   0.00012887381727259893,-0.001510907404309538,8.944550526601956e-6,0.0002037208152216763,0.00008146595244851667,
   -0.000069678531823951,0.0000503263689689456,0.0001492166732843454,0.00022756356873651612,-0.00009974826978105334,
   0.0002792751683569632,0.000018142050958757454,0.00022573412282176546,-0.00005692203123806219,0.000058684309683386874,
   -0.000039132480094607275,0.0003064791154878835,-0.00001995030043778763,0.0000749137759170151,-0.00006155124146353925,
   -0.00014663107414999296,0.0000564327375864636,0.00013006314571549282,-0.00014134459348238567,-0.000028796449640332844
  };

  PrecisionType c3[64] = {0.28209479177387814,-0.048504424490299944,-0.00047199220895327717,0.027828819250973833,-0.0000823469119752192,
   -0.0001507119295850668,0.011698969592197619,-0.00020230193317310937,0.008762709152605788,0.00005113247700077914,
   0.021590837069111163,-0.00005431595750742186,-0.00005358631810829558,0.00004361960880031902,0.0002525194212526876,
   -0.0016560358958121809,-0.000013265673498445112,-0.0011315998012024907,0.000022917326199486775,-0.0010546196694896563,
   0.00003660454031792783,0.0016240351325422378,-0.000046913216131270016,0.0000950468846695291,0.000038294218581863035,
   0.00015262840395047965,0.00012217848128107166,-0.00004756775964369436,0.00024556580986170797,-0.00005479971602778104,
   0.00009902903241268502,-0.000041062880738696784,0.00011981945858268689,-0.000041553579641518784,0.000042197506764572345,
   -0.000028999571508644718,0.0003999309013144311,-0.000023705890813824416,0.00003521360437735358,-0.00004472212349916598,
   -0.00001324463027018321,0.00003388102464807434,3.7372513712464236e-6,-0.00007770907313702878,-0.00004657746900438242
  };


  typedef unsigned int    GradientPixelType;
  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, 8 >
                                                          PixelType;


  typedef itk::OdfReconImageFilter<
    GradientPixelType,
    PixelType,     3 >                                    ReconImageFilterType;


  ReconImageFilterType::Pointer reconFilter =
                                                    ReconImageFilterType::New();

  reconFilter->SetBeltramiLambda(0.006);
  reconFilter->NormalizeOff();
  //Basic parameters...
  const double precision     = 0.0001;

  //Attach the grads and signal!
  addGradImage<ReconImageFilterType>(reconFilter);

  //Attach a mask
  addMask<ReconImageFilterType>(reconFilter);
  
  //Make 3 RSHs to check....
  typedef ReconImageFilterType::OutputPixelType OutputPixelType;

  OutputPixelType pix1(c1);
  OutputPixelType pix2(c2);
  OutputPixelType pix3(c3);

  // Also see if vnl_svd is thread safe now...
  std::cout << std::endl << "This filter is using " <<
    reconFilter->GetNumberOfThreads() << " threads " << std::endl;

  try
  {
    reconFilter->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught on filter update!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  typedef ReconImageFilterType::OutputImageType           OutputImageType;
  typedef OutputImageType::IndexType                      OutputImageIndexType;
  
  OutputImageType::Pointer outputImage = reconFilter->GetOutput();
 ///OK lets check the pixels we set explicitly..
  OutputImageIndexType    outputImageIndex;

  bool passed = true;

  OutputPixelType testPixel;

  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 0;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel1" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix1[i],fabs(testPixel[i] - pix1[i]));
    if (fabs(testPixel[i] - pix1[i]) > precision)
      passed = false;
  }

  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 1;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel2" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix2[i],fabs(testPixel[i] - pix2[i]));
    if (fabs(testPixel[i] - pix2[i]) > precision)
      passed = false;
  }
  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 2;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel3" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix3[i],fabs(testPixel[i] - pix3[i]));
    if (fabs(testPixel[i] - pix3[i]) > precision)
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
  if ( OdfReconImTest1(argc,argv) == EXIT_FAILURE )
    return EXIT_FAILURE;

  if ( OdfReconImTest2(argc,argv) == EXIT_FAILURE )
    return EXIT_FAILURE;

  if ( OdfReconImTest3(argc,argv) == EXIT_FAILURE )
    return EXIT_FAILURE;

  //~ if ( OdfReconImTest4(argc,argv) == EXIT_FAILURE )
    //~ return EXIT_FAILURE;

  return EXIT_SUCCESS;

}
