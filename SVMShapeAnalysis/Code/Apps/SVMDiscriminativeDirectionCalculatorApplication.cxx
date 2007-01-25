#include <itkVariableLengthVector.h>
#include <iostream>
#include <string>
#include <list>

#include "SVMDiscriminativeDirectionCalculator.h"
#include "SVMApplicationUtils.h"
#include "itkSVMFileReader.h"

/**
 * Compute the discriminative direction of a vector (or of the support
 * vectors) of a trained support vector machine.  Run with --help to
 * see options and an example.
 */

// copy typedefs from SVMApplicationUtils
typedef SVMApplicationUtils::MeasurementVectorType MeasurementVectorType;
typedef SVMApplicationUtils::SVMType               SVMType;
typedef SVMApplicationUtils::SVMReaderType         SVMReaderType;

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
            << "  -n | --normalizeOutputVectors   oneNorm---scale vectors to desired norm" 
            << std::endl 
            << "  -i | --vectorFilenames          filenames..." 
            << std::endl 
            << "  -o | --outputFilenamePrefix     prefix" 
            << std::endl 
            << "  -f | --vectorOutputFormat       ascii | {binary}" 
            << std::endl << std::endl;

  std::cerr << "The discriminative directions will be computed for the "
            << "support vectors " << std::endl
            << "if no vector filenames are specified.  If the output prefix is"
            << " /tmp/dd_" << std::endl
            << "then the first output vector will be stored as /tmp/dd_000.mvh"
            << " and" << std::endl
            << "/tmp/dd_000.raw.  If the ascii format is chosen, then the "
            << "file will be " << std::endl
            << "/tmp/dd_001.mva."
            << std::endl << std::endl;

  std::cerr << "Typical Use:" << std::endl << std::endl
            << progName << std::endl
            << "-n --svmFilename mySVM.mhd "
            << "--outputVectorFilename /tmp/mySVMDiscDir_"
            << std::endl << std::endl;
}

void printUsageError(const std::string& progName,
                     const std::string& errorString)
{
  std::cerr << "Error: " << errorString << std::endl;
  std::cerr << "Try " << progName << " --help for usage help." << std::endl;
}

void NormalizeL1(MeasurementVectorType& mv, double norm = 1)
{
  double l1normInitial = 0;
  for (unsigned int i = 0; i < mv.Size(); ++i)
  {
    l1normInitial += fabs(mv[i]);
  }
  for (unsigned int i = 0; i < mv.Size(); ++i)
  {
    mv[i] *= norm / l1normInitial;
  }        
}
                   
