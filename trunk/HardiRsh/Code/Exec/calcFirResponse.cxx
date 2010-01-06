/*
 *      Copyright 2008 Luke Bloy <lbloy@seas.upenn.edu>
 *
 */


#include <iostream>
#include <iomanip>      // Needed for stream modifiers fixed and setprecision
#include <fstream>

#include <getopt.h>
#include <sys/stat.h>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNiftiImageIO.h"
#include "itkImageRegionConstIterator.h"

#include "itkImageMaskSpatialObject.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkNumericTraits.h"

#include "itkVariableLengthVectorCastImageFilter.h"
#include "itkVectorImage.h"

#include "itkFiberImpulseResponseImageCalculator.h"

#include "itkSymRealSphericalHarmonicRep.h"

#define INDENT "\t"
#define EXEC_NAME "calcFirResponse"
#define SVN_FILE_VERSION "$Id: calcFirResponse.cxx 172 2009-09-29 18:39:02Z bloyl@UPHS.PENNHEALTH.PRV $"

#ifndef RELEASE_ID
#define RELEASE_ID "0.0_super_alpha"
#endif

#ifndef SVN_REV
#define SVN_REV "NO_SVN"
#endif


static int verbose = 0;

static int fileExists(std::string fName)
{
  struct stat st;
  if(stat(fName.c_str(),&st) == 0)
    return 1; //fName is there
  else
    return 0;
}

void echoVersion()
{
  std::cerr << std::endl << EXEC_NAME << std::endl <<
  INDENT << " Release          : " << RELEASE_ID << std::endl <<
  INDENT << " Svn Revision     : " << SVN_REV << std::endl <<
  INDENT << " Svn File verions : " << SVN_FILE_VERSION << std::endl
  << std::endl;
}

void GetImageInfo(std::string fName,itk::ImageIOBase::IOPixelType *pixelType, itk::ImageIOBase::IOComponentType *componentType)
{
  itk::ImageIOBase::Pointer imageIO;
  //~ try
  //~ {
    imageIO = itk::ImageIOFactory::CreateImageIO( fName.c_str() , itk::ImageIOFactory::ReadMode);
    if ( imageIO )
    {
      imageIO->SetFileName(fName);
      imageIO->ReadImageInformation();
      *pixelType = imageIO->GetPixelType();
      *componentType = imageIO->GetComponentType();
      //~ std::cerr << "reading : " << fName << std::endl
        //~ << "pixelType     : " << imageIO->GetPixelTypeAsString(*pixelType) << std::endl
        //~ << "componantType : " << imageIO->GetComponentTypeAsString(*componentType) << std::endl;
    }
    else
    {
      std::cout << "Could not read the input image information from " <<
        fName << std::endl;
      //TODO should throw exception
      exit( EXIT_FAILURE );
    }
}

// echoUsage:display usage information
void echoUsage()
{
    echoVersion();
    std::cerr
    << EXEC_NAME << "--\n"
    << EXEC_NAME << " Computes the real spherical harmonic coeffiecents of the DWI signal.\n"
    << "Usage: "<< EXEC_NAME << " [options]\n"
    << "Required Options:\n"
    << INDENT << "[-d --dataFile]      Specify the input file                 (required)\n"
    << INDENT << "[-p --prefix]        Prefix for results                     (required)\n"
    << INDENT << "[-r --bvecs]         File containing Gradient Directions    (required)\n"
    << INDENT << "[-b --bvals]         File containing bValues                (required)\n"
    << INDENT << "[-m --maskFile]      maskFile for brain region              (required)\n"
    << INDENT << "[-l --maxL]          The largest order (l) to fit to        (required)\n"

    << "Options:\n"
    << INDENT << "[-u --usage]         Display this message\n"
    << INDENT << "[-V --Version]       Display this version information\n"
    << INDENT << "[-v --verbose]       Turns on verbose output\n"
    << INDENT << "[-o --outputDir]     The output directory to write the results\n"
    << INDENT << "                     Defualts to the locatation of the input file\n"

    << std::endl;
}

