
#include <iostream>
#include <getopt.h>
#include <sys/stat.h>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkNiftiImageIO.h"
#include "itkSymRealSphericalHarmonicRep.h"
#include "itkSymRSHPowerImageFilter.h"
#include "itkImageIOFactory.h"

#include "itkCommand.h"


#define INDENT "\t"
#define EXEC_NAME "computeRshPowers"
#define SVN_FILE_VERSION "$Id: computeRshPowers.cxx 188 2009-10-13 18:23:41Z bloyl@UPHS.PENNHEALTH.PRV $"

#ifndef RELEASE_ID
#define RELEASE_ID "0.0_super_alpha"
#endif

#ifndef SVN_REV
#define SVN_REV "NO_SVN"
#endif

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

using namespace std;

static int verbose = 0;
static int normalize = 0;

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

// echoUsage:display usage information
void echoUsage() {
    std::cerr
    << EXEC_NAME << "--\n"
    << INDENT << EXEC_NAME << " Computes the Power in each order (l level) of the Rsh domain.\n\n"
    << "Usage: "<< EXEC_NAME<<" [options]\n"
    << "Required Options:\n"
    << INDENT << "[-d --dataFile]     Specify the input RSH data file (required)\n"
    << INDENT << "[-p --prefix]       Prefix for result file\n"
    << "Options:\n"
    << INDENT << "[--normalize,   -n] Normalize the output. Effectively compute fractional power\n"
    << INDENT << "                    Default is off\n"
    << INDENT << "[--usage,   -u]     Display this message\n"
    << INDENT << "[--help,    -h]     Display this message\n"
    << INDENT << "[--Version, -V]     Display version information\n"
    << INDENT << "[--verbose, -v]     Turns on verbose output\n\n"
    << INDENT << "[-o --outputDir]    The output directory to write the results\n"
    << INDENT << "                    Defaults to the location of the input file\n\n";
}

// Main runner
// There will be 2 more removing various template parameters.
template <typename TOdfPercisionType, unsigned int TOdfOrder,
        unsigned int TImageDimension >