int main(int argc, char** argv)
{
  //
  // parse command line arguments
  //
  std::vector<std::string>   vectorFilenames;
  std::string                svmFilename;
  std::string                outputFilenamePrefix;
  std::string                outputLevelString              = "standard";
  std::string                vectorOutputFormat             = "binary";
  bool                       normalizeOutputVectors         = false;
  double                     vectorNorm                     = 1.0;

  std::string progName(SVMApplicationUtils::ExtractFilename(argv[0]));
  argc--; argv++;
  if (argc == 0)
  {
    printUsageError(progName, "No arguments");
    exit(0);
  }
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
    else if (std::string(argv[0]) == std::string("--normalizeOutputVectors") ||
             std::string(argv[0]) == std::string("-n"))
    {
      if (argc < 2) {
        printUsageError(progName, 
                        "normalizeVectorLength flag requires one argument");
        exit(0);
      }
      normalizeOutputVectors = true;
      vectorNorm = atof(argv[1]);
      argc--; argv++;
      argc--; argv++;
    }
    else if (std::string(argv[0]) == std::string("--vectorOutputFormat") ||
             std::string(argv[0]) == std::string("-f"))
    {
      if (argc < 2) {
        printUsageError(progName,
                        "vectorOutputFormat flag requires format argument");
        exit(0);
      }      
      vectorOutputFormat = argv[1];
      argc--; argv++;
      argc--; argv++;
    }    
    else if (std::string(argv[0]) == std::string("--svmFilename") ||
             std::string(argv[0]) == std::string("-s"))
    {
      if (argc < 2) {
        printUsageError(progName, 
                        "svmFilename flag requires one argument");
        exit(0);
      }
      svmFilename = argv[1];
      argc--; argv++;
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
    else if (std::string(argv[0]) == std::string("--outputFilenamePrefix") ||
             std::string(argv[0]) == std::string("-o"))             
    {
      if (argc < 2) {
        printUsageError(progName, 
                        "outputFilenamePrefix flag requires one argument");
        exit(0);
      }
      outputFilenamePrefix = argv[1];
      argc--; argv++;
      argc--; argv++;
    }
    else
    {
      printUsageError(progName,
                      std::string("Invalid command line argument: ") + 
                      std::string(argv[0])); 
      exit(0);
    }
  }

  //
  // set output level
  //
  SVMApplicationUtils::SetOutputLevel(outputLevelString);

  //
  // choose writing format
  //
  bool writeVectorsAsBinary = true;
  if (vectorOutputFormat == "ascii")
  {
    writeVectorsAsBinary = false;
  }

  //
  // check that necessary arguments were included
  // --a svm
  // --an output filename prefix
  //  
  if (svmFilename.size() == 0)
  {
    printUsageError(progName,
                    "no support vector machine file specified.");
    exit(0);    
  }  
  if (outputFilenamePrefix.size() == 0)
  {
    printUsageError(progName,
                    "an output file prefix must be specified.");
    exit(0);        
  }
  bool applyToSupportVectors = vectorFilenames.size() == 0;

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
  bool svmKernelIsLinear = 
    svm->GetKernel()->GetNameOfClass() == "LinearSVMKernel";

  //
  // setup discriminative direction calculator
  //
  typedef itk::Statistics::SVMDiscriminativeDirectionCalculator<SVMType>
    CalculatorType;
  CalculatorType::Pointer calculator = CalculatorType::New();
  calculator->SetSupportVectorMachine(svm.GetPointer());
  MeasurementVectorType
    discriminativeDirection(svm->GetMeasurementVectorSize());

  if (applyToSupportVectors)
  {
    for (unsigned int i = 0; i < svm->GetNumberOfSupportVectors(); ++i)
    {
      std::cerr << "Computing discriminative direction for SV " << i 
                << "...";
      //
      // compute the discriminative direction
      //
      double error;
      calculator->CalculateDiscriminativeDirection(i,
                                                   discriminativeDirection,
                                                   error);
      std::cerr << "[" << error << "]";

      //
      // if required, normalize the vector (l1 norm)
      //
      if (normalizeOutputVectors)
      {
        NormalizeL1(discriminativeDirection, vectorNorm);
      }

      //
      // write the direction vector to file
      //
      std::cerr << "Writing file...";
      try
      {
        std::ostringstream outputFilename;
        outputFilename << outputFilenamePrefix << i << ".mvh";
        SVMApplicationUtils::
          SaveMeasurementVectorToVectorFile(outputFilename.str(),
                                            discriminativeDirection,
                                            writeVectorsAsBinary);
      }
      catch (...)
      {
        std::cerr << "Error writing discriminative direction to file." 
                  << std::endl;
        exit(0);
      }
      std::cerr << "DONE" << std::endl;  

      if (svmKernelIsLinear)
      {
        std::cerr << std::endl
                  << "Linear kernel detected, discriminative directions will"
                  << " be identical." << std::endl;
        break;
      }
    }
  }
  else
  {
    unsigned int fileIndex = 0;
    for (std::vector<std::string>::const_iterator filename 
           = vectorFilenames.begin();
         filename != vectorFilenames.end(); ++filename)
    {
      std::cerr << "Loading vector " << fileIndex 
                << "...";
      //
      // load vector from file
      //
      MeasurementVectorType inputVector;
      try 
      {
        SVMApplicationUtils::
          LoadMeasurementVectorFromVectorFile(*filename, inputVector);
      }
      catch (...)
      {
        std::cerr << "Error loading vector from file: " 
                  << *filename << std::endl;
        exit(0);    
      }
      
      //
      // compute discriminative direction
      //
      std::cerr << "Computing discriminative direction...";
      double error;
      calculator->CalculateDiscriminativeDirection(inputVector,
                                                   discriminativeDirection,
                                                   error);
      std::cerr << "[" << error << "]";

      //
      // if required, normalize the vector (l1 norm)
      //
      if (normalizeOutputVectors)
      {
        NormalizeL1(discriminativeDirection, vectorNorm);
      }

      //
      // write the direction vector to file
      //
      std::cerr << "Writing file...";
      try
      {
        std::ostringstream outputFilename;
        outputFilename << outputFilenamePrefix << fileIndex++ << ".mvh";
        SVMApplicationUtils::
          SaveMeasurementVectorToVectorFile(outputFilename.str(),
                                            discriminativeDirection,
                                            writeVectorsAsBinary);
      }
      catch (...)
      {
        std::cerr << "Error writing discriminative direction to file." 
                  << std::endl;
        exit(0);
      }  
      std::cerr << "DONE" << std::endl;  

      if (svmKernelIsLinear)
      {
        std::cerr << std::endl
                  << "Linear kernel detected, discriminative directions will"
                  << " be identical." << std::endl;
        break;
      }
    }
  }

  return 0;
}
