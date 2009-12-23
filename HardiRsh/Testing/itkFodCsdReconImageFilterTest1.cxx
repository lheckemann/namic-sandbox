#include <iostream>

#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkFixedArray.h"
#include "itkCastImageFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkImage.h"
#include "itkFodCsdReconImageFilter.h"

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


int FodReconImTest1(int,char*[])
{
  //Just try to instantiate the filter...

  typedef unsigned int    GradientPixelType;
  typedef float           PrecisionType;
  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, 8 > RshPixelType;

  typedef itk::FodCsdReconImageFilter<
    GradientPixelType,
    RshPixelType,     3 >      FodCsdReconImageFilter;

  try
  {
    FodCsdReconImageFilter::Pointer reconFilter =
    FodCsdReconImageFilter::New();

    std::cerr << reconFilter << std::endl;
    return EXIT_SUCCESS;
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

}

int FodReconImTest2(int, char*[])
{
  //Test non CSD FOD computation
  
  typedef float           PrecisionType;

  PrecisionType c1[64] = {0.2820947917738782,0.0003584788445761752,-0.00007937825000764802,0.6307831305050401,-0.0003198456168175703,
   -0.0008429554423697799,0.0018945514016326557,-0.00012815213357013092,0.0006202132842066726,-0.0011157037496730172,
   0.8462843753216347,0.0008719316875781108,-0.0009811202877617411,-0.0003722652229515632,0.00276915249048372,
   -0.009694335425347641,-0.0036215370233156244,0.005837178506411621,-0.0008877659646464406,0.0012947360247832405,
   0.011824277592349261,1.017107236282055,-0.008351813893041333,-0.0012540853509177673,0.0021497874950885273,
   0.01039004522013564,-0.006916263844872323,-0.010866735218603346,0.10973544512277575,-0.002463384661258101,
   -0.020417393677857183,0.02348359007156689,0.038179585007885534,-0.03900835238372862,0.03041588750699463,
   0.04527249503102866,1.1631066229203195,-0.04578877088798666,-0.05536685809337068,-0.062949113779273,0.058539908724736404,
   -0.03269662281853896,-0.041594844348417176,-0.0046968363892035524,-0.011513821507897435
  };

  PrecisionType c2[64] = {0.2820787727197884,-0.5473129553341248,0.00030574854149986914,-0.3154447472358818,-4.087731074008545e-6,
   0.0004075590783406407,0.6249047425901818,-0.0002119647329164524,0.47443052232211963,0.00008868585894118524,
   0.3163986520423554,-0.0008986250602450522,0.0003020493556676508,-0.0011716669937336965,-0.0003490732658208967,
   -0.6840262964426872,0.005701005321534082,-0.5083987017623098,-0.0011506407328420957,-0.46354392558897234,
   0.0016944912849908562,-0.32308347637977886,0.0007032494606520603,0.0007376861815988771,-0.011638815243668403,
   -0.0034693690654326362,0.0051353137133706385,-0.004199367207768962,0.7743040822924554,-0.012619929891394394,
   0.5641933883914357,-0.08185793334818059,0.47211392268498914,-0.02614335509871442,0.4370012021165951,-0.01280641357132931,
   0.31412886364407305,0.04614578467261932,0.06894578462596233,0.019158084330118747,0.03497081825450005,
   -0.04652838168986621,-0.021750470456071667,-0.07928908027526076,-0.03512918360445164
  };

  PrecisionType c3[64] = {0.28207357672137984,-0.273495666738676,0.00010880554413692956,0.15765649643414908,-0.0001227666942551291,
   -0.00018169833472042842,0.31339556202384444,-0.00017268831039123702,0.2375987881193451,-0.0005660702383822523,
   0.5813311878257403,-0.00016895181533496976,-0.0002405041371289901,-0.0008736981761195323,0.0011180577133970743,
   -0.34721050346220644,0.0008255413335282923,-0.2511212846069974,-0.001058091813252975,-0.23106798848516094,
   0.0069747995467629356,0.34667636787320966,-0.0038819145887761942,-0.0002684906125549668,-0.004866901268530801,
   0.0036459358971545025,-0.0007697036507406665,-0.00781375498815981,0.44142968251557435,-0.006772087399997954,
   0.26938088098723173,-0.029866466577076655,0.25442659471971724,-0.03127241503226658,0.23600485460778178,
   0.01642935789386157,0.7383886213617967,0.00031994182413823467,0.005451922550543492,-0.020210278370860476,
   0.046677331571121784,-0.03857596111460097,-0.0304858614502114,-0.04219606832861687,-0.023367953244006045
  };


  typedef unsigned int    GradientPixelType;
  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, 8 >
                                                          PixelType;


  typedef itk::FodCsdReconImageFilter<
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
  
  //Create the response function in the RSH space
  vnl_vector<double> respRot = vnl_vector<double>(5);
  respRot[0] = 0.7212520319878115;
  respRot[1] = -0.595677776291134;
  respRot[2] = 0.31454381107471285;
  respRot[3] = -0.12532455607008042;
  respRot[4] = 0.03818977726185059;
  reconFilter->SetResponseRSH(respRot);
  
  vnl_vector<double> respFilter = vnl_vector<double>(5,1.0);
  reconFilter->SetRecipricalFilter(respFilter);

  reconFilter->UseCSDOff();
  
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

int FodReconImTest3(int, char*[])
{
  //Test CSD
  typedef float           PrecisionType;

  PrecisionType c1[91] = {0.3032552897930145 , -0.0000958407690632 , -0.0066747111268342 , 0.6533671617507935 , 0.0044652074575424 ,
   -0.0016525052487850 , 0.0003591519489419 , -0.0002463758282829 , 0.0004286793118808 , -0.0165787059813738 , 0.7151672840118408 ,
    0.0118626700714231 , -0.0049546207301319 , 0.0000691846871632 , 0.0002348797279410 , -0.0000537164087291 , -0.0000565599075344 ,
     0.0000443356620963 , -0.0002549083728809 , 0.0006879592547193 , -0.0241870768368244 , 0.6049066185951233 , 0.0185959823429585 ,
      -0.0087401252239943 , -0.0001456794125261 , 0.0000951622932917 , 0.0000412244407926 , -0.0000680209923303 , 0.0000009375322634 ,
       -0.0000109558168333 , 0.0000180805382115 , 0.0000366772983398 , -0.0000044758958211 , -0.0002615232078824 , 0.0007219212711789 ,
        -0.0256621055305004 , 0.4154595732688904 , 0.0207582209259272 , -0.0096628284081817 , -0.0001343555632047 , -0.0000406388426200 ,
         -0.0000850058422657 , -0.0000414304886363 , -0.0000036684889437 , 0.0000008298320608 , 0.0000069297611844 , -0.0000140739566632 ,
          -0.0000538910353498 , -0.0000623070882284 , 0.0000784707372077 , 0.0000815668245195 , -0.0000895435805432 , -0.0000128561296151 ,
           0.0005640331655741 , -0.0190995931625366 , 0.2231622487306595 , 0.0156886260956526 , -0.0067567047663033 , 0.0000931066460907 ,
            -0.0001234022929566 , -0.0001584473793628 , -0.0001150352181867 , -0.0000147592490976 , -0.0000051963979786 , -0.0000102332805909 ,
             0.0000223480838031 , -0.0000171194224095 , -0.0000203843846975 , 0.0000474989501527 , -0.0000744082499295 , 0.0000058660575633 ,
              -0.0001863244106062 , 0.0001439861080144 , 0.0000981534976745 , -0.0001966000272660 , 0.0003126915835310 , 0.0003220748039894 ,
               -0.0068044043146074 , 0.0815755575895309 , 0.0050438409671187 , -0.0020817185286433 , 0.0004013648722321 , -0.0001580974203534 ,
                -0.0002297804167029 , -0.0002584496105555 , -0.0000557501698495 , 0.0000256462644757 , -0.0000093493190434 , 0.0001369888632325 ,
                 -0.0001249538327102 , 0.0001059497080860
  };

  PrecisionType c2[91] = {0.3029249608516693 , -0.5678346157073975 , 0.0001642580318730 , -0.3245662152767181 ,
   0.0002242040209239 , 0.0022263473365456 , 0.5434350967407227 , -0.0017250997480005 , 0.3995838463306427 ,
    -0.0005052688065916 , 0.2646528482437134 , -0.0012808766914532 , -0.0031790863722563 , 0.0007922257063910 ,
     -0.0062704617157578 , -0.4398967623710632 , -0.0019123961683363 , -0.3124829530715942 , -0.0013738593552262 ,
      -0.2736729681491852 , -0.0005303606158122 , -0.1856940239667892 , 0.0032562753185630 , 0.0033946768380702 ,
       0.0009017214179039 , 0.0056162062101066 , -0.0029076063074172 , 0.0093219773843884 , 0.3056060373783112 ,
        0.0065882569178939 , 0.2140953391790390 , 0.0048937271349132 , 0.1806926280260086 , 0.0032033247407526 ,
         0.1639476567506790 , 0.0010877241147682 , 0.1121592447161674 , -0.0054505015723407 , -0.0021450689528137 ,
          -0.0035454323515296 , -0.0034215976484120 , -0.0002555686805863 , -0.0042258240282536 , 0.0038445815443993 ,
           -0.0087617998942733 , -0.1715657562017441 , -0.0074622663669288 , -0.1200785934925079 , -0.0050798300653696 ,
            -0.0987995192408562 , -0.0038617716636509 , -0.0864784419536591 , -0.0024104586336762 , -0.0801309272646904 ,
             -0.0008229380473495 , -0.0553935766220093 , 0.0062466156668961 , 0.0009536101133563 , 0.0049504614435136 ,
              0.0010324962204322 , 0.0028614804614335 , 0.0006008234340698 , 0.0002462268457748 , 0.0010043783113360 ,
               -0.0033524711616337 , 0.0054709850810468 , 0.0639840960502625 , 0.0045239026658237 , 0.0469817630946636 ,
                0.0022510222624987 , 0.0375003814697266 , 0.0017930285539478 , 0.0315470919013023 , 0.0007313973037526 ,
                 0.0287512484937906 , 0.0003033570828848 , 0.0276012029498816 , 0.0000140677411764 , 0.0192933082580566 ,
                  -0.0034187003038824 , -0.0000851582808536 , -0.0031985864043236 , -0.0003224085958209 , -0.0023568919859827 ,
                   0.0001351203100057 , -0.0013392785331234 , 0.0006681153899990 , -0.0000515153878951 , 0.0003699171356857 ,
                    0.0028312993235886 , -0.0023936298675835
  };

  PrecisionType c3[91] = {0.2994056642055511 , -0.2773320972919464 , -0.0059803873300552 , 0.1594088226556778 ,
   0.0032274045515805 , -0.0005021223914810 , 0.2646422386169434 , 0.0004583702248055 , 0.2100456655025482 ,
    -0.0135740982368588 , 0.4955773353576660 , 0.0138998441398144 , -0.0033890064805746 , 0.0016442969208583 ,
     -0.0009842916624621 , -0.2119190692901611 , -0.0013907576212659 , -0.1614883691072464 , 0.0015569501556456 ,
      -0.1339580565690994 , -0.0173183716833591 , 0.2185015827417374 , 0.0137488031759858 , -0.0028434635605663 ,
       -0.0024467380717397 , 0.0019360268488526 , -0.0033752415329218 , 0.0023204693570733 , 0.1449441760778427 , 
       0.0026769235264510 , 0.1091511324048042 , 0.0011079097166657 , 0.1018161624670029 , 0.0036555707920343 , 
       0.1157872602343559 , -0.0148042393848300 , 0.3077879846096039 , 0.0115566141903400 , -0.0053594429045916 ,
        0.0041679027490318 , -0.0015579717000946 , 0.0029479342047125 , -0.0003566828090698 , 0.0051041091792285 ,
         -0.0026987842284143 , -0.0801542624831200 , -0.0032308509107679 , -0.0597564540803432 , -0.0019522014772519 ,
         -0.0578437037765980 , -0.0003049694350921 , -0.0520530603826046 , 0.0022848085500300 , -0.0370721668004990 ,
          -0.0090587195008993 , 0.1053345277905464 , 0.0012136104051024 , -0.0022309850901365 , 0.0050277961418033 ,
           -0.0007820515893400 , -0.0008478201343678 , 0.0000982456185739 , -0.0013062087818980 , -0.0018589701503515 ,
            -0.0043290387839079 , 0.0019842567853630 , 0.0298414751887321 , 0.0023703293409199 , 0.0224530976265669 ,
             0.0013997979694977 , 0.0228327009826899 , -0.0002911534393206 , 0.0231529511511326 , 0.0018399785039946 ,
             0.0310183446854353 , 0.0030839329119772 , 0.0422041788697243 , -0.0010698565747589 , 0.0838758200407028 ,
              -0.0061566932126880 , 0.0007175270002335 , 0.0054536499083042 , -0.0004744650213979 , 0.0013150272425264 ,
               0.0008875888888724 , 0.0029091695323586 , 0.0003751906333491 , 0.0025598567444831 , 0.0019498664187267 ,
                0.0024747771676630 , -0.0009497676510364 
  };


  typedef unsigned int    GradientPixelType;
  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, 12 >
                                                          PixelType;


  typedef itk::FodCsdReconImageFilter<
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
  
  //Create the response function in the RSH space
  vnl_vector<double> respRot = vnl_vector<double>(7);
  respRot[0] = 0.6974809399761335;
  respRot[1] = -0.5700179575889499;
  respRot[2] = 0.2774507111038906;
  respRot[3] = -0.07148441157218688;
  respRot[4] = -0.0001397338893377939;
  respRot[5] = 0.02963003563177089;
  respRot[6] = -0.03236432352649706;
 
  reconFilter->SetResponseRSH(respRot);
  
  reconFilter->UseCSDOn();
  
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
  fprintf(stderr,"\n\n");

  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 1;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel2" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix2[i],fabs(testPixel[i] - pix2[i]));
    if (fabs(testPixel[i] - pix2[i]) > precision)
      passed = false;
  }
  fprintf(stderr,"\n\n");

  outputImageIndex[0] = 0; outputImageIndex[1] = 0; outputImageIndex[2] = 2;
  testPixel = outputImage->GetPixel(outputImageIndex);
  std::cout << "testing Pixel3" << std::endl;
  for (unsigned int i=0;i<OutputPixelType::Dimension;i++)
  {
    fprintf(stderr,"index %d : %f - %f = %f\n",i,testPixel[i],pix3[i],fabs(testPixel[i] - pix3[i]));
    if (fabs(testPixel[i] - pix3[i]) > precision)
      passed = false;
  }
  fprintf(stderr,"\n\n");

  if (!passed){
    std::cerr << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }

  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}

int main( int argc, char * argv[])
{
  if ( FodReconImTest1(argc,argv) == EXIT_FAILURE )
    return EXIT_FAILURE;
  if ( FodReconImTest2(argc,argv) == EXIT_FAILURE )
    return EXIT_FAILURE;
  if ( FodReconImTest3(argc,argv) == EXIT_FAILURE )
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
