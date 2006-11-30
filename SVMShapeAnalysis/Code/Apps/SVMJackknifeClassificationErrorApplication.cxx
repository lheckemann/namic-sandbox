#include <itkVariableLengthVector.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <list>
#include <itkListSample.h>
#include <itkMembershipSample.h>
#include "SVMJackknifeClassificationErrorEstimator.h"
#include "SVMApplicationUtils.h"
#include <stdlib.h>
#include <time.h>
#include "itkSupportVectorMachine.h"
#include "itkSVMSolver1.h"

/**
 * Train and classify a support vector machine repeatedly, shuffeling
 * the data each time. Classification error is reported.  Run with
 * --help to see options and an example.
 */

// used to dodge microsoft compiler problem
#define jackknife_min(lhs,rhs) ((lhs) < (rhs) ? (lhs) : (rhs))

// copy typedefs from SVMApplicationUtils
typedef SVMApplicationUtils::MeasurementVectorType MeasurementVectorType;
typedef SVMApplicationUtils::SampleType            SampleType;
typedef SVMApplicationUtils::MembershipSampleType  MembershipSampleType;
typedef SVMApplicationUtils::SVMType               SVMType;
typedef SVMApplicationUtils::SVMSolverType         SVMSolverType;

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
            << "  -p | --kernelParameters         param1..." 
            << std::endl
            << "  -1 | --class1VectorFilenames    filenames..." 
            << std::endl
            << "  -2 | --class2VectorFilenames    filenames..." 
            << std::endl 
            << "  -j | --jackknifeIterations      iterations {1000}"
            << std::endl 
            << "  -t | --testSetSizes             N1 N2 N3..."
            << std::endl 
            << "  -s | --testSetSizeStride        stride {Num Vectors / 10}"
            << std::endl 
            << "  -o | --errorOutputFilename      filename"
            << std::endl 
            << "       --precomputeKernelMatrixOff"
            << std::endl 
            << "       --seedRandomOff"
            << std::endl << std::endl;

  std::cerr << "Kernel Parameters:" << std::endl << std::endl
            << "\tlinear: " << std::endl
            << "\t  no parameters, K(x,y) = x^t y" 
            << std::endl << std::endl
            << "\tpolynomial: " << std::endl
            << "\t  gamma r power, K(x,y) = (\\gamma x^t y + r)^{power}" 
            << std::endl << std::endl
            << "\trbf: " << std::endl
            << "\t  gamma,         K(x,y) = e^(-\\gamma \\| x - y \\|^2)" 
            << std::endl << std::endl;

  std::cerr << "Typical Use:" << std::endl << std::endl
            << progName 
            << "                                                         \\" 
            << std::endl
            << "--kernelType rbf "
            << "--kernelParameters 0.01                               \\"
            << std::endl 
            << "--class1VectorFilenames class1_* "
            << "--class2VectorFilenames class2_*      \\"
            << std::endl
            << "--jackknifeIterations 1000 --testSetSizeStride 9 "
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
  std::list<std::string>     class1VectorFilenames;
  std::list<std::string>     class2VectorFilenames;
  std::string                kernelType                      = "linear";
  std::vector<std::string>   kernelParameters;  
  unsigned long              jackknifeIterations             = 100;
  std::vector<unsigned int>  testSetSizes;
  std::vector<unsigned int>  trainingSetSizes;
  unsigned int               testSetSizeStride               = 0;
  bool                       precomputeKernelMatrix          = true;
  bool                       seedRandom                      = true;
  std::string                outputLevelString               = "standard";
  std::string                errorOutputFilename             = "";

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
    else if (std::string(argv[0]) == std::string("--errorOutputFilename") ||
             std::string(argv[0]) == std::string("-o"))
    {
      if (argc < 2) {
        printUsageError(progName, 
                        "errorOutputFilename flag requires one argument");
        exit(0);
      }
      errorOutputFilename = argv[1];
      argc--; argv++;
      argc--; argv++;
    }
    else if (std::string(argv[0]) == std::string("--testSetSizes") ||
             std::string(argv[0]) == std::string("-t"))
    {
      argc--; argv++;
      while (argc > 0 && argv[0][0] != '-')
      {
        testSetSizes.push_back(atoi(argv[0]));
        argc--; argv++;        
      }
    }
    else if (std::string(argv[0]) == std::string("--testSetSizeStride") ||
             std::string(argv[0]) == std::string("-s"))
    {
      if (argc < 2) {
        printUsageError(progName, 
                        "testSetSizeStride flag requires one argument");
        exit(0);
      }
      testSetSizeStride = atoi(argv[1]);
      argc--; argv++;
      argc--; argv++;
    }
    else if (std::string(argv[0]) == std::string("--kernelType") ||
             std::string(argv[0]) == std::string("-k"))
    {
      if (argc < 2) {
        printUsageError(progName, "kernelType flag requires one argument");
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
    else if (std::string(argv[0]) == std::string("--jackknifeIterations") ||
             std::string(argv[0]) == std::string("-j"))
    {
      if (argc < 2) {
        printUsageError(progName, 
                        "jackknifeIterations flag requires one argument");
        exit(0);
      }
      jackknifeIterations = atol(argv[1]);
      argc--; argv++;
      argc--; argv++;
    }    
    else if (std::string(argv[0]) ==std::string("--precomputeKernelMatrixOff"))
    {
      precomputeKernelMatrix = false;
      argc--; argv++;
    }    
    else if (std::string(argv[0]) ==std::string("--seedRandomOff"))
    {
      seedRandom = false;
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
  // check that necessary arguments were included
  // --a kernel
  // --at least one file for each class
  //
  if (kernelType.size() == 0)
  {
    std::cerr << "Error: a kernel type must be specified." << std::endl;
    printUsage(progName);
    exit(0);    
  }
  if (class1VectorFilenames.size() == 0)
  {
    std::cerr << "Error: Class 1 vector filenames must be specified." 
              << std::endl;
    printUsage(progName);
    exit(0);    
  }
  if (class2VectorFilenames.size() == 0)
  {
    std::cerr << "Error: Class 2 vector filenames must be specified." 
              << std::endl;
    printUsage(progName);
    exit(0);    
  }

  //
  // load vectors from file
  //
  // A Sample will hold measurement vectors from both classes.  A
  // MembershipSample is used to keep track of the class labels.
  //
  SampleType::Pointer sample                     = SampleType::New();
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
  unsigned int minClassSize = 
    jackknife_min(membershipSample->
                  GetClassSampleSize(SVMApplicationUtils::Class1Label),
                  membershipSample->
                  GetClassSampleSize(SVMApplicationUtils::Class2Label));

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
  // 100 is value used by Polina's code
  solver->SetC(100);

  //
  // setup the jackknife classification error estimator
  //
  typedef itk::Statistics::
    SVMJackknifeClassificationErrorEstimator<MembershipSampleType>
    JackknifeEstimatorType;
  JackknifeEstimatorType::Pointer jackknifeEstimator =
    JackknifeEstimatorType::New();

  // since we will use the same data over and over again, it will be
  // faster to generate the kernel matrix ourselves and just pass in
  // the data rather than have the solver generate the kernel matrices
  jackknifeEstimator->SetPrecomputeKernelMatrix(precomputeKernelMatrix);
  jackknifeEstimator->SetJackknifeIterations(jackknifeIterations);
  jackknifeEstimator->SetMembershipSample(membershipSample);
  jackknifeEstimator->SetSupportVectorMachine(svm);  
  jackknifeEstimator->SetSVMSolver(solver);

  //
  // the jackknife estimator will use the time as a random seed unless
  // another seed is explicitly specified
  //
  if (!seedRandom)
  {
    jackknifeEstimator->SetRandomSeed(0);
  }

  //
  // update test and training set sizes to leave N out
  //
  // if set sizes are not specified on the command line, use the
  // stride and the total number of vectors to generate them
  //
  if (testSetSizes.size() == 0)
  {
    if (testSetSizeStride == 0)
    {
      testSetSizeStride = minClassSize / 10 > 0 ? minClassSize / 10 : 1;
    }
    for (unsigned int N = 1; N < minClassSize; N += testSetSizeStride)
    {
      testSetSizes.push_back(N);
    }
  }
  for (std::vector<unsigned int>::const_iterator i = testSetSizes.begin();
       i != testSetSizes.end(); ++i)
  {
    trainingSetSizes.push_back(minClassSize-*i);
  }

  //
  // print some output
  //
  if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Standard)) 
  {
    std::cerr << "Loaded " << sample->Size() << " total measurement vectors."
              << std::endl;
    std::cerr << " " << membershipSample->
      GetClassSampleSize(SVMApplicationUtils::Class1Label)
              << " vectors from class 1." << std::endl;  
    std::cerr << " " << membershipSample->
      GetClassSampleSize(SVMApplicationUtils::Class2Label)
              << " vectors from class 2." << std::endl;  
    std::cerr << " Length of measurement vectors: " 
              << sample->GetMeasurementVectorSize() << std::endl;
    std::cerr << " Kernel type: " << svm->GetKernel()->GetNameOfClass()
              << std::endl;
    std::cerr << " Jackknife iterations: " << jackknifeIterations
              << std::endl;
    std::cerr << " Training/Test set sizes: ";
    for (unsigned int i = 0; i < testSetSizes.size(); ++i)
    {
      std::cerr << trainingSetSizes[i] << "/" << testSetSizes[i] << " ";
    }
    std::cerr << std::endl;
  }  

  //
  // run jackknife estimation for each training/test set size
  //  
  std::vector<double>       overallClassificationErrors;
  std::vector<double>       class1ClassificationErrors;  
  std::vector<double>       class2ClassificationErrors;  
  for (unsigned int i = 0; i < testSetSizes.size(); ++i)
  {
    unsigned int trainingSetSize = trainingSetSizes[i];
    unsigned int testSetSize = testSetSizes[i];

    if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Standard))
    {
      std::cerr << "size (train/test): ";
      std::cerr << std::setw(6) << std::setfill('0')
                << trainingSetSize << "/" 
                << std::setw(6) << std::setfill('0')
                << testSetSize << ", ";
    }

    //
    // set to leave-N-out
    //
    jackknifeEstimator->
      SetTrainingSetSize(0, trainingSetSize);
    jackknifeEstimator->SetTestSetSize(0, testSetSize);
    jackknifeEstimator->
      SetTrainingSetSize(1, trainingSetSize);
    jackknifeEstimator->SetTestSetSize(1, testSetSize);

    //
    // run jackknife estimator
    //
    jackknifeEstimator->Update();

    //
    // display results
    //
    const JackknifeEstimatorType::ConfusionMatrixType confusionMatrix =
      jackknifeEstimator->GetConfusionMatrix();

    // compute total classification error
    unsigned long totalClassifications = 
      confusionMatrix(0,0) + confusionMatrix(0,1) +
      confusionMatrix(1,0) + confusionMatrix(1,1);
    unsigned long totalErrors = confusionMatrix(0,1) + confusionMatrix(1,0);
    double totalErrorRatio = static_cast<double>(totalErrors) 
      / static_cast<double>(totalClassifications);
    overallClassificationErrors.push_back(totalErrorRatio);

    // comptute class 1 classification error
    unsigned long class1Classifications = 
      confusionMatrix(0,0) + confusionMatrix(0,1);
    unsigned long class1Errors = confusionMatrix(0,1);
    double class1ErrorRatio = static_cast<double>(class1Errors) 
      / static_cast<double>(class1Classifications);
    class1ClassificationErrors.push_back(class1ErrorRatio);

    // comptute class 2 classification error
    unsigned long class2Classifications = 
      confusionMatrix(1,0) + confusionMatrix(1,1);
    unsigned long class2Errors = confusionMatrix(1,0);
    double class2ErrorRatio = static_cast<double>(class2Errors) 
      / static_cast<double>(class2Classifications);
    class2ClassificationErrors.push_back(class2ErrorRatio);

    if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Standard))
    {
      std::cerr << "error (1/2/total): "
                << std::showpoint << std::fixed << std::setw(7)
                << class1ErrorRatio
                << "/" << class2ErrorRatio 
                << "/" << totalErrorRatio 
                << std::endl;
    }

    if (SVMApplicationUtils::OutputLevelAtLeast(SVMApplicationUtils::Verbose))
    {
      std::cerr << std::endl << "Confusion Matrix: " << std::endl;
      std::cerr << confusionMatrix << std::endl;
    }

    if (SVMApplicationUtils::
        OutputLevelAtLeast(SVMApplicationUtils::Everything))
    {
      const JackknifeEstimatorType::HistogramType misclassificationHistogram =
        jackknifeEstimator->GetMisclassificationHistogram();
      std::cerr << "Misclassification Histogram: " << std::endl;
      std::cerr << misclassificationHistogram << std::endl;
    }
  }

  //
  // write out the error ratios to a file
  //
  //
  // write labels to file
  //
  if (errorOutputFilename.size() != 0)
  {
    try
    {
      std::ofstream output(errorOutputFilename.c_str());
      output << "% training set size, test set size, class 1 error, "
             << "class 2 error, overall error" << std::endl;
      for (unsigned int i = 0; i < testSetSizes.size(); ++i)
      {
        output << trainingSetSizes[i] << " "
               << testSetSizes[i]     << " "
               << class1ClassificationErrors[i] << " "
               << class2ClassificationErrors[i] << " "
               << overallClassificationErrors[i] << std::endl;
      }
      if (output.bad()) throw;
      output.close();
    }
    catch (...)
    {
      std::cerr << "Error writing results to file." << std::endl;
      exit(0);
    }
  }  

  return 0;
}
