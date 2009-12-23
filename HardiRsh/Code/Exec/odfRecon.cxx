
#include <iostream>
#include <getopt.h>
#include <sys/stat.h> 

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNiftiImageIO.h"
#include "itkImageRegionConstIterator.h"

//Recon filters
#include "itkOdfReconImageFilter.h"
//#include "itkFopdfReconImageFilter.h"

#include "itkSymRealSphericalHarmonicRep.h"

#include "itkImageMaskSpatialObject.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkNumericTraits.h"

#include "itkVariableLengthVectorCastImageFilter.h"
#include "itkVectorImage.h"

#define INDENT "\t"
#define EXEC_NAME "odfRecon"
#define SVN_FILE_VERSION "$Id: odfRecon.cxx 156 2009-09-18 22:24:53Z bloyl@UPHS.PENNHEALTH.PRV $"

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

// echoUsage:display usage information
void echoUsage()
{
    echoVersion();
    std::cerr
    << EXEC_NAME << "--\n"
    << EXEC_NAME << " COmputes the ODF in the real Spherical Harmonic basis set.\n"
    << "Usage: "<< EXEC_NAME << " [options]\n"
    << "Required Options:\n"
    << INDENT << "[-d --dataFile]      Specify the input file                 (required)\n"
    << INDENT << "[-p --prefix]        Prefix for results                     (required)\n"
    << INDENT << "[-r --bvecs]         File containing Gradient Directions    (required)\n"
    << INDENT << "[-m --maskFile]      maskFile for brain region\n"
    << INDENT << "[-l --maxL]          The largest order (l) to fit to\n"

    << "Options:\n"
    << INDENT << "[-n --normalize]     Should we normalize the ODFs (0==No,1==Yes,defualts to YES)\n"
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

template <typename TPixelType, unsigned int NOrder, typename TOdfPercisionType, typename TMaskPixelType>
int estimateOdf(string dataFile, string bvecFile, string maskFile,
                 string outputBasename, unsigned int residue_flag, unsigned int verbose,unsigned int normalize_flag)
{

  typedef TPixelType                        ReferencePixelType;
  typedef TPixelType                        GradientPixelType;
  typedef TOdfPercisionType                 OdfPrecisionType;

  typedef itk::SymRealSphericalHarmonicRep< TOdfPercisionType, NOrder >
                                            OdfPixelType;

  typedef itk::OdfReconImageFilter<
    GradientPixelType,
    OdfPixelType>                           OdfReconFilterType;


  typename OdfReconFilterType::Pointer odfReconFilter =
                                            OdfReconFilterType::New();


  typedef typename OdfReconFilterType::GradientImagesType   GradientImageType;

  typedef typename GradientImageType::RegionType            GradRegionType;
  typedef typename GradientImageType::SizeType              GradSizeType;
  typedef typename GradientImageType::IndexType             GradIndexType;

  typedef itk::Image<GradientPixelType, 4>                  InputImageType;
  typedef typename InputImageType::IndexType                Img4dIndexType;

  typedef itk::ImageFileReader< InputImageType >  ReaderType;

  typedef itk::ImageRegionIteratorWithIndex< GradientImageType > IteratorType;
  typedef typename GradientImageType::PixelType GradPixelType;

  typedef typename OdfReconFilterType::OutputImageType      OdfImageType;
  
  ///TODO extract Dimension out of ODFImageType;
  typedef itk::VectorImage
    <typename OdfImageType::PixelType::ComponentType, 3>
                                                            VectorImageType;

  typedef itk::VariableLengthVectorCastImageFilter<OdfImageType,VectorImageType> CasterType;
  
  typedef itk::ImageFileWriter< VectorImageType > WriterType;

  //make the gradiant table
  typename OdfReconFilterType::GradientDirectionContainerType::Pointer
      DiffusionVectors = OdfReconFilterType::GradientDirectionContainerType::New();

  //Process the gradient table...
  //read in each line and put it in a string stream to process...
  std::string line;
  ifstream bvecIn(bvecFile.c_str());
  getline(bvecIn,line);
  stringstream Xss(line);
  getline(bvecIn,line);
  stringstream Yss(line);
  getline(bvecIn,line);
  stringstream Zss(line);

  typename OdfReconFilterType::GradientDirectionType vect3d;

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

  typename GradientImageType::Pointer gradIm = GradientImageType::New();

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
    odfReconFilter->SetImageMask( spatialObjectMask );
  }
  
  // This is necessary until we fix netlib/dsvdc.c
  odfReconFilter->SetNumberOfThreads( 1 );

  odfReconFilter->SetGradientImage( DiffusionVectors, gradIm);

  if (verbose)
    {
      CommandProgressUpdate::Pointer observer = CommandProgressUpdate::New();
      odfReconFilter->AddObserver( itk::ProgressEvent(), observer );
    }

  if ( residue_flag)
  {
    odfReconFilter->SetCalculateResidualImage(true);
  }
  if ( normalize_flag )
  {
    odfReconFilter->SetNormalize(true);
  }
  else
  {
    odfReconFilter->SetNormalize(false);
  }
  
  if (verbose)
    printf("computing ODF image\n");
  odfReconFilter->Update();

  typename WriterType::Pointer writer = WriterType::New();

  itk::NiftiImageIO::Pointer imageIO = itk::NiftiImageIO::New();
  writer->SetImageIO(imageIO);
  writer->SetFileName( outputBasename+".hdr" );

  //We need to cast the odf output to vectorImage.
  typename CasterType::Pointer caster = CasterType::New();
  caster->SetInput(odfReconFilter->GetOutput() );

  if (verbose)
    printf("Casting ODF image\n");
  caster->Update();
  
  writer->SetInput( caster->GetOutput() );
  if (verbose)
    printf("Writing ODF image\n");

  writer->Update();

  if ( residue_flag )
  {  
    //Write out the Residual image....
    typedef itk::ImageFileWriter< typename OdfReconFilterType::ResidualImageType > ResidualWriterType;
    typename ResidualWriterType::Pointer residueWriter = ResidualWriterType::New();

    residueWriter->SetImageIO(itk::NiftiImageIO::New()); 
    residueWriter->SetFileName( outputBasename+"_Residue.hdr" );

    residueWriter->SetInput( odfReconFilter->GetResidualImage() );
    if (verbose)
      printf("Writing Residue image\n");

    residueWriter->Update();
  }
  
  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  string outputDir="";
  string dataFile,prefix;
  string bvecFile;
  string maskFile;

  int outputDir_flag = 0;
  int prefix_flag = 0;

  int residue_flag = 0;
  int normalize_flag = 1;
  
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
    {"maskFile",    required_argument,      0, 'm'},
    {"maxL",        required_argument,      0, 'l'},
    {"normalize",   required_argument,      0, 'n'},
    {"error",       no_argument,            0, 'e'}
  };

  int c, option_index = 0;
  int reqParams = 0;
  int maxL = 4;
  
  while ( (c = getopt_long (argc, argv, "uhVvo:d:p:r:t:m:l:en:",
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

      case 'e':
        residue_flag = 1;
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

  if ( reqParams != 5)
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
    std::cerr << "Output basename : " << outputBasename <<  std::endl;
    std::cerr << "Using mask      : " << maskFile <<  std::endl;
    std::cerr << "Max order L     : " << maxL <<  std::endl;
    std::cerr << std::endl;
  }

  /*
   * Start working
   */

 //Done Parsing Inputs lets now figure out What we want to do.
  itk::ImageIOBase::IOPixelType       dwiPixelType;
  itk::ImageIOBase::IOComponentType   dwiCompType;
  GetImageInfo(dataFile,&dwiPixelType,&dwiCompType);

  itk::ImageIOBase::IOPixelType       maskPixelType;
  itk::ImageIOBase::IOComponentType   maskCompType;
  GetImageInfo(maskFile,&maskPixelType,&maskCompType);

  //Die if it isn't scalar and unsigned char.
  if ( maskPixelType != itk::ImageIOBase::SCALAR
     || maskCompType !=  itk::ImageIOBase::SHORT  )
  {
    std::cerr << "Sorry the mask you supplied must be scalar unsigned char Image" << std::endl;
    std::cerr << "this is what I recieved for file : " << maskFile << std::endl;
    std::cerr << "      pixelType " << maskPixelType << std::endl;
    std::cerr << "      compType  " << maskCompType << std::endl;
    return EXIT_FAILURE;
  }

  //Die if it isn't scalar and int16.
  if ( dwiPixelType != itk::ImageIOBase::SCALAR
     || dwiCompType !=  itk::ImageIOBase::SHORT  )
  {
    std::cerr << "Sorry the Image you supplied must be scalar short Image" << std::endl;
    std::cerr << "this is what I recieved for file : " << dataFile << std::endl;
    std::cerr << "      pixelType " << dwiPixelType << std::endl;
    std::cerr << "      compType  " << dwiCompType << std::endl;
    return EXIT_FAILURE;
  }

  //Right now just support int16

  //<typename TPixelType, unsigned int NOrder=4, typename TOdfPercisionType = float, typename TMaskPixelType = unsigned char>
  try {
    switch ( maxL )
    {
      case 2:
        estimateOdf<short,2,float,short>( dataFile, bvecFile, maskFile, outputBasename, residue_flag, verbose, normalize_flag );
        break;
      case 4:
        estimateOdf<short,4,float,short>( dataFile, bvecFile, maskFile, outputBasename, residue_flag, verbose, normalize_flag );
        break;
      case 6:
        estimateOdf<short,6,float,short>( dataFile, bvecFile, maskFile, outputBasename, residue_flag, verbose, normalize_flag );
        break;
      case 8:
        estimateOdf<short,8,float,short>( dataFile, bvecFile, maskFile, outputBasename, residue_flag, verbose, normalize_flag );
        break;
      case 10:
        estimateOdf<short,10,float,short>( dataFile, bvecFile, maskFile, outputBasename, residue_flag, verbose, normalize_flag );
        break;
      case 12:
        estimateOdf<short,12,float,short>( dataFile, bvecFile, maskFile, outputBasename, residue_flag, verbose, normalize_flag );
        break;
      case 14:
        estimateOdf<short,14,float,short>( dataFile, bvecFile, maskFile, outputBasename, residue_flag, verbose, normalize_flag );
        break;
      case 16:
        estimateOdf<short,16,float,short>( dataFile, bvecFile, maskFile, outputBasename, residue_flag, verbose, normalize_flag );
        break;
      case 18:
        estimateOdf<short,18,float,short>( dataFile, bvecFile, maskFile, outputBasename, residue_flag, verbose, normalize_flag );
        break;
      case 20:
        estimateOdf<short,20,float,short>( dataFile, bvecFile, maskFile, outputBasename, residue_flag, verbose, normalize_flag );
        break;
      default:
        std::cout << "Unsupported dimension" << std::endl;
        exit( EXIT_FAILURE );
    }
  }
  catch (exception& e)
  {
    cout << "Standard exception: " << e.what() << endl;
  }

  return EXIT_SUCCESS;
}