using namespace std;

template <typename TPixelType, unsigned int NOrder, typename TPercisionType, typename TMaskPixelType>
int compute(
  string dataFile,
  string bvecFile,
  string bvalFile,
  string maskFile,
  string outputBasename,
  unsigned int vectorImage_flag,
  unsigned int verbose
)
{

  typedef TPixelType                        GradientPixelType;
  typedef TPercisionType                    PrecisionType;

  //Define the RSH PixelType to use.
  typedef itk::SymRealSphericalHarmonicRep<double,NOrder>
                                            RSHPixelType;

  typedef itk::FiberImpulseResponseImageCalculator<GradientPixelType,RSHPixelType>
                                            CaluculatorType;

  typename CaluculatorType::Pointer firFilter = CaluculatorType::New();

  typedef typename CaluculatorType::GradientImagesType      GradientImageType;

  typedef typename GradientImageType::RegionType            GradRegionType;
  typedef typename GradientImageType::SizeType              GradSizeType;
  typedef typename GradientImageType::IndexType             GradIndexType;

  typedef itk::ImageRegionIteratorWithIndex< GradientImageType > IteratorType;
  typedef typename GradientImageType::PixelType GradPixelType;


  //make the gradiant table
  typename CaluculatorType::GradientDirectionContainerType::Pointer
      DiffusionVectors = CaluculatorType::GradientDirectionContainerType::New();

  //Before we do anything lets parse the bvec and bval files... we want to keep track of the nGrads.
  int NumberOfGradients   = 0;
  double bValue           = 0;

  std::ifstream bvalIn(bvalFile.c_str());
  //first count the number of grad directions and make sure the only bvalues in the bval file
  // are 0 and a number...
  std::string line;
  while (! bvalIn.eof() ) {
      getline(bvalIn,line);
      double val;
      std::stringstream ss(line);

      while (ss >> val) {
          ++NumberOfGradients;
          if (val != 0 && bValue ==0)
              bValue = val;
          else if (val != 0 && bValue != val) {
              std::cerr << "multiple bvalues not allowed" << std::endl;
              return EXIT_FAILURE;
          }
      }
  }
  //ok so now lets process the gradient table...
  //read in each line and put it in a string stream to process...
  std::ifstream bvecIn(bvecFile.c_str());
  getline(bvecIn,line);
  std::stringstream Xss(line);
  getline(bvecIn,line);
  std::stringstream Yss(line);
  getline(bvecIn,line);
  std::stringstream Zss(line);

  typename CaluculatorType::GradientDirectionType vect3d;

  int counter = 0;
  double x,y,z;
  while (Xss >> x){
      Yss >> y;
      Zss >> z;
      vect3d[0] = x; vect3d[1] = y; vect3d[2] = z;
      DiffusionVectors->InsertElement( counter, vect3d );
      if (verbose == 2)
        std::cout << "inserting Grad: " << counter <<" : "<< vect3d << std::endl;
      ++counter;
  }

  if (counter != NumberOfGradients){
      std::cerr << "different number of bvalues and gradients" << std::endl;
      return EXIT_FAILURE;
  }

  if (verbose == 1)
    std::cerr << "Done processing grad table" << std::endl;


 typename GradientImageType::Pointer gradIm = GradientImageType::New();

 if (vectorImage_flag)
  {
    typedef itk::ImageFileReader< GradientImageType >  ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( dataFile );
    gradIm = reader->GetOutput();
    reader->Update();
  }
  else
  {
    typedef itk::Image<GradientPixelType, 4>                  InputImageType;
    typedef typename InputImageType::IndexType                Img4dIndexType;
    typedef itk::ImageFileReader< InputImageType >  ReaderType;

    ///TODO this should be a filter. cause we'll need to do it alot I think.
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( dataFile );
    typename InputImageType::Pointer img4D = reader->GetOutput();
    try
    {
      reader->Update();
    }
    catch( itk::ExceptionObject & err )
    {
      std::cerr << "ExceptionObject caught !" << std::endl;
      std::cerr << err << std::endl;
      return EXIT_FAILURE;
    }

    //Set up the gradient image size
    GradSizeType  sizeGradImage;
    typename InputImageType::SizeType size4D = img4D->GetLargestPossibleRegion().GetSize();
    sizeGradImage[0] = size4D[0];
    sizeGradImage[1] = size4D[1];
    sizeGradImage[2] = size4D[2];
    gradIm->SetVectorLength(size4D[3]);

    if (verbose > 2)
      std::cerr << "size4d : " << size4D << std::endl;

    GradIndexType   indexGradImage = {{ 0, 0, 0 }};
    GradRegionType  regionGradImage;
    regionGradImage.SetSize(  sizeGradImage );
    regionGradImage.SetIndex( indexGradImage);
    gradIm->SetRegions( regionGradImage );
    typename InputImageType::SpacingType img4Dspacing = img4D->GetSpacing();
    typename InputImageType::PointType img4Dorigin = img4D->GetOrigin();
    typename InputImageType::DirectionType img4Ddir = img4D->GetDirection();

    typename GradientImageType::SpacingType gradSpacing;
    typename GradientImageType::PointType gradOrigin;
    typename GradientImageType::DirectionType gradDirs;

    gradSpacing[0]  = img4Dspacing[0];  gradSpacing[1] = img4Dspacing[1];   gradSpacing[2] = img4Dspacing[2];
    gradOrigin[0]   = img4Dorigin[0];   gradOrigin[1]  = img4Dorigin[1];    gradOrigin[2] = img4Dorigin[2];

    for (unsigned int i = 0; i<3; ++i)
    {
      for (unsigned int j = 0; j<3; ++j)
      {
        gradDirs[i][j] = img4Ddir[i][j];
      }
    }

    gradIm->SetSpacing( gradSpacing );
    gradIm->SetOrigin( gradOrigin );
    gradIm->SetDirection( gradDirs );

    gradIm->Allocate();

    if (verbose > 2)
      printf("Done GradIm->Allocate\n");

    ///Copy data from img4d to gradim
    ////THIS IS SLOW!!!
    IteratorType it( gradIm, gradIm->GetRequestedRegion() );

    ///Probably a better way to do this but I don't really know what it is.
    for ( it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      GradIndexType   gradIndex = it.GetIndex();
      GradPixelType   gradPix = it.Get();
      Img4dIndexType  img4dIndex;
      img4dIndex[0] = gradIndex[0];
      img4dIndex[1] = gradIndex[1];
      img4dIndex[2] = gradIndex[2];

      for ( unsigned int i=0; i<size4D[3]; ++i )
      {
        img4dIndex[3] = i;
        gradPix.SetElement( i, img4D->GetPixel( img4dIndex ) );
      }
      it.Set( gradPix );
    }
    if (verbose)
      printf("Done Conversion\n");
  }
  //if the mask flag is set lets use it!!!
  if ( !maskFile.empty() )
  {
    typedef itk::Image< TMaskPixelType , 3 >   ImageMaskType;

    typedef itk::ImageMaskSpatialObject< 3 >   MaskType;
    typename MaskType::Pointer  spatialObjectMask = MaskType::New();

    typedef itk::ImageFileReader< ImageMaskType >    MaskReaderType;

    typename MaskReaderType::Pointer  maskReader = MaskReaderType::New();
    
    typedef itk::BinaryThresholdImageFilter< ImageMaskType, typename MaskType::ImageType >
                                                    ThresholderType;
    typename ThresholderType::Pointer thresholder = ThresholderType::New();
    
    thresholder->SetOutsideValue(itk::NumericTraits< typename MaskType::ImageType::PixelType>::Zero);
    thresholder->SetInsideValue(itk::NumericTraits< typename MaskType::ImageType::PixelType>::One);
    
    //the threshold is inclusive so is >= 1
    thresholder->SetLowerThreshold(itk::NumericTraits< TMaskPixelType >::One);
    thresholder->InPlaceOn();
    
    if (verbose)
      std::cerr << "Reading maskFile '" << maskFile << "'" << std::endl;

    if (verbose >=2)
    {
      std::cerr << "Setting up binary Threshold filter" << std::endl;
      std::cerr << "low thresh : " << thresholder->GetLowerThreshold()<<std::endl;
      printf("outValue = %d\ninValue = %d\n",thresholder->GetOutsideValue(),thresholder->GetInsideValue());
    }

    maskReader->SetFileName( maskFile );
    try
    {
      maskReader->Update();
    }
    catch( itk::ExceptionObject & err )
    {
      std::cerr << "ExceptionObject caught !" << std::endl;
      std::cerr << err << std::endl;
      return EXIT_FAILURE;
    }
    thresholder->SetInput( maskReader->GetOutput() );
    thresholder->Update();
    spatialObjectMask->SetImage( thresholder->GetOutput() );
    firFilter->SetImageMask( spatialObjectMask );
  }

  firFilter->SetGradientImage( DiffusionVectors, gradIm);
  firFilter->SetBValue(bValue);

  if (verbose)
    printf("computing Fiber Response function\n");

  firFilter->Compute();
  typename CaluculatorType::VectorType firRsh = firFilter->GetRespRSH();

  if (verbose)
  {
    printf("Fiber Response function:\n");
    std::cerr << firRsh << std::endl;
  }

  ofstream out( (outputBasename + ".txt").c_str() );
  out << fixed << setprecision(10) <<  firRsh << std::endl;
  
  return EXIT_SUCCESS;
}

