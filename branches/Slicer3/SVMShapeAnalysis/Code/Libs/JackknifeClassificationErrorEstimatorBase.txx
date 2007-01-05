#ifndef JackknifeClassificationErrorEstimatorBase_txx
#define JackknifeClassificationErrorEstimatorBase_txx

#include <assert.h>
#include <algorithm>
#include <numeric>
#include "vcl_algorithm.h"

namespace itk{
namespace Statistics{

template<class TMembershipSample>
JackknifeClassificationErrorEstimatorBase<TMembershipSample>::
JackknifeClassificationErrorEstimatorBase()
  : m_MembershipSample(NULL), m_JackknifeIterations(0)
{
  this->m_OutputLevel = Silent;
  this->m_RandomNumberGenerator.Initialize();
}

template<class TMembershipSample>
void
JackknifeClassificationErrorEstimatorBase<TMembershipSample>::
SetMembershipSample(const MembershipSampleType* ms)
{
  if (this->m_MembershipSample != ms)
  {
    this->m_MembershipSample = ms;
    unsigned int numClasses = m_MembershipSample->GetNumberOfClasses(); 

    // resize set size vectors
    this->m_TrainingSetSize.resize(numClasses, 0);
    this->m_TestSetSize.resize(numClasses, 0);

    this->Modified();
  }
}

template<class TMembershipSample>
void
JackknifeClassificationErrorEstimatorBase<TMembershipSample>::
PermuteData() 
{
  assert(this->m_MembershipSample.IsNotNull());
#ifndef NDEBUG
  unsigned int numClasses = m_MembershipSample->GetNumberOfClasses(); 
#endif
  assert(this->m_TrainingSetSize.size() == numClasses);
  assert(this->m_TestSetSize.size()     == numClasses);
  assert(this->m_TrainingSets.size()    == numClasses);
  assert(this->m_TestSets.size()        == numClasses);
  
  TrainingSampleIter trainingSampleIter = this->m_TrainingSets.begin();
  TestSampleIter     testSampleIter     = this->m_TestSets.begin();
  typename std::vector<SampleSizeType>::const_iterator trainingSetSizeIter = 
    this->m_TrainingSetSize.begin();
  typename std::vector<SampleSizeType>::const_iterator testSetSizeIter = 
    this->m_TestSetSize.begin();

  unsigned int classIndex = 0;  
  while (trainingSampleIter != this->m_TrainingSets.end())
  {
    //
    // get a subsample containing the measurement vectors in this
    // class
    // 
    typename MembershipSampleType::ClassSampleType::ConstPointer 
      classSubsample =
      this->m_MembershipSample->GetClassSample(classIndex);
    unsigned int classSize = classSubsample->Size();

    //
    // make sure there are enough elements to fill the training and
    // test sets
    //
    if (*trainingSetSizeIter + *testSetSizeIter > classSize)
    {
      itkExceptionMacro(<< "The training and test sets are larger than the "
                        << "class size, training set size: " 
                        << *trainingSetSizeIter
                        << ", test set size: "
                        << *testSetSizeIter
                        << ", class size: " 
                        << classSize);
    }

    //
    // clear the training and test samples for this class
    //
    (*trainingSampleIter)->Clear();
    (*testSampleIter)->Clear();

    //
    // get the instance identifiers for the elements of this class
    //
    typedef typename TrainingSampleType::InstanceIdentifier IDType;
    typedef std::vector<IDType>                             IDVector;
    typedef typename IDVector::iterator                     IDIter;
    IDVector classInstanceIdentifiers(classSize, 0);
    IDIter instanceIter = classInstanceIdentifiers.begin();
    for (typename MembershipSampleType::ClassSampleType::ConstIterator 
           vecIter = classSubsample->Begin();
         vecIter != classSubsample->End();
         ++vecIter)
    {
      *instanceIter++ = vecIter.GetInstanceIdentifier();
    }

    //
    // shuffle the instance identifiers: implementation copied from
    // gcc 4.0.4 stl_algo.h for portability of tests
    //
    IDIter first = classInstanceIdentifiers.begin();
    IDIter last  = classInstanceIdentifiers.end();
    if (first != last)
    {
      for(IDIter i = first + 1; i != last; ++i)
      {
        std::iter_swap(i, first + m_RandomNumberGenerator((i - first) + 1));
      }
    }
    //std::random_shuffle(classInstanceIdentifiers.begin(),
    //  classInstanceIdentifiers.end(), 
    //  m_RandomNumberGenerator);
    
    //
    // testing, make sure all instance IDs are accounted for
    //
#ifndef NDEBUG
    for (typename MembershipSampleType::ClassSampleType::ConstIterator 
           vecIter = classSubsample->Begin();
         vecIter != classSubsample->End();
         ++vecIter)
    {
      if (std::find(classInstanceIdentifiers.begin(),
                    classInstanceIdentifiers.end(),
                    vecIter.GetInstanceIdentifier())
          == classInstanceIdentifiers.end())
      {
        std::cerr << "ERROR, INSTANCEID NOT FOUND!!!" << std::endl;
        exit(0);
      }
    }
#endif

    //
    // add instances to training set
    //
    instanceIter = classInstanceIdentifiers.begin();
    if (this->m_OutputLevel >= Verbose)
    {
      std::cerr << "Training instance ids: ";
    }
    for (SampleSizeType elementIndex = 0; 
         elementIndex < *trainingSetSizeIter; ++elementIndex)
    {
      if (this->m_OutputLevel >= Verbose)
      {
        std::cerr << *instanceIter << " ";
      }
      (*trainingSampleIter)->AddInstance(*instanceIter++);
    }
    if (this->m_OutputLevel >= Verbose)
    {
      std::cerr << std::endl;
    }

    //
    // add instances to test set
    //
    if (this->m_OutputLevel >= Verbose)
    {
      std::cerr << "Test instance ids: ";
    }
    for (SampleSizeType elementIndex = 0; 
         elementIndex < *testSetSizeIter; ++elementIndex)
    {
      if (this->m_OutputLevel >= Verbose)
      {
        std::cerr << *instanceIter << " ";
      }
      (*testSampleIter)->AddInstance(*instanceIter++);
    }
    if (this->m_OutputLevel >= Verbose)
    {
      std::cerr << std::endl;
    }

    ++classIndex;
    ++trainingSampleIter;
    ++testSampleIter;
    ++trainingSetSizeIter;
    ++testSetSizeIter;
  }
}

template<class TMembershipSample>
void
JackknifeClassificationErrorEstimatorBase<TMembershipSample>::
Test()
{
  /*
    std::cerr << "Testing Samples: " << std::endl;
    for (int i = 0; i < this->m_TrainingSetSize[0]; ++i) {
    std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
    std::cerr << "## 1 ##" << 
    this->m_TestSets[0]->GetMeasurementVectorByIndex(i)
    << std::endl << "## 2 ##" <<
    this->m_TestSets[1]->GetMeasurementVectorByIndex(i)
    << std::endl;
    }
  */

  unsigned int groundTruthLabel = 0;
  unsigned long numMissclassifications = 0;
  for (TestSampleIter sampleIter = this->m_TestSets.begin();
       sampleIter != m_TestSets.end(); 
       ++sampleIter, ++groundTruthLabel)
  {
    for (typename TestSampleType::ConstIterator vectorIter = 
           (*sampleIter)->Begin();
         vectorIter != (*sampleIter)->End(); ++vectorIter)
    {
      // classify this vector
      unsigned int classifiersLabel =
        Classify(vectorIter.GetInstanceIdentifier());

      // update confusion matrix
      ++this->m_ConfusionMatrix(groundTruthLabel, classifiersLabel);

      // update misclassifications
      if (groundTruthLabel != classifiersLabel) 
      {
        ++numMissclassifications;
      }
    }
  }
  
  // update misclassificaiton histogram
  ++this->m_MisclassificationHistogram[numMissclassifications];
}

template<class TMembershipSample>
void
JackknifeClassificationErrorEstimatorBase<TMembershipSample>::
Update() 
{
  if (this->m_MembershipSample.IsNull()) {
    itkExceptionMacro(<< "Membership sample must not be null");
  }
  unsigned int numClasses = m_MembershipSample->GetNumberOfClasses(); 

  //
  // initialize training sets
  //
  this->m_TrainingSets.resize(numClasses);
  for (TrainingSampleIter sampleIter = this->m_TrainingSets.begin();
       sampleIter != this->m_TrainingSets.end(); 
       ++sampleIter)
  {
    if ((*sampleIter).IsNull())
    {
      (*sampleIter) = TrainingSampleType::New();
    }
    else
    {
      (*sampleIter)->Clear();
    }
    (*sampleIter)->SetSample(this->m_MembershipSample->GetSample());      
  }

  //
  // initialize test sets
  //
  this->m_TestSets.resize(numClasses);
  for (TestSampleIter sampleIter = this->m_TestSets.begin();
       sampleIter != this->m_TestSets.end(); 
       ++sampleIter)
  {
    if ((*sampleIter).IsNull())
    {
      (*sampleIter) = TestSampleType::New();
    }
    else
    {
      (*sampleIter)->Clear();
    }
    (*sampleIter)->SetSample(this->m_MembershipSample->GetSample());      
  }

  //
  // initialize the confusion matrix
  //
  this->m_ConfusionMatrix.SetSize(numClasses, numClasses);
  this->m_ConfusionMatrix.Fill(0);

  //
  // initialize the misclassificaiton histogram
  //
  unsigned long maxPossibleMisclassifications = 
    std::accumulate(this->m_TestSetSize.begin(), this->m_TestSetSize.end(), 0);
  // need to add 1 since no (0) misclassificaitons may occur
  this->m_MisclassificationHistogram.SetSize(maxPossibleMisclassifications+1);
  this->m_MisclassificationHistogram.Fill(0);

  //
  // jackknife iterations
  //
  for (unsigned int jackknifeIter = 0; 
       jackknifeIter < this->m_JackknifeIterations; ++ jackknifeIter)
  {
    // shuffle the data---update training and test sets
    this->PermuteData();
    
    // train the classifier
    this->Train();

    //
    // classify testing set
    //
    // the Test function should update the confusion matrix and
    // misclassification histogram
    //
    this->Test();
  }
}

} // end namespace Statistics
} // end namespace itk
#endif // JackknifeClassificationErrorEstimatorBase_txx
