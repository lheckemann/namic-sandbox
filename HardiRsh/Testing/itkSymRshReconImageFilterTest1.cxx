#include <iostream>

#include "itkSymRshReconImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkFixedArray.h"
#include "itkCastImageFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkImage.h"

#include "vnl/algo/vnl_determinant.h"

template <typename FilterType>
typename FilterType::GradientDirectionContainerType *
RotateGradientContainer( vnl_matrix<double> rotMatrix, const typename FilterType::GradientDirectionContainerType *inputContainer )
{
  //We want a full copy of the container at m_GradientDirectionContainer
  typename FilterType::GradientDirectionContainerType::Pointer
     gradContainer = FilterType::GradientDirectionContainerType::New();
  
  typename FilterType::GradientDirectionType dir;

  for (unsigned int g = 0; g < inputContainer->size(); ++g)
  {
    dir[0] = inputContainer->GetElement(g)[0];
    dir[1] = inputContainer->GetElement(g)[1];
    dir[2] = inputContainer->GetElement(g)[2];
    
    dir = rotMatrix * dir;
    gradContainer->InsertElement(g,dir/dir.two_norm());

  }
  
  return gradContainer.GetPointer();
}

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
addGradRotImage(FilterType *filter, vnl_matrix<double> rotMatrix)
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
    gradContainer->InsertElement(g,rotMatrix * dir);
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


int RshReconImTest1(int,char*[])
{
  //Just try to instantiate the filter...

  typedef unsigned int    GradientPixelType;
  typedef float           PrecisionType;
  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, 8 > RshPixelType;

  typedef itk::SymRshReconImageFilter<
    GradientPixelType,
    RshPixelType,     3 >      SymRshReconImageFilterType;

  try
  {
    SymRshReconImageFilterType::Pointer symReconFilter =
    SymRshReconImageFilterType::New();

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

int RshReconImTest2(int, char*[])
{

  typedef float           PrecisionType;

  PrecisionType c1[45] = {0.7212520319878115,-0.0003385282051116059,0.0000749605643584339,-0.595677776291134,0.00030204505569102896,
   0.0007960419344463232,0.0007041597783487286,-0.00004763110564954577,0.00023051855355286227,-0.00041468059636469434,
   0.31454381107471285,0.0003240763081598791,-0.0003646591186538331,-0.00013836214560059706,0.0010292282395887864,
   0.0011945036277760113,0.00044623369448974076,-0.0007192376368217943,0.0001093875566357573,-0.00015953304798718704,
   -0.0014569479866522461,-0.12532455607008042,0.001029082609176401,0.00015452420774457638,-0.0002648896339109974,
   -0.0012802266647137153,0.000852198927608263,0.001338962813971084,0.003603085155208449,-0.00008088348021568353,
   -0.0006703905742253685,0.000771066948178821,0.0012535994711646709,-0.0012808114574620819,0.000998683995279085,
   0.0014864901181477208,0.03818977726185059,-0.0015034416680695956,-0.0018179313371678475,-0.0020668893003337463,
   0.0019221161938821688,-0.0010735703142327364,-0.0013657370782731923,-0.00015421727639094215,-0.0003780481257301127
  };

  PrecisionType c2[45] = {0.7212110749207201,0.5168530171498542,-0.0002887325334129942,0.29788911035991555,3.860234109767657e-6,
   -0.00038487694700845704,0.2322622572563444,-0.0000787822590717635,0.17633456191915492,0.00003296242831990481,
   0.1175978674951678,-0.00033399760106559567,0.00011226457469246803,-0.0004354808055854545,-0.00012974224572428333,
   0.08428343529960733,-0.00070245883185939,0.06264318975634332,0.00014177810745572006,0.0571163340709046,
   -0.00020878955579194114,0.039809266718898555,-0.00008665204937968046,-0.00009089522710098441,0.001434095935573438,
   0.00042748406702716735,-0.0006327562015597532,0.0005174319996116457,0.02542372285822353,-0.00041436640641116795,
   0.01852488792573384,-0.0026877469185355926,0.015501524274953299,-0.0008583984377894538,0.01434862311254935,
   -0.0004204894651740898,0.010314197424105997,0.001515163961319288,0.0022637857150172927,0.0006290420490375863,
   0.0011482419010312075,-0.0015277262617840737,-0.0007141612004342816,-0.0026034004581681758,-0.0011534417144631534
  };

  PrecisionType c3[45] = {0.7211977899380844,0.25827464735418104,-0.00010275012352936597,-0.14888234429882974,0.00011593428533505229,
   0.0001715861676506933,0.11648169023021018,-0.00006418414526189117,0.08830982882484213,-0.00021039486880406431,
   0.21606700140931168,-0.00006279537875581755,-0.0000893896780772268,-0.00032473287001336636,0.0004155555087966001,
   0.04278211840113507,-0.00010172044544695842,0.030942325833962343,0.0001303745387379613,0.028471425672640424,
   -0.0008594115012602876,-0.0427163039981062,0.00047831654833043954,0.0000330825164025321,0.0005996833167219648,
   -0.00044924004222121963,0.00009484031269623577,0.0009627847882698048,0.014494029111202213,-0.00022235666473496146,
   0.008844929296054942,-0.0009806441505020996,0.008353915961239244,-0.0010268074663041665,0.007749051249967878,
   0.0005394462606931466,0.02424446428796903,0.000010505061840256069,0.00017900999250726468,-0.000663590090687699,
   0.0015326169250098123,-0.001266614198214694,-0.0010009815398488789,-0.0013854778392970273,-0.000767270095335934
  };


  typedef unsigned int    GradientPixelType;
  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, 8 >
                                                          PixelType;


  typedef itk::SymRshReconImageFilter<
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

  reconFilter->CalculateResidualImageOn();
  if (!reconFilter->GetCalculateResidualImage())
  {
    std::cerr << "Error in GetCalculatedResitualImage()\n";
    return EXIT_FAILURE;  
  }
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
  if ( RshReconImTest1(argc,argv) == EXIT_FAILURE )
    return EXIT_FAILURE;
  if ( RshReconImTest2(argc,argv) == EXIT_FAILURE )
    return EXIT_FAILURE;

  return EXIT_SUCCESS;

}