template <unsigned int NOrder, typename TPercisionType, typename TMaskPixelType>
int runnerOverDWICompType(
  string dataFile,
  string bvecFile,
  string bvalFile,
  string maskFile,
  string outputBasename,
  unsigned int verbose
)
{
  //Done Parsing Inputs lets now figure out What we want to do.
  itk::ImageIOBase::IOPixelType       pixelType;
  itk::ImageIOBase::IOComponentType   compType;
  GetImageInfo(dataFile,&pixelType,&compType);

  int inputIsVectorImage = 0;
  
  if ( pixelType == itk::ImageIOBase::VECTOR )
  {
    inputIsVectorImage = 1;
  }
  else if ( pixelType != itk::ImageIOBase::SCALAR)
  {
    std::cout << "dwi pixel type must be either vecotro or scalar";
    return(EXIT_FAILURE);
  }
  
  switch ( compType )
  {
    case itk::ImageIOBase::CHAR:
    {
      return compute<char, NOrder, TPercisionType,TMaskPixelType> 
        (dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose, inputIsVectorImage);
      break;
    }
    case itk::ImageIOBase::UCHAR:
    {
      return compute<unsigned char, NOrder, TPercisionType,TMaskPixelType> 
        (dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose, inputIsVectorImage);
      break;
    }
    case itk::ImageIOBase::USHORT:
    {
      return compute<unsigned short, NOrder, TPercisionType,TMaskPixelType> 
        (dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose, inputIsVectorImage);
      break;
    }
    case itk::ImageIOBase::SHORT:
    {
      return compute<short, NOrder, TPercisionType,TMaskPixelType> 
        (dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose, inputIsVectorImage);
      break;
    }
    case itk::ImageIOBase::UINT:
    {
      return compute<unsigned int, NOrder, TPercisionType,TMaskPixelType> 
        (dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose, inputIsVectorImage);
      break;
    }
    case itk::ImageIOBase::INT:
    {
      return compute<int, NOrder, TPercisionType,TMaskPixelType> 
        (dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose, inputIsVectorImage);
      break;
    }
    default:
    {
      std::cout << "Unsupported componant Type : " << std::endl;
      return EXIT_FAILURE;
    }
  }
}


