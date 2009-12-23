
#include <iostream>
#include <getopt.h>
#include <sys/stat.h> 

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNiftiImageIO.h"
#include "itkImageRegionConstIterator.h"

#include "itkFodCsdReconImageFilter.h"
#include "itkSymRealSphericalHarmonicRep.h"
//#include "itkFODRealSymSphericalHarmonicBasis.h"
#include "itkRealSymSphericalHarmonicBasis.h"

#include "itkImageMaskSpatialObject.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkNumericTraits.h"

#include "itkVariableLengthVectorCastImageFilter.h"
#include "itkVectorImage.h"


#define INDENT "\t"
#define EXEC_NAME "fodRecon"
#define SVN_FILE_VERSION "$Id: fodRecon.cxx 186 2009-10-13 13:55:22Z bloyl@UPHS.PENNHEALTH.PRV $"

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
    }
    else
    {
      std::cout << "Could not read the input image information from " <<
        fName << std::endl;
      //TODO should throw exception
      exit( EXIT_FAILURE );
    }
}

///TODO support normalize and residue options
// echoUsage:display usage information
void echoUsage()
{
    echoVersion();
    std::cerr
    << EXEC_NAME << "--\n"
    << EXEC_NAME << " COmputes the FOD in the real Spherical Harmonic basis set.\n"
    << INDENT << "First computes the diffusion tenso images and the largest FA\n"
    << INDENT << "In Order to determin the Fiber impulse responce function\n"
    << "Usage: "<< EXEC_NAME << " [options]\n"
    << "Required Options:\n"
    << INDENT << "[-d --dataFile]      Specify the input file                 (required)\n"
    << INDENT << "[-p --prefix]        Prefix for results                     (required)\n"
    << INDENT << "[-r --bvecFile]      File containing Gradient Directions    (required)\n"
    << INDENT << "[-R --firRespFile]   File containing rsh comps of the FIR   (required)\n"
    << INDENT << "[-m --maskFile]      maskFile for brain region              (required)\n"
    << INDENT << "[-l --maxL]          The largest order (l) to fit to        (required)\n"

    << "Options: -n and -e are ignored at the moment\n"
    << INDENT << "[-n --normalize]     Should we normalize the FODs (0==No,1==Yes,defualts to No)\n"
    << INDENT << "[-e --error]         Save the Residue of the gradient images\n"

    << INDENT << "[-u --usage]         Display this message\n"
    << INDENT << "[-V --Version]       Display this version information\n"
    << INDENT << "[-v --verbose]       Turns on verbose output\n"
    << INDENT << "[-o --outputDir]     The output directory to write the results\n"
    << INDENT << "                     Defualts to the locatation of the input file\n"

    << std::endl;
}

using namespace std;
#include "itkCommand.h"
class CommandProgressUpdate : public itk::Command
{
public:
  typedef  CommandProgressUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandProgressUpdate() {};
public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      const itk::ProcessObject * filter =
        dynamic_cast< const itk::ProcessObject * >( object );
      if( ! itk::ProgressEvent().CheckEvent( &event ) )
        {
        return;
        }
      std::cout << filter->GetProgress() << std::endl;
    }
};

