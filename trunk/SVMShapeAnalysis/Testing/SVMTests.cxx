#include <string>
#include <list>
#include <vector>
#include <sstream>
#include "SVMTestingUtils.h"
#include "SVMDiscriminativeDirectionCalculator.h"
#include "SVMJackknifeClassificationErrorEstimator.h"
#include <fstream>

const std::string DATA_DIRECTORY = 
"/usr/local/dev/MITGollandDist/Data";
//"C:\\work\\dev\\MITGollandDist\\Data";

const double ASCII_EPSILON  = 0.000001;
const double BINARY_EPSILON = 0.000000000001;

/**
 * If lhs and rhs are closer than epsilon, return true.  Otherwise
 * report the difference and return false.
 */
bool FloatingPointEqual(const double& lhs, const double& rhs, 
                        const double& epsilon = 0.0)
{
  if (fabs(lhs-rhs) <= epsilon)
  {
    return true;
  }
  else
  {
    std::cerr 
      << std::endl
      << lhs << " != " << rhs << std::endl
      << "lhs-rhs = " << lhs-rhs << std::endl
      << "epsilon = " << epsilon << std::endl;
    return false;
  }
}

/**
 * Compare the arrays elementwise, if all pairs are closer than
 * epsilon, return true.  Otherwise report the difference and return
 * false.
 */
template <class ArrayType1, class ArrayType2>
bool FloatingPointArrayEqual(const ArrayType1& v1,
                             const ArrayType2& v2,
                             unsigned int length,
                             double epsilon = 0.0)
{
  for (unsigned int i = 0; i < length; ++i)
  {
    if (!FloatingPointEqual(v1[i], v2[i], epsilon))
    {
      std::cerr << "index = " << i << std::endl;
      return false;
    }
  }
  return true;
}