template <typename TPercisionType, typename TMaskPixelType>
int runnerOverOrder(
  string dataFile,
  string bvecFile,
  string bvalFile,
  string maskFile,
  string outputBasename,
  unsigned int verbose,
  unsigned int lMax
)
{
  
  try {
    switch ( lMax )
    {
      case 2:
        return(runnerOverDWICompType<2, TPercisionType, TMaskPixelType>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose ));
        break;
      case 4:
        return(runnerOverDWICompType<4, TPercisionType, TMaskPixelType>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose ));
        break;
      case 6:
        return(runnerOverDWICompType<6, TPercisionType, TMaskPixelType>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose ));
        break;
      case 8:
        return(runnerOverDWICompType<8, TPercisionType, TMaskPixelType>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose ));
        break;
      case 10:
        return(runnerOverDWICompType<10, TPercisionType, TMaskPixelType>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose ));
        break;
      case 12:
        return(runnerOverDWICompType<12, TPercisionType, TMaskPixelType>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose ));
        break;
      case 14:
        return(runnerOverDWICompType<14, TPercisionType, TMaskPixelType>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose ));
        break;
      case 16:
        return(runnerOverDWICompType<16, TPercisionType, TMaskPixelType>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose ));
        break;
      case 18:
        return(runnerOverDWICompType<18, TPercisionType, TMaskPixelType>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose ));
        break;
      case 20:
        return(runnerOverDWICompType<20, TPercisionType, TMaskPixelType>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose ));
        break;
      default:
        std::cout << "Unsupported dimension" << std::endl;
        return( EXIT_FAILURE );
    }
  }
  catch (exception& e)
  {
    cout << "Standard exception: " << e.what() << endl;
    return( EXIT_FAILURE );
  }
  return( EXIT_SUCCESS );
}