template <typename TPixelType, unsigned int NOrder, typename TPercisionType, typename TMaskPixelType>
int compute(
string dataFile,
string bvecFile,
string respFile,
string maskFile,
string outputBasename,
unsigned int useCSD_flag,
unsigned int vectorImage_flag,
unsigned int verbose,
unsigned int normalize_flag
)
{

  typedef TPixelType                                        GradientPixelType;
  typedef TPercisionType                                    PrecisionType;

//  typedef itk::FODRealSymSphericalHarmonicBasis< NOrder>    RSHBasisType;
  typedef itk::RealSymSphericalHarmonicBasis< NOrder>    RSHBasisType;

  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, NOrder, RSHBasisType >
                                                            RshPixelType;

  typedef itk::FodCsdReconImageFilter<
                    GradientPixelType, RshPixelType>        ReconFilterType;


  typename ReconFilterType::Pointer reconFilter =
                                            ReconFilterType::New();

  typedef typename ReconFilterType::GradientImagesType      GradientImageType;

  typedef typename GradientImageType::RegionType            GradRegionType;
  typedef typename GradientImageType::SizeType              GradSizeType;
  typedef typename GradientImageType::IndexType             GradIndexType;

  typedef itk::ImageRegionIteratorWithIndex
                              < GradientImageType >         IteratorType;
  typedef typename GradientImageType::PixelType             GradPixelType;

  typedef typename ReconFilterType::OutputImageType         ImageType;
  
  ///TODO extract Dimension out of ODFImageType;
  typedef itk::VectorImage
    <typename ImageType::PixelType::ComponentType, 3>       VectorImageType;

  typedef itk::VariableLengthVectorCastImageFilter
                          < ImageType,VectorImageType >     CasterType;
  
  typedef itk::ImageFileWriter< VectorImageType >           WriterType;

  //make the gradiant table
  typename ReconFilterType::GradientDirectionContainerType::Pointer
      DiffusionVectors = ReconFilterType::GradientDirectionContainerType::New();

  //Process the gradient table...
  //read in each line and put it in a string stream to process...
  ifstream bvecIn(bvecFile.c_str());
  std::string line;
  getline(bvecIn,line);
  stringstream Xss(line);
  getline(bvecIn,line);
  stringstream Yss(line);
  getline(bvecIn,line);
  stringstream Zss(line);

  typename ReconFilterType::GradientDirectionType vect3d;

  int counter = 0;
  double x,y,z;
  while (Xss >> x)
  {
    Yss >> y;
    Zss >> z;
    vect3d[0] = x; vect3d[1] = y; vect3d[2] = z;
    DiffusionVectors->InsertElement( counter, vect3d );
    if (verbose >= 2)
    {
      std::cout << "inserting Grad: " << counter <<" : "<< vect3d << std::endl;
    }
    ++counter;
  }

  if (verbose >= 1)
  {
    std::cerr << "Done processing grad table" << std::endl;
    std::cerr << "Found "<<counter << " Gradients" << std::endl;
  }

  //Read the fiber response file to load the spherical Harmonics of the FIR.
  vnl_vector<double> firRSH; 
  ifstream respIn(respFile.c_str());

  getline(respIn,line);
  stringstream Rss(line);
 
  double coeff;
  counter = 0;
  while (Rss >> coeff)
    counter ++;    

  firRSH.set_size(counter);
  // Rewind the stream
  Rss.clear();
  Rss.seekg ( 0, std::ios::beg );
  counter = 0;
  while (Rss >> coeff)
  {
    firRSH[counter++] = coeff;
  }
  
  if (verbose >= 1)
  {
    std::cerr << "Fiber Impulse Response Function" << std::endl;
    std::cerr << firRSH << std::endl;
  }
  
  reconFilter->SetResponseRSH(firRSH);

  //Convert the 4dFile to a vectorImage 
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

    typedef itk::ImageFileReader< InputImageType >            ReaderType;

    if (verbose)
      printf("Converting DWI to vector image\n");

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

    //Copy data from img4d to gradim
    //THIS IS SLOW!!!
    IteratorType it( gradIm, gradIm->GetRequestedRegion() );

    //Probably a better way to do this but I don't really know what it is.
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
    reconFilter->SetImageMask( spatialObjectMask );
  }

  reconFilter->SetGradientImage( DiffusionVectors, gradIm);
  if (verbose)
  {
    CommandProgressUpdate::Pointer observer = CommandProgressUpdate::New();
    reconFilter->AddObserver( itk::ProgressEvent(), observer );
  }

  if (verbose)
    printf("computing Fod image\n");
    
  if ( useCSD_flag )
  {
    reconFilter->UseCSDOn();    
  }
  else
  {
    reconFilter->UseCSDOff();
  }

  if ( normalize_flag )
  {
    reconFilter->NormalizeOn();    
  }
  else
  {
    reconFilter->NormalizeOff();
  }

  reconFilter->Update();

  typename WriterType::Pointer writer = WriterType::New();

  itk::NiftiImageIO::Pointer imageIO = itk::NiftiImageIO::New();
  writer->SetImageIO(imageIO);
  writer->SetFileName( outputBasename+".hdr" );

  //We need to cast the odf output to vectorImage.
  typename CasterType::Pointer caster = CasterType::New();
  caster->SetInput(reconFilter->GetOutput() );

  if (verbose)
    printf("Casting ODF image\n");
    
  caster->Update();
  
  writer->SetInput( caster->GetOutput() );
  if (verbose)
    printf("Writing ODF image\n");

  writer->Update();

  return EXIT_SUCCESS;
}


