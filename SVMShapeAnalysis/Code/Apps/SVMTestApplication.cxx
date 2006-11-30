#include <itkVariableLengthVector.h>
#include <iostream>
#include <string>
#include <list>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkListSample.h>
#include <itkMembershipSample.h>
#include <fstream>
#include "SVMApplicationUtils.h"

/**
 * Classify vectors using a trained support vector machine.  Run with
 * --help to see options and an example.
 */

// copy types from SVMApplicationUtils
typedef SVMApplicationUtils::MeasurementVectorType    MeasurementVectorType;
typedef SVMApplicationUtils::SampleType               SampleType;
typedef SVMApplicationUtils::MembershipSampleType     MembershipSampleType;
typedef SVMApplicationUtils::SVMType                  SVMType;
typedef SVMApplicationUtils::SVMReaderType            SVMReaderType;
typedef SVMApplicationUtils::ClassLabelType           ClassLabelType;

void printUsage(const std::string& progName) 
{
  std::cerr << "Usage: " 
            << progName 
            << std::endl << std::endl
            << "  -h | --help                     print this help and quit"
            << std::endl
            << "  -v | --outputLevel              silent | {standard} | verbose | everything"
            << std::endl
            << "  -s | --svmFilename              filename" 
            << std::endl 
            << "  -i | --vectorFilenames          filenames..." 
            << std::endl 
            << "  -o | --labelOutputFilename      filename" 
            << std::endl << std::endl;

  std::cerr << "Typical Use:" << std::endl << std::endl
            << progName << " "
            << "--svmFilename mySVM.mhd "
            << "--vectorFilenames myVectors_*       \\" 
            << std::endl
            << "--labelOutputFilename labels.txt"
            << std::endl << std::endl;
}

void printUsageError(const std::string& progName,
                     const std::string& errorString)
{
  std::cerr << "Error: " << errorString << std::endl;
  std::cerr << "Try " << progName << " --help for usage help." << std::endl;
}

int main(int argc, char** argv)
{
  //
  // parse command line arguments
  //
  std::list<std::string>   vectorFilenames;
  std::string              svmFilename;
  std::string              labelOutputFilename;
  std::string              outputLevelString        = "standard";

  std::string progName(SVMApplicationUtils::ExtractFilename(argv[0]));
  argc--; argv++;
  while (argc > 0)
  {
    if (std::string(argv[0]) == std::string("--help") ||
        std::string(argv[0]) == std::string("-h"))
    {
      printUsage(progName);
      exit(0);
    }
    else if (std::string(argv[0]) == std::string("--outputLevel") ||
             std::string(argv[0]) == std::string("-v"))
    {
      if (argc < 2 || (argc >= 2 && argv[1][0]=='-')) 
      {
        outputLevelString = "verbose";
      }      
      else 
      {
        outputLevelString = argv[1];
        argc--; argv++;
      }
      argc--; argv++;
    }
    else if (std::string(argv[0]) == std::string("--vectorFilenames") ||
             std::string(argv[0]) == std::string("-i"))             
    {
      argc--; argv++;
      while (argc > 0 && argv[0][0] != '-')
      {
        vectorFilenames.push_back(std::string(argv[0]));        
        argc--; argv++;        
      }
    }
    else if (std::string(argv[0]) == std::string("--svmFilename") ||
             std::string(argv[0]) == std::string("-s"))
    {
      if (argc < 2) {
        printUsageError(progName,
                        "svmFilename flag requires filename argument");
        exit(0);
      }
      svmFilename = argv[1];
      argc--; argv++;
      argc--; argv++;
    }
    else if (std::string(argv[0]) == std::string("--labelOutputFilename") ||
             std::string(argv[0]) == std::string("-o"))
    {
      if (argc < 2) {
        printUsageError(progName,
                        "labelOutputFilename flag requires filename argument");
        exit(0);
      }      
      labelOutputFilename = argv[1];
      argc--; argv++;
      argc--; argv++;
    }    
    else
    {
      printUsageError(progName,
                      std::string("unknown command line argument: ") +
                      std::string(argv[0]));
      exit(0);
    }
  }

  //
  // set output level
  //
  SVMApplicationUtils::SetOutputLevel(outputLevelString);

  //
  // check that necessary arguments were included
  // --at least one vector
  // --a svm
  //
  if (vectorFilenames.size() == 0)
  {
    printUsageError(progName,
                    "no vector filenames specified.");
    exit(0);    
  }
  if (svmFilename.size() == 0)
  {
    printUsageError(progName,
                    "no support vector machine file specified.");
    exit(0);    
  }

  //
  // load vectors from file
  //
  // A Sample will hold measurement vectors from both classes.  A
  // MembershipSample is used to keep track of the class labels.
  //
  SampleType::Pointer           sample           = SampleType::New();
  MembershipSampleType::Pointer membershipSample = MembershipSampleType::New();
  try 
  {
    SVMApplicationUtils::LoadUnclassifiedVectors(vectorFilenames, sample);
  }
  catch (...)
  {
    std::cerr << "Error loading vectors from file." << std::endl;
    exit(0);    
  }

  //
  // load the support vector machine from file
  //
  SVMType::Pointer svm;
  SVMReaderType::Pointer svmReader = SVMReaderType::New();
  svmReader->SetFileName(svmFilename);

  try
  {
    svmReader->Update();
    svm = svmReader->GetSVM();
  }
  catch (...)
  {
    std::cerr << "Error loading SVM from file." << std::endl;
    exit(0);
  }

  //
  // classify vectors using svm
  //
  std::vector<ClassLabelType> labels(sample->Size());
  unsigned int labelIndex = 0;
  for (SampleType::ConstIterator i = sample->Begin(); i != sample->End(); ++i)
  {
    if (SVMApplicationUtils::
        OutputLevelAtLeast(SVMApplicationUtils::Verbose)) 
    {
      std::cerr << std::setw(6) << std::setfill('0')
                << labelIndex << " --- "; 
    }
    double svmLabel = svm->Classify(i.GetMeasurementVector());
    labels[labelIndex] = (svmLabel < 0.0 
                          ? SVMApplicationUtils::Class2Label 
                          : SVMApplicationUtils::Class1Label);
    if (SVMApplicationUtils::
        OutputLevelAtLeast(SVMApplicationUtils::Verbose)) 
    {
      std::cerr << labels[labelIndex] << std::endl; 
    }
    ++labelIndex;
  }

  //
  // write labels to file
  //
  if (labelOutputFilename.size() != 0)
  {
    try
    {
      std::ofstream output(labelOutputFilename.c_str());
      std::copy(labels.begin(), labels.end(), 
                std::ostream_iterator<ClassLabelType>(output, " "));
      if (output.bad()) throw;
      output.close();
    }
    catch (...)
    {
      std::cerr << "Error writing labels to file." << std::endl;
      exit(0);
    }
  }

  return 0;
}