int main(int argc, char** argv)
{
  string outputDir="";
  string dataFile,prefix;
  string bvecFile;
  string bvalFile;
  string maskFile;

  int outputDir_flag = 0;
  int prefix_flag = 0;

  static struct option long_options[] =
  {
    {"usage",       no_argument,            0, 'u'},
    {"help",        no_argument,            0, 'h'},
    {"Version",     no_argument,            0, 'V'},
    {"verbose",     no_argument,            0, 'v'},
    {"outputDir",   required_argument,      0, 'o'},
    {"dataFile",    required_argument,      0, 'd'},
    {"prefix",      required_argument,      0, 'p'},
    {"bvecs",       required_argument,      0, 'r'},
    {"bvals",       required_argument,      0, 'b'},
    {"maskFile",    required_argument,      0, 'm'},
    {"maxL",        required_argument,      0, 'l'},
  };

  int c, option_index = 0;
  int reqParams = 0;
  int maxL = 4;
  while ( (c = getopt_long (argc, argv, "uhVvo:d:p:r:t:m:l:b:",
              long_options,&option_index)) != -1)
  {
    switch (c)
    {
      case 'u':
        echoUsage();
        return EXIT_SUCCESS;

      case 'h':
        echoUsage();
        return EXIT_SUCCESS;

      case 'V':
        echoVersion();
        return EXIT_SUCCESS;

      case 'v':
        verbose++;
        break;

      case 'o':
        outputDir = optarg;
        outputDir_flag = 1;
        outputDir += "/";
        break;

      case 'd':
        dataFile = optarg;
        ++reqParams;
        break;

      case 'p':
        prefix = optarg;
        prefix_flag = 1;
        ++reqParams;
        break;

      case 'r':
        bvecFile = optarg;
        ++reqParams;
        break;

      case 'b':
        bvalFile = optarg;
        ++reqParams;
        break;

      case 'm':
        maskFile = optarg;
        ++reqParams;
        break;

      case 'l':
        maxL = atoi(optarg);
        ++reqParams;
        break;

      case '?':
        /* getopt_long already printed an error message. */
        break;

      default:
        abort ();

    }
  }

  if ( reqParams != 6)
  {
    echoUsage();
    cerr << "Please specify all required parameters!\n";
     cerr << reqParams << endl;
   return EXIT_FAILURE;
  }

  //Process fileNames...
  string extension = "";
  string bName = "";
  string path  = "";
  string::size_type extPos = dataFile.rfind(".");
  if ( extPos != std::string::npos )
  {
    extension   = dataFile.substr(extPos,dataFile.length() - extPos);
    string tmp = dataFile.substr(0,extPos);
    std::string::size_type pathPos = dataFile.rfind("/");
    if ( pathPos != std::string::npos )
    {
      path  = tmp.substr(0,pathPos)+"/";
      bName = tmp.substr(pathPos,tmp.length()-pathPos);
    }
    else
    {
       path = "./";
       bName = tmp;
    }
  }
  else
  {
    std::cerr << "Can't find extension!!!!" << std::endl;
    return EXIT_FAILURE;
  }

  string outputBasename = "";
  if (outputDir_flag)
    outputBasename += outputDir;
  else
    outputBasename += path;

  if (prefix_flag)
    outputBasename += prefix;
  else
    outputBasename += bName;

 //check for the existence of the input files.
  if (!fileExists(dataFile))
  {
    std::cerr << dataFile << " --- Doesn't exist!!!\nExiting!\n\n" << std::endl;
    echoUsage();
    return EXIT_FAILURE;
  }

  if (!fileExists(bvecFile))
  {
    std::cerr << bvecFile << " --- Doesn't exist!!!\nExiting!\n\n" << std::endl;
    echoUsage();
    return EXIT_FAILURE;
  }

  if (!fileExists(bvalFile))
  {
    std::cerr << bvalFile << " --- Doesn't exist!!!\nExiting!\n\n" << std::endl;
    echoUsage();
    return EXIT_FAILURE;
  }

  if (!fileExists(maskFile))
  {
    std::cerr << maskFile << " --- Doesn't exist!!!\nExiting!\n\n" << std::endl;
    echoUsage();
    return EXIT_FAILURE;
  }

  if (verbose)
  {
    std::cerr << "data input File :" << dataFile << std::endl;
    std::cerr << "bvec input File :" << bvecFile << std::endl;
    std::cerr << "bval input File :" << bvalFile << std::endl;
    std::cerr << "Output basename : " << outputBasename <<  std::endl;
    std::cerr << "Using mask      : " << maskFile <<  std::endl;
    std::cerr << "Max order L     : " << maxL <<  std::endl;
    std::cerr << std::endl;
  }

  /*
   * Start working
   */
  
  //Determine the mask Type
  itk::ImageIOBase::IOPixelType       maskPixelType;
  itk::ImageIOBase::IOComponentType   maskCompType;
  GetImageInfo(maskFile,&maskPixelType,&maskCompType);
  
  if ( maskPixelType != itk::ImageIOBase::SCALAR )
  {
    std::cout << "Unsupported Mask Pixel Type" << std::endl;
    exit( EXIT_FAILURE );
  }

  try {
    switch ( maskCompType )
    {
      case itk::ImageIOBase::UCHAR:
      case itk::ImageIOBase::CHAR:
        return (runnerOverOrder<float,char>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose, maxL ));
        break;
      case itk::ImageIOBase::USHORT:
      case itk::ImageIOBase::SHORT:
        return (runnerOverOrder<float,short>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose, maxL ));
        break;
      case itk::ImageIOBase::UINT:
      case itk::ImageIOBase::INT:
        return (runnerOverOrder<float,int>( dataFile, bvecFile, bvalFile, maskFile, outputBasename, verbose, maxL ));
        break;
      default:
        std::cout << "Unsupported Mask Componant Type" << std::endl;
        exit( EXIT_FAILURE );
    }
  }
  catch (exception& e)
  {
    cout << "Standard exception: " << e.what() << endl;
  }
  
  return EXIT_SUCCESS;
}

