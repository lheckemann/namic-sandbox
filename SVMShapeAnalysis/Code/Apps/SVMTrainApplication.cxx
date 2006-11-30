#include <itkVariableLengthVector.h>
#include <iostream>
#include <string>
#include <list>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkListSample.h>
#include <itkMembershipSample.h>

#include "SVMApplicationUtils.h"
#include "itkSupportVectorMachine.h"
#include "itkSVMSolver1.h"

/**
 * Train a support vector machine from example measurement vectors.
 * Run with --help to see options and an example.
 */

// copy types from SVMApplicationUtils
typedef SVMApplicationUtils::MeasurementVectorType    MeasurementVectorType;
typedef SVMApplicationUtils::SampleType               SampleType;
typedef SVMApplicationUtils::MembershipSampleType     MembershipSampleType;
typedef SVMApplicationUtils::SVMType                  SVMType;
typedef SVMApplicationUtils::SVMSolverType            SVMSolverType;
typedef SVMApplicationUtils::SVMWriterType            SVMWriterType;

void printUsage(const std::string& progName) 
{
  std::cerr << "Usage: " 
            << progName 
            << std::endl << std::endl
            << "  -h | --help                     print this help and quit"
            << std::endl
            << "  -v | --outputLevel              silent | {standard} | verbose | everything"
            << std::endl
            << "  -k | --kernelType               {linear} | polynomial | rbf" 
            << std::endl
            << "  -p | --kernelParameters         params..." 
            << std::endl
            << "  -1 | --class1VectorFilenames    filenames..." 
            << std::endl
            << "  -2 | --class2VectorFilenames    filenames..." 
            << std::endl 
            << "  -o | --svmOutputFilename        filename" 
            << std::endl 
            << "  -f | --svmOutputFormat          ascii | {binary} | sketch" 
            << std::endl << std::endl;

  std::cerr << "Kernel Parameters:" << std::endl << std::endl
            << "\tlinear: " << std::endl
            << "\t  no parameters, K(x,y) = x^t y" 
            << std::endl << std::endl
            << "\tpolynomial: " << std::endl
            << "\t  gamma r power, K(x,y) = (\\gamma x^t y + r)^{power}" 
            << std::endl << std::endl
            << "\trbf: " << std::endl
            << "\t  gamma, K(x,y) = e^(-\\gamma \\| x - y \\|^2)" 
            << std::endl << std::endl;

  std::cerr << "Typical Use:" << std::endl << std::endl
            << progName << " "
            << "--kernelType rbf "
            << "--kernelParameters 0.01                       \\"
            << std::endl 
            << "--svmOutputFilename mySVMFile                                             \\"
            << std::endl 
            << "--class1VectorFilenames class1_* "
            << "--class2VectorFilenames class2_*         "
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
  std::list<std::string>   class1VectorFilenames;
  std::list<std::string>   class2VectorFilenames;
  std::string              kernelType              = "linear";
  std::vector<std::string> kernelParameters;  
  std::string              svmOutputFilename;
  std::string              outputLevelString       = "standard";
  std::string              svmOutputFormat         = "binary";

  
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
    else if (std::string(argv[0]) == std::string("--class1VectorFilenames") ||
             std::string(argv[0]) == std::string("-1"))
    {
      argc--; argv++;
      while (argc > 0 && argv[0][0] != '-')
      {
        class1VectorFilenames.push_back(std::string(argv[0]));        
        argc--; argv++;        
      }
    }
    else if (std::string(argv[0]) == std::string("--class2VectorFilenames") ||
             std::string(argv[0]) == std::string("-2"))
    {
      argc--; argv++;
      while (argc > 0 && argv[0][0] != '-')
      {
        class2VectorFilenames.push_back(std::string(argv[0]));        
        argc--; argv++;        
      }
    }
    else if (std::string(argv[0]) == std::string("--kernelType") ||
             std::string(argv[0]) == std::string("-k"))
    {
      if (argc < 2) {
        printUsageError(progName,
                        "kernelType flag requires a kernel type argument");
        exit(0);
      }
      kernelType = argv[1];
      argc--; argv++;
      argc--; argv++;
    }
    else if (std::string(argv[0]) == std::string("--kernelParameters") ||
             std::string(argv[0]) == std::string("-p"))
    {
      argc--; argv++;
      while (argc > 0 && argv[0][0] != '-')
      {
        kernelParameters.push_back(std::string(argv[0]));        
        argc--; argv++;        
      }
    }    
    else if (std::string(argv[0]) == std::string("--svmOutputFilename") ||
             std::string(argv[0]) == std::string("-o"))
    {
      if (argc < 2) {
        printUsageError(progName,
                        "svmOutputFilename flag requires filename argument");
        exit(0);
      }
      svmOutputFilename = argv[1];
      argc--; argv++;
      argc--; argv++;
    }
    else if (std::string(argv[0]) == std::string("--svmOutputFormat") ||
             std::string(argv[0]) == std::string("-f"))
    {
      if (argc < 2) {
        printUsageError(progName,
                        "svmOutputFormat flag requires format argument");
        exit(0);
      }      
      svmOutputFormat = argv[1];
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
  // --a kernel
  // --at least one file for each class
  //
  if (kernelType.size() == 0)
  {
    printUsageError(progName,
                    "a kernel type must be specified.");
    exit(0);    
  }
  if (class1VectorFilenames.size() == 0)
  {
    printUsageError(progName,
                    "no class 1 vector filenames specified.");
    exit(0);    
  }
  if (class2VectorFilenames.size() == 0)
  {
    printUsageError(progName,
                    "no class 2 vector filenames specified.");
    exit(0);    
  }
  if (svmOutputFilename.size() == 0)
  {
    printUsageError(progName,
                    "no support vector machine output file specified.");
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
    SVMApplicationUtils::LoadClassifiedVectors(class1VectorFilenames, 
                                               class2VectorFilenames,
                                               sample, membershipSample);
  }
  catch (...)
  {
    std::cerr << "Error loading vectors." << std::endl;
    exit(0);
  }
  

  //
  // create a support vector machine
  //
  SVMType::Pointer svm = SVMType::New();

  //
  // setup the kernel for the svm
  //
  bool didSetupKernel = 
    SVMApplicationUtils::ParseSVMKernel(svm, kernelType, kernelParameters);
  if (!didSetupKernel)
  {
    printUsageError(progName,
                    std::string("Could not parse kernel: ") + kernelType);
    
    exit(0);
  }

  //
  // setup the SVM solver
  //
  SVMSolverType::Pointer solver = SVMSolverType::New();
  solver->
    SetClass1TrainingSamples(membershipSample->
                             GetClassSample(SVMApplicationUtils::Class1Label));
  solver->
    SetClass2TrainingSamples(membershipSample->
                             GetClassSample(SVMApplicationUtils::Class2Label));
  solver->SetSVM(svm);

  //
  // print some output
  //
  if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Standard)) 
  {
    std::cerr << "Loaded " << sample->Size() << " total measurement vectors."
              << std::endl;
    std::cerr << membershipSample->
      GetClassSampleSize(SVMApplicationUtils::Class1Label)
              << " vectors from class 1." << std::endl;  
    std::cerr << membershipSample->
      GetClassSampleSize(SVMApplicationUtils::Class2Label)
              << " vectors from class 2." << std::endl;  
    std::cerr << "Length of measurement vectors: " 
              << sample->GetMeasurementVectorSize() << std::endl;
    std::cerr << "Kernel type: " << svm->GetKernel()->GetNameOfClass()
              << std::endl;
  }  

  //
  // train the support vector machine
  //
  if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Verbose)) 
  {
    std::cerr << "Training...";
  }
  solver->SetC(100);
  solver->Update();
  if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Verbose)) 
  {
    std::cerr << "DONE" << std::endl;  
  }
  if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Standard)) 
  {
    std::cerr << "SVM has " << svm->GetNumberOfSupportVectors() 
              << " support vectors" << std::endl;
  }
  if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Verbose)) 
  {
    std::cerr << "Instance Identifiers:" << std::endl;
    for (unsigned int i = 0; i < svm->GetNumberOfSupportVectors(); ++i)
    {
      std::cerr << solver->GetSupportVectorInstanceIdentifier(i) << " ";
    }
    std::cerr << std::endl;
  }
  
  //
  // write SVM configuration to file
  //
  SVMWriterType::Pointer svmWriter = SVMWriterType::New();
  svmWriter->SetFileName(svmOutputFilename.c_str());
  if (svmOutputFormat == "ascii")
  {
    svmWriter->SetWriteASCII(true);
  }
  else 
  {
    svmWriter->SetWriteASCII(false);
  }
  svmWriter->SetInput(svm);
  if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Verbose)) 
  {
    std::cerr << "Writing SVM...";
  }
  try 
  {
    svmWriter->Update();
    if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Verbose)) 
    {
      std::cerr << "DONE" << std::endl;  
    }
  }
  catch (...)
  {
    std::cerr << std::endl << "Error writing SVM to file." << std::endl;
  }

  return 0;
}