int main(int argc, char** argv)
{
  typedef std::list<std::string> SList;
  
  //
  // create gold standard svm filenames and associated kernels
  //
  SVMTestingUtils::FilenameList         goldStandardSVMFilenames;
  SVMTestingUtils::FilenameList         goldStandardClassificationFilenames;
  SVMTestingUtils::FilenameList         goldStandardHistogramFilenames;
  std::list<std::string>                kernelTypes;
  std::list<std::vector<std::string> >  kernelParams;
  std::list<std::vector<std::string> >  
    goldStandardDiscriminativeDirectionFilenames;

  // linear kernel
  goldStandardSVMFilenames.
    push_back(DATA_DIRECTORY + "/Testing/svmLinear01_binary.svm");
  goldStandardClassificationFilenames.
    push_back(DATA_DIRECTORY + "/Testing/svmLinear01_class.txt");
  goldStandardHistogramFilenames.
    push_back(DATA_DIRECTORY + "/Testing/svmLinear01_hist.txt");
  kernelTypes.push_back("linear");
  std::vector<std::string> linearParams;
  kernelParams.push_back(linearParams);
  std::vector<std::string> linearDDFilenames;
  linearDDFilenames.
    push_back(DATA_DIRECTORY + "/Testing/svmLinear01_DD_binary_0.mvh");
  goldStandardDiscriminativeDirectionFilenames.
    push_back(linearDDFilenames);

  // polynomial kernel
  goldStandardSVMFilenames.push_back(DATA_DIRECTORY + 
                                     "/Testing/svmPolynomial01_binary.svm");
  goldStandardClassificationFilenames.
    push_back(DATA_DIRECTORY + "/Testing/svmPolynomial01_class.txt");
  goldStandardHistogramFilenames.
    push_back(DATA_DIRECTORY + "/Testing/svmPolynomial01_hist.txt");
  kernelTypes.push_back("polynomial");
  std::vector<std::string> polynomialParams;
  // cubic polynomial
  polynomialParams.push_back("0.01");
  polynomialParams.push_back("1.0");
  polynomialParams.push_back("2.0");
  kernelParams.push_back(polynomialParams);
  std::vector<std::string> polynomialDDFilenames;
  goldStandardDiscriminativeDirectionFilenames.
    push_back(polynomialDDFilenames);

  // rbf kernel
  goldStandardSVMFilenames.push_back(DATA_DIRECTORY + 
                                     "/Testing/svmRBF01_binary.svm");
  goldStandardClassificationFilenames.
    push_back(DATA_DIRECTORY + "/Testing/svmRBF01_class.txt");
  goldStandardHistogramFilenames.
    push_back(DATA_DIRECTORY + "/Testing/svmRBF01_hist.txt");
  kernelTypes.push_back("rbf");
  std::vector<std::string> rbfParams;
  rbfParams.push_back("0.0001");
  kernelParams.push_back(rbfParams);
  std::vector<std::string> rbfDDFilenames;
  for (int i = 0; i < 23; i++)
  {
    std::ostringstream oss;
    oss << DATA_DIRECTORY << "/Testing/svmRBF01_DD_binary_" << i << ".mvh";
    rbfDDFilenames.push_back(oss.str());
  }
  goldStandardDiscriminativeDirectionFilenames.
    push_back(rbfDDFilenames);
  
  //
  // create input vector filenames
  //
  unsigned int numTrainingVectorsPerClass = 40; // must be <= 100
  std::string vectorDir = DATA_DIRECTORY + "/UCI-ML-HandwritingDataset/META/";
  SVMTestingUtils::FilenameList inputTrainingVectorFilenames1;
  SVMTestingUtils::FilenameList inputTrainingVectorFilenames2;
  SVMTestingUtils::FilenameList inputTestingVectorFilenames1;
  SVMTestingUtils::FilenameList inputTestingVectorFilenames2;
  // add 1s (images of handwritten 1s)
  for (unsigned int i = 0; i < numTrainingVectorsPerClass; ++i)
  {
    std::ostringstream oss;
    oss << vectorDir << "1_0" 
        << std::setw(2) << std::setfill('0') << i 
        << ".mvh";
    inputTrainingVectorFilenames1.push_back(oss.str());
    oss.str("");
    oss << vectorDir << "1_1" 
        << std::setw(2) << std::setfill('0') << i 
        << ".mvh";
    inputTestingVectorFilenames1.push_back(oss.str());
  }
  // add 7s (images of handwritten 7s)
  for (unsigned int i = 0; i < numTrainingVectorsPerClass; ++i)
  {
    std::ostringstream oss;
    oss << vectorDir << "7_0" 
        << std::setw(2) << std::setfill('0') << i 
        << ".mvh";
    inputTrainingVectorFilenames2.push_back(oss.str());
    oss.str("");
    oss << vectorDir << "7_1" 
        << std::setw(2) << std::setfill('0') << i 
        << ".mvh";
    inputTestingVectorFilenames2.push_back(oss.str());
  }

  //
  // load input vectors
  //
  std::cerr << "Loading input vectors...";
  SVMTestingUtils::SampleType::Pointer sample = 
    SVMTestingUtils::SampleType::New();
  SVMTestingUtils::MembershipSampleType::Pointer membershipSample = 
    SVMTestingUtils::MembershipSampleType::New();
  SVMTestingUtils::LoadClassifiedVectors(inputTrainingVectorFilenames1, 
                                         inputTrainingVectorFilenames2,
                                         sample, membershipSample);

  SVMTestingUtils::SampleType::Pointer testingSample = 
    SVMTestingUtils::SampleType::New();
  SVMTestingUtils::MembershipSampleType::Pointer testingMembershipSample = 
    SVMTestingUtils::MembershipSampleType::New();
  SVMTestingUtils::LoadClassifiedVectors(inputTestingVectorFilenames1, 
                                         inputTestingVectorFilenames2,
                                         testingSample, 
                                         testingMembershipSample);
  std::cerr << "DONE" << std::endl;

  SVMTestingUtils::FilenameList::const_iterator         svmFilenameIter  =
    goldStandardSVMFilenames.begin();
  std::list<std::string>::const_iterator                kernelTypeIter   =
    kernelTypes.begin();
  std::list<std::vector<std::string> >::const_iterator  kernelParamsIter =
    kernelParams.begin();

  SVMTestingUtils::FilenameList::const_iterator 
    goldStandardClassificationFilenameIter  =
    goldStandardClassificationFilenames.begin();

  SVMTestingUtils::FilenameList::const_iterator 
    goldStandardHistogramFilenameIter  =
    goldStandardHistogramFilenames.begin();

  std::list<std::vector<std::string> >::const_iterator  
    goldStandardDDFilenameIter =
    goldStandardDiscriminativeDirectionFilenames.begin();

  while (svmFilenameIter != goldStandardSVMFilenames.end())
  {
    std::cerr << "%%%%%%%%%%%%%%%%%%%%" << std::endl;
    std::cerr << *kernelTypeIter << " ";
    std::copy(kernelParamsIter->begin(), kernelParamsIter->end(), 
              std::ostream_iterator<std::string>(std::cerr, " "));
    std::cerr << std::endl;

    //
    // create a support vector machine
    //
    SVMTestingUtils::SVMType::Pointer svm = SVMTestingUtils::SVMType::New();

    //
    // setup the kernel for the svm
    //
    std::cerr << "Parsing kernel...";
    std::string kernelType = "linear";
    std::vector<std::string> kernelParameters;
    SVMTestingUtils::ParseSVMKernel(svm, *kernelTypeIter,
                                        *kernelParamsIter);
    std::cerr << "DONE" << std::endl;

    //
    // setup the SVM solver and train
    //
    std::cerr << "Setting up solver...";
    SVMTestingUtils::SVMSolverType::Pointer solver = 
      SVMTestingUtils::SVMSolverType::New();
    solver->
      SetClass1TrainingSamples(membershipSample->
                               GetClassSample(SVMTestingUtils::
                                              Class1Label));
    solver->
      SetClass2TrainingSamples(membershipSample->
                               GetClassSample(SVMTestingUtils::
                                              Class2Label));
    solver->SetSVM(svm);
    solver->SetC(100);
    std::cerr << "DONE" << std::endl;
    std::cerr << "Training...";
    solver->Update();  
    std::cerr << "DONE" << std::endl;

    //
    // load gold standard svm
    //
    std::cerr << "Loading gold standard svm...";
    SVMTestingUtils::SVMType::Pointer svmGoldStandard;
    SVMTestingUtils::SVMReaderType::Pointer svmReader = 
      SVMTestingUtils::SVMReaderType::New();
    svmReader->SetFileName(*svmFilenameIter);
    svmReader->Update();
    svmGoldStandard = svmReader->GetSVM();
    std::cerr << "DONE" << std::endl;

    //
    // compare number of support vectors
    //
    std::cerr << "Testing num SVs...";
    if (svm->GetNumberOfSupportVectors() != 
        svmGoldStandard->GetNumberOfSupportVectors())
    {
      std::cerr << std::endl
                << svm->GetNumberOfSupportVectors() << " != " 
                << svmGoldStandard->GetNumberOfSupportVectors() 
                << std::endl;
      return EXIT_FAILURE;
    }
    std::cerr << "DONE" << std::endl;
    
    //
    // compare B
    //
    std::cerr << "Testing B...";
    if (!FloatingPointEqual(svm->GetB(), svmGoldStandard->GetB(), 
                            ASCII_EPSILON))
    {
      return EXIT_FAILURE;
    }
    std::cerr << "DONE" << std::endl;

    //
    // compare alphas
    //
    std::cerr << "Testing alphas...";
    unsigned int numSV = svm->GetNumberOfSupportVectors();
    for (unsigned int i = 0; i < numSV; ++i)
    {
      if (!FloatingPointEqual(svm->GetAlpha(i), 
                              svmGoldStandard->GetAlpha(i), 
                              BINARY_EPSILON))
      {
        return EXIT_FAILURE;
      }
    }
    std::cerr << "DONE" << std::endl;
    
    //
    // compare support vectors
    //
    std::cerr << "Testing SVs...";
    for (unsigned int i = 0; i < numSV; ++i)
    {
      if (!FloatingPointArrayEqual(svm->GetSupportVector(i), 
                                   svmGoldStandard->GetSupportVector(i), 
                                   svm->GetMeasurementVectorSize(),
                                   BINARY_EPSILON))
      {
        return EXIT_FAILURE;
      }
    }
    std::cerr << "DONE" << std::endl;

    //
    // load gold standard classifications
    //
    std::cerr << "Loading gold standard classifications...";
    std::vector<SVMTestingUtils::ClassLabelType> goldStandardClasses;
    std::ifstream input(goldStandardClassificationFilenameIter->c_str());
    SVMTestingUtils::ClassLabelType inputLabel;
    while (input >> inputLabel && !input.eof())
    {
      goldStandardClasses.push_back(inputLabel);
    }
    input.close();
    std::cerr << "DONE" << std::endl;

    //
    // compare classifications
    //
    std::cerr << "Comparing classifications...";
    unsigned int goldStandardClassIndex = 0;
    for (SVMTestingUtils::MembershipSampleType::ConstIterator sampleIter =
           testingMembershipSample->Begin();
         sampleIter != testingMembershipSample->End();
         ++sampleIter)
    {
      SVMTestingUtils::ClassLabelType currentLabel =
        (svm->Classify(sampleIter.GetMeasurementVector()) < 0.0
         ? SVMTestingUtils::Class2Label
         : SVMTestingUtils::Class1Label);
      if (currentLabel != goldStandardClasses[goldStandardClassIndex++])
      {
        return EXIT_FAILURE;
      }
    }
    std::cerr << "DONE" << std::endl;

    //
    // compare discriminative directions
    //
    std::cerr << "Comparing discriminative directions...";
    typedef itk::Statistics::
      SVMDiscriminativeDirectionCalculator<SVMTestingUtils::SVMType>
      CalculatorType;
    CalculatorType::Pointer calculator = CalculatorType::New();
    calculator->SetSupportVectorMachine(svmGoldStandard.GetPointer());
    SVMTestingUtils::MeasurementVectorType
      discriminativeDirection(svm->GetMeasurementVectorSize());

    for (unsigned int i = 0; i < goldStandardDDFilenameIter->size(); ++i)
    {
      // load gold standard measurement vector
      SVMTestingUtils::MeasurementVectorType goldStandardVector;
      SVMTestingUtils::
        LoadMeasurementVectorFromVectorFile((*goldStandardDDFilenameIter)[i], 
                                            goldStandardVector);

      // compute discriminative direction
      double error;
      calculator->CalculateDiscriminativeDirection(i,
                                                   discriminativeDirection,
                                                   error);      
      
      // compare vectors
      if (!FloatingPointArrayEqual(discriminativeDirection, 
                                   goldStandardVector, 
                                   svm->GetMeasurementVectorSize(),
                                   BINARY_EPSILON))
      {
        return EXIT_FAILURE;
      }      
    }
    std::cerr << "DONE" << std::endl;

    //
    // setup the jackknife classification error estimator
    //
    typedef itk::Statistics::
      SVMJackknifeClassificationErrorEstimator<
      SVMTestingUtils::MembershipSampleType>
      JackknifeEstimatorType;
    JackknifeEstimatorType::Pointer jackknifeEstimator =
      JackknifeEstimatorType::New();
    
    // since we will use the same data over and over again, it will be
    // faster to generate the kernel matrix ourselves and just pass in
    // the data rather than have the solver generate the kernel matrices
    jackknifeEstimator->SetPrecomputeKernelMatrix(true);
    jackknifeEstimator->SetJackknifeIterations(100);
    jackknifeEstimator->SetMembershipSample(membershipSample);
    jackknifeEstimator->SetSupportVectorMachine(svmGoldStandard);  
    jackknifeEstimator->SetSVMSolver(solver);
    jackknifeEstimator->SetRandomSeed(0);

    //
    // set to leave-half-out
    //
    unsigned int N = numTrainingVectorsPerClass/2;
    jackknifeEstimator->
      SetTrainingSetSize(0, numTrainingVectorsPerClass-N);
    jackknifeEstimator->SetTestSetSize(0, N);
    jackknifeEstimator->
      SetTrainingSetSize(1, numTrainingVectorsPerClass-N);
    jackknifeEstimator->SetTestSetSize(1, N);

    //
    // run jackknife estimator
    //
    std::cerr << "Running jackknife estimation...";
    jackknifeEstimator->Update();
    std::cerr << "DONE" << std::endl;

    //
    // load gold standard histograms
    //
    std::cerr << "Loading gold standard histogram...";
    std::vector<unsigned long> goldStandardHistogram;
    std::ifstream inputHistogram(goldStandardHistogramFilenameIter->c_str());
    unsigned long frequency;
    while (inputHistogram >> frequency && !inputHistogram.eof())
    {
      goldStandardHistogram.push_back(frequency);
    }
    inputHistogram.close();
    std::cerr << "DONE" << std::endl;

    //
    // compare misclassification histogram
    //
    std::cerr << "Comparing histograms...";
    if (!FloatingPointArrayEqual(goldStandardHistogram,
                                 jackknifeEstimator->
                                 GetMisclassificationHistogram(),
                                 N+1, BINARY_EPSILON))
    {
      return EXIT_FAILURE;
    }      
    std::cerr << "DONE" << std::endl;

    ++svmFilenameIter;
    ++kernelTypeIter;
    ++kernelParamsIter;
    ++goldStandardClassificationFilenameIter;
    ++goldStandardDDFilenameIter;
    ++goldStandardHistogramFilenameIter;
  }

  return EXIT_SUCCESS;
}