int computePowerRunner(string odfFileName, string outputBasename)
{

  typedef itk::SymRealSphericalHarmonicRep< TOdfPercisionType,TOdfOrder>
                                                      OdfPixelType;

  typedef itk::Image<OdfPixelType,TImageDimension>    OdfImageType;

  typedef itk::SymRSHPowerImageFilter<OdfImageType>   FilterType;
  typedef typename FilterType::OutputImageType        OutputImageType;

  //setup reader, writer and filter...
  typedef itk::ImageFileReader< OdfImageType > ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(odfFileName);

  typedef itk::ImageFileWriter< OutputImageType > WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetImageIO( itk::NiftiImageIO::New() );
  writer->SetFileName( outputBasename+".hdr" );

  typename FilterType::Pointer filter = FilterType::New();
  
  if (normalize)
    filter->NormalizeOn();
  
  filter->SetInput( reader->GetOutput() );
  writer->SetInput ( filter->GetOutput() );

  try
  {
    if (verbose)
    {
      std::cerr << "Beginning pipeline!" << std::endl;
      CommandProgressUpdate::Pointer observer = CommandProgressUpdate::New();
      filter->AddObserver( itk::ProgressEvent(), observer );
    }

    writer->Update();

    return EXIT_SUCCESS;
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

}

template <typename TOdfPercisionType, unsigned int TOdfOrder>
int computePowerRunnerOverDim
(
  string odfFileName, string outputBaseName
  ,unsigned int ImageDimension
)
{
  switch ( ImageDimension )
  {
    //~ case 1: //HUH
      //~ return computePowerRunner<TOdfPercisionType,TOdfOrder,1>(odfFileName,outputBaseName);
      //~ break;
    //~ case 2:
      //~ return computePowerRunner<TOdfPercisionType,TOdfOrder,2>(odfFileName,outputBaseName);
      //~ break;
    case 3:
      return computePowerRunner<TOdfPercisionType,TOdfOrder,3>(odfFileName,outputBaseName);
      break;
    default:
      std::cout << "Unsupported Image Dimension : " << ImageDimension << std::endl;
      return EXIT_FAILURE;
  }
}

template <typename TOdfPercisionType>
int computePowerRunnerOverDimAndOrder
(
  string odfFileName, string outputBaseName
  ,unsigned int ImageDimension, unsigned int OdfOrder
)
{
  switch ( OdfOrder )
  {
    //~ case 0: //HUH
      //~ return computePowerRunnerOverDim<TOdfPercisionType,0>(odfFileName,outputBaseName,ImageDimension);
    case 2:
      return computePowerRunnerOverDim<TOdfPercisionType,2>(odfFileName,outputBaseName,ImageDimension);
    case 4:
      return computePowerRunnerOverDim<TOdfPercisionType,4>(odfFileName,outputBaseName,ImageDimension);
    case 6:
      return computePowerRunnerOverDim<TOdfPercisionType,6>(odfFileName,outputBaseName,ImageDimension);
    case 8:
      return computePowerRunnerOverDim<TOdfPercisionType,8>(odfFileName,outputBaseName,ImageDimension);
    case 10:
      return computePowerRunnerOverDim<TOdfPercisionType,10>(odfFileName,outputBaseName,ImageDimension);
    case 12:
      return computePowerRunnerOverDim<TOdfPercisionType,12>(odfFileName,outputBaseName,ImageDimension);
    case 14:
      return computePowerRunnerOverDim<TOdfPercisionType,14>(odfFileName,outputBaseName,ImageDimension);
    case 16:
      return computePowerRunnerOverDim<TOdfPercisionType,16>(odfFileName,outputBaseName,ImageDimension);
    case 18:
      return computePowerRunnerOverDim<TOdfPercisionType,18>(odfFileName,outputBaseName,ImageDimension);
    case 20:
      return computePowerRunnerOverDim<TOdfPercisionType,20>(odfFileName,outputBaseName,ImageDimension);
    default:
      std::cout << "Unsupported RSH ORDER : " << OdfOrder << std::endl;
      return EXIT_FAILURE;
  }
}

int computePowerRunnerOverAll
(
  string odfFileName, string outputBaseName
)
{
  //read in odfFileName and figure out the pixeltype, imageDimension and Order
  itk::ImageIOBase::Pointer imageIO;
  imageIO = itk::ImageIOFactory::CreateImageIO( odfFileName.c_str(),
                                        itk::ImageIOFactory::ReadMode);

  if ( imageIO )
  {
    imageIO->SetFileName(odfFileName);
    imageIO->ReadImageInformation();

    //~ itk::ImageIOBase::IOPixelType pixelType = imageIO->GetPixelType();
    itk::ImageIOBase::IOComponentType componentType = imageIO->GetComponentType();

    unsigned int imageDimensions = 3; //Will need to get this out somehow?

    //Get the number of componants per pixel and compute the Order.
    unsigned int nComps = imageIO->GetNumberOfComponents();

    //Find the right order... just search
    int l;
    for(l = 0; (pow(static_cast<float>(l),2) + l + 2)/2.0 - l <= nComps; l=l+2);
    l = l-2;

    //switch over the componentType
    switch ( componentType )
    {
      //~ case itk::ImageIOBase::CHAR:
      //~ {
        //~ return computePowerRunnerOverDimAndOrder<char>
          //~ (odfFileName,outputBaseName,imageDimensions,l);
      //~ }
      //~ case itk::ImageIOBase::UCHAR:
      //~ {
        //~ return computePowerRunnerOverDimAndOrder<unsigned char>
          //~ (odfFileName,outputBaseName,imageDimensions,l);
      //~ }
      //~ case itk::ImageIOBase::USHORT:
      //~ {
        //~ return computePowerRunnerOverDimAndOrder<unsigned short>
          //~ (odfFileName,outputBaseName,imageDimensions,l);
      //~ }
      case itk::ImageIOBase::SHORT:
      {
        return computePowerRunnerOverDimAndOrder<short>
          (odfFileName,outputBaseName,imageDimensions,l);
      }
      //~ case itk::ImageIOBase::UINT:
      //~ {
        //~ return computePowerRunnerOverDimAndOrder<unsigned int>
          //~ (odfFileName,outputBaseName,imageDimensions,l);
      //~ }
      case itk::ImageIOBase::INT:
      {
        return computePowerRunnerOverDimAndOrder<int>
          (odfFileName,outputBaseName,imageDimensions,l);
      }
      //~ case itk::ImageIOBase::ULONG:
      //~ {
        //~ return computePowerRunnerOverDimAndOrder<unsigned long>
          //~ (odfFileName,outputBaseName,imageDimensions,l);
      //~ }
      //~ case itk::ImageIOBase::LONG:
      //~ {
        //~ return computePowerRunnerOverDimAndOrder<long>
          //~ (odfFileName,outputBaseName,imageDimensions,l);
      //~ }
      case itk::ImageIOBase::FLOAT:
      {
        return computePowerRunnerOverDimAndOrder<float>
          (odfFileName,outputBaseName,imageDimensions,l);
      }
      case itk::ImageIOBase::DOUBLE:
      {
        return computePowerRunnerOverDimAndOrder<double>
          (odfFileName,outputBaseName,imageDimensions,l);
      }
      default:
      {
        std::cout << "Unsupported componant Type : " << std::endl;
        return EXIT_FAILURE;
      }
    }
  }
  else
  {
    std::cout << "Could not read the input image information from " <<
        odfFileName << std::endl;
    return EXIT_FAILURE;
  }
}

int main(int argc, char** argv)
{
  string outputDir="";
  string dataFile,prefix;

  int outputDir_flag = 0;
  int prefix_flag = 0;

  static struct option long_options[] =
  {
    {"usage",       no_argument,            0, 'u'},
    {"help",        no_argument,            0, 'h'},
    {"Version",     no_argument,            0, 'V'},
    {"verbose",     no_argument,            0, 'v'},
    {"normlaize",   no_argument,            0, 'n'},
    {"outputDir",   required_argument,      0, 'o'},
    {"dataFile",    required_argument,      0, 'd'},
    {"prefix",      required_argument,      0, 'p'}
  };

  int c, option_index = 0;
  int reqParams = 0;
  while ( (c = getopt_long (argc, argv, "uhVvno:d:p:",
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

      case 'n':
        normalize = 1;
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

      case '?':
        /* getopt_long already printed an error message. */
        break;

      default:
        abort ();

    }
  }

  if ( reqParams < 2)
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
    outputBasename += bName+"_powers";

 //check for the existence of the input files.
  if (!fileExists(dataFile))
  {
    std::cerr << dataFile << " --- Doesn't exist!!!\nExiting!\n\n" << std::endl;
    echoUsage();
    return EXIT_FAILURE;
  }

  if (verbose)
  {
    std::cerr << "dataFile        : " << dataFile << std::endl;
    std::cerr << "outputBaseName  : " << outputBasename << std::endl;
    if (normalize)
      std::cerr << "Normalization On"<< std::endl;
    else
      std::cerr << "Normalization Off"<< std::endl;
  }

  return computePowerRunnerOverAll( dataFile, outputBasename );
}