template <unsigned int NOrder, typename TPercisionType, typename TMaskPixelType>
int runnerOverDWICompType(
string dataFile,
string bvecFile,
string respFile,
string maskFile,
string outputBasename,
unsigned int useCSD_flag,
unsigned int verbose,
unsigned int normalize_flag
)
{
  //Done Parsing Inputs lets now figure out What we want to do.
  itk::ImageIOBase::IOPixelType       pixelType;
  itk::ImageIOBase::IOComponentType   compType;
  GetImageInfo(dataFile,&pixelType,&compType);

  int vectorImage_flag = 0;
  
  if ( pixelType == itk::ImageIOBase::VECTOR )
  {
    vectorImage_flag = 1;
  }
  else if ( pixelType != itk::ImageIOBase::SCALAR)
  {
    std::cout << "dwi pixel type must be either vector scalar";
    return(EXIT_FAILURE);
  }
  
  switch ( compType )
  {
    case itk::ImageIOBase::CHAR:
    {
      return (compute<char, NOrder, TPercisionType,TMaskPixelType> 
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, vectorImage_flag, verbose, normalize_flag ));
      break;
    }
    case itk::ImageIOBase::UCHAR:
    {
      return (compute<unsigned char, NOrder, TPercisionType,TMaskPixelType> 
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, vectorImage_flag, verbose, normalize_flag ));
      break;
    }
    case itk::ImageIOBase::USHORT:
    {
      return (compute<unsigned short, NOrder, TPercisionType,TMaskPixelType> 
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, vectorImage_flag, verbose, normalize_flag ));
      break;
    }
    case itk::ImageIOBase::SHORT:
    {
      return (compute<short, NOrder, TPercisionType,TMaskPixelType> 
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, vectorImage_flag, verbose, normalize_flag ));
      break;
    }
    case itk::ImageIOBase::UINT:
    {
      return (compute<unsigned int, NOrder, TPercisionType,TMaskPixelType> 
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, vectorImage_flag, verbose, normalize_flag ));
      break;
    }
    case itk::ImageIOBase::INT:
    {
      return (compute<int, NOrder, TPercisionType,TMaskPixelType> 
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, vectorImage_flag, verbose, normalize_flag ));
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
string respFile,
string maskFile,
string outputBasename,
unsigned int useCSD_flag,
unsigned int verbose,
unsigned int lMax,
unsigned int normalize_flag
)
{
  
  try {
    switch ( lMax )
    {
      case 2:
        return(runnerOverDWICompType<2, TPercisionType, TMaskPixelType>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, normalize_flag ));
        break;
      case 4:
        return(runnerOverDWICompType<4, TPercisionType, TMaskPixelType>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, normalize_flag ));
        break;
      case 6:
        return(runnerOverDWICompType<6, TPercisionType, TMaskPixelType>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, normalize_flag ));
        break;
      case 8:
        return(runnerOverDWICompType<8, TPercisionType, TMaskPixelType>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, normalize_flag ));
        break;
      case 10:
        return(runnerOverDWICompType<10, TPercisionType, TMaskPixelType>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, normalize_flag ));
        break;
      case 12:
        return(runnerOverDWICompType<12, TPercisionType, TMaskPixelType>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, normalize_flag ));
        break;
      case 14:
        return(runnerOverDWICompType<14, TPercisionType, TMaskPixelType>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, normalize_flag ));
        break;
      case 16:
        return(runnerOverDWICompType<16, TPercisionType, TMaskPixelType>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, normalize_flag ));
        break;
      case 18:
        return(runnerOverDWICompType<18, TPercisionType, TMaskPixelType>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, normalize_flag ));
        break;
      case 20:
        return(runnerOverDWICompType<20, TPercisionType, TMaskPixelType>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, normalize_flag ));
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
  string respFile;
  string maskFile;

  int outputDir_flag = 0;
  int prefix_flag = 0;
  int useCSD_flag = 1;
  int normalize_flag = 0;
  
  static struct option long_options[] =
  {
    {"usage",       no_argument,            0, 'u'},
    {"help",        no_argument,            0, 'h'},
    {"Version",     no_argument,            0, 'V'},
    {"verbose",     no_argument,            0, 'v'},
    {"outputDir",   required_argument,      0, 'o'},
    {"dataFile",    required_argument,      0, 'd'},
    {"prefix",      required_argument,      0, 'p'},
    {"bvecFile",    required_argument,      0, 'r'},
    {"firRespFile", required_argument,      0, 'R'},
    {"maskFile",    required_argument,      0, 'm'},
    {"maxL",        required_argument,      0, 'l'},
    {"normalize",   required_argument,      0, 'n'}
  };

  int c, option_index = 0;
  int reqParams = 0;
  int maxL = 4;
  
  while ( (c = getopt_long (argc, argv, "uhVvo:d:p:r:R:m:l:n:",
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

      case 'R':
        respFile = optarg;
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

      case 'n':
        normalize_flag = atoi(optarg);
        break;
        
      case '?':
        /* getopt_long already printed an error message. */
        break;

      default:
        echoUsage();
        cerr << "Unrecognized Option specified!\n";
        return EXIT_FAILURE;

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

  if (!fileExists(respFile))
  {
    std::cerr << respFile << " --- Doesn't exist!!!\nExiting!\n\n" << std::endl;
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
    std::cerr << "data input File    :" << dataFile << std::endl;
    std::cerr << "bvec input File    :" << bvecFile << std::endl;
    std::cerr << "firResp input File :" << bvecFile << std::endl;
    std::cerr << "Output basename    : " << outputBasename <<  std::endl;
    std::cerr << "Using mask         : " << maskFile <<  std::endl;
    std::cerr << "Max order L        : " << maxL <<  std::endl;
    if (normalize_flag) std::cerr << "normalizing FODs on" << std::endl;
    else std::cerr << "normalizing FODs off" << std::endl;
    std::cerr << std::endl;
  }

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
        return (runnerOverOrder<float,char>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, maxL, normalize_flag ));
        break;
      case itk::ImageIOBase::USHORT:
      case itk::ImageIOBase::SHORT:
        return (runnerOverOrder<float,short>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, maxL, normalize_flag ));
        break;
      case itk::ImageIOBase::UINT:
      case itk::ImageIOBase::INT:
        return (runnerOverOrder<float,int>
          ( dataFile, bvecFile, respFile, maskFile, outputBasename, useCSD_flag, verbose, maxL, normalize_flag ));
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
