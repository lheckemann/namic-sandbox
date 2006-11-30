#ifndef JackknifeClassificationErrorEstimatorBase_H
#define JackknifeClassificationErrorEstimatorBase_H

#include <vector>
#include <itkSample.h>
#include <itkObject.h>
#include <itkVariableSizeMatrix.h>
#include <itkVariableLengthVector.h>
#include "itkMersenneTwisterRandomVariateGenerator.h"

namespace itk{
namespace Statistics{

/** \class JackknifeClassificationErrorEstimatorBase
 *
 * JackknifeClassificationErrorEstimatorBase is an abstract class that
 * applies jackknife estimation of classification error to a
 * classifier.  The details of the classifier must be specified by a
 * subclass.
 *
 * Developers implementing a subclass must implement the Train and
 * Classify methods.  The Train method should train a classifier given
 * a random subsample of the training data.  The Classify method uses
 * the trained classifier to classify a vector.  For an example, see
 * the itk::Statistics::SVMJackknifeClassificationErrorEstimator
 * class.
 *
 * Users of a subclass of this class should provide labeled training
 * vectors (as a MembershipSample), training and test set sizes (for
 * example leave-1-out), and a number of jackknife iterations.  
 *
 * At each jackknife iteration, the training data is randomly shuffled
 * and divided into training and test sets.  The classifier is trained
 * on the training sets and tested using the testing set.
 * Classification results are recorded in a confusion matrix and
 * misclassification histogram.
 *
 * The user can examine the confusion matrix to determine the number
 * of vectors from each class that were classified with each label.
 * m_ConfusiontMatrix(i,j) is equal to the number of class i vectors
 * that were classified as class j.  Perfect classification results in
 * a diagonal confusion matrix.
 *
 * For the misclassification histogram, the x-axis represents the
 * total number of misclassifications for a single jackknife iteration
 * (starting with 0).  The y-axis represents the number of iterations
 * that produced that number of misclassifications.  That is,
 * m_MisclassificationHistogram[i] is equal to the number of jackknife
 * iterations that produced i misclassifications.  The sum of the
 * entries in the histogram is equal to the number of jackknife
 * iterations.
 *
 */

template<class TMembershipSample>
class ITK_EXPORT JackknifeClassificationErrorEstimatorBase 
  : public itk::Object
{
public:
  /** Standard itk class typedefs */
  typedef JackknifeClassificationErrorEstimatorBase      Self;
  typedef Object                                         Superclass;
  typedef SmartPointer<Self>                             Pointer;
  typedef SmartPointer<const Self>                       ConstPointer;

  /** sample typedef alias */
  typedef TMembershipSample                              MembershipSampleType;

  /** expresses the size of the training and test samples */
  typedef typename MembershipSampleType::InstanceIdentifier SampleSizeType;

  /** the type of measurement vector stored in the sample */
  typedef typename MembershipSampleType::MeasurementVectorType
  MeasurementVectorType;

  /** the type of sample used to train the classifier */
  typedef typename MembershipSampleType::ClassSampleType TrainingSampleType;

  /** the type of sample used to test the classifier */
  typedef typename MembershipSampleType::ClassSampleType TestSampleType;

  /** indicates number of jackknife iterations */
  typedef unsigned long                                  IterationCountType;

  /** holds classification confusion matrix */
  typedef itk::VariableSizeMatrix<IterationCountType>    ConfusionMatrixType;

  /** holds classification error histogram */
  typedef itk::VariableLengthVector<IterationCountType>  HistogramType;

  /** standard ITK macro */
  itkTypeMacro(JackknifeClassificationErrorEstimatorBase, Object);

  /** access the membership sample that the random subsamples are
   *  generated from
   */
  virtual void SetMembershipSample(const MembershipSampleType* ms);
  itkGetConstObjectMacro(MembershipSample, MembershipSampleType);

  /** Set the size of the randomly generated training sets */
  virtual void SetTrainingSetSize(unsigned int classIndex,
                                  SampleSizeType size)
  {
    this->m_TrainingSetSize[classIndex] = size;
  }

  /** Get the size of the randomly generated training sets */
  virtual SampleSizeType GetTrainingSetSize(unsigned int classIndex) const
  {
    return this->m_TrainingSetSize[classIndex];
  }

  /** Set the size of the randomly generated test sets */
  virtual void SetTestSetSize(unsigned int classIndex,
                              SampleSizeType size)
  {
    this->m_TestSetSize[classIndex] = size;
  }

  /** Get the size of the randomly generated test sets */
  virtual SampleSizeType GetTestSetSize(unsigned int classIndex) const
  {
    return this->m_TestSetSize[classIndex];
  }

  /** Set the number of jackknife iterations */
  itkSetMacro(JackknifeIterations, IterationCountType);

  /** Get the number of jackknife iterations */
  itkGetMacro(JackknifeIterations, IterationCountType);

  /** Get reference to the confusion matrix that was generated during
   *  the jackknife estimation process.  m_ConfusiontMatrix(i,j)
   *  contains the number of times a vector of class i was classified
   *  as class j.  A diagonal confusion matrix will result from a
   *  perfect classifier.
   */
  itkGetConstReferenceMacro(ConfusionMatrix, ConfusionMatrixType);

  /** Get reference to the histogram of classification errors that was
   *  generated during the jackknife estimation process.
   *  m_MisclassificationHistogram[i] contains the number of
   *  interations in which i misclassifications occured.
   */
  itkGetConstReferenceMacro(MisclassificationHistogram, HistogramType);

  /** Set the seed for the random number generator.  If no seed is
   *  set, the time is used as a seed.  This method should only be
   *  used for testing purposes where data should be shuffled in the
   *  same way every time.
   */
  void SetRandomSeed(int seed = 0)
  {
    this->m_RandomNumberGenerator.Initialize(seed);
  }

  /** Run the jackknife estimation process.  The following steps are
   *  called for the number of jackknife iterations: generate random
   *  training and test sets, train the classifier using the training
   *  set, test the classifier on the test set.  During the testing
   *  phase, the misclassification histogram and confusion matrix are
   *  updated.  A typical subclass will be based on a particular
   *  classifier and should implement the Train and Classify methods.
   */
  virtual void Update();

 protected:

  /** container for training samples */
  typedef std::vector<typename TrainingSampleType::Pointer>
  TrainingSampleVecType;

  /** container for test samples */
  typedef std::vector<typename TestSampleType::Pointer>    TestSampleVecType;

  /** iterator for training sets */
  typedef typename TrainingSampleVecType::iterator         TrainingSampleIter;

  /** iterator for training sets */
  typedef typename TrainingSampleVecType::iterator         TestSampleIter;

  /** const iterator for training sets */
  typedef typename TrainingSampleVecType::const_iterator
  TrainingSampleConstIter;

  /** const iterator for training sets */
  typedef typename TrainingSampleVecType::const_iterator         
  TestSampleConstIter;

  JackknifeClassificationErrorEstimatorBase();
  ~JackknifeClassificationErrorEstimatorBase() {};

  /** permute the data---this is called at the start of each jackknife
   *  iteration.  The training and test samples should be filled for
   *  each class.
   */
  virtual void PermuteData();

  /** Train the classifier on the current training set. Implementation
   *  is dependent on classifier.
   */
  virtual void Train() = 0;

  /** Test the trained classifier on the current test set; the number
   *  of misclassifications during testing is recorded in the
   *  misclassification histogram.  This method also updates the
   *  confusion matrix.
   */
  virtual void Test();

  /** Called by the Test method to classifiy a particular element of
   *  the test set.  Implementation is dependent on the classifier.
   *  This method should return the index of the label of vector
   *  v---the index is the same as the index of the label in the
   *  MembershipSample.
   */
  virtual unsigned int Classify(unsigned int instanceIdentifier) = 0;

  /** the training samples, one for each class */
  TrainingSampleVecType                       m_TrainingSets;

  /** the test samples, one for each class */
  TestSampleVecType                           m_TestSets;

  /** size of the training set that will be generated at each iteration */
  std::vector<SampleSizeType>                 m_TrainingSetSize;

  /** size of the test set that will be generated at each iteration */
  std::vector<SampleSizeType>                 m_TestSetSize;

  /** sample from which the training and test sets are generated */
  typename MembershipSampleType::ConstPointer m_MembershipSample;

  /** number of shuffle-train-test iterations  */
  IterationCountType                          m_JackknifeIterations;

  /** The confusion matrix contains the number of times a vector with
   *  ground truth label rowIndex was identified by the trained
   *  classifier as having the label columnIndex.  Thus, the confusion
   *  matrix is diagonal if the classifier is perfect.  The trace
   *  indicates the number of correct classifications; the sum of the
   *  off-diagonal entries indicates the number of incorrect
   *  classifications.
   */
  ConfusionMatrixType                         m_ConfusionMatrix;

  /** The misclassification histogram holds the number of occurences,
   *  out of the total number of iterations, of X misclassifications.
   *  X ranges from 0 to the size of the test set.  The sum of the
   *  entries of the histogram is equal to the number of iterations.
   */
  HistogramType                               m_MisclassificationHistogram;

  /** local debug output information */
  enum OutputLevel {Silent, Standard, Verbose, Everything};
  OutputLevel                                 m_OutputLevel;

 private:

  /**
   * An itk random number generator is used in order to assure
   * consistent testing results across platforms.
   */ 
  class LocalRandomNumberGenerator
  {
  public:
    LocalRandomNumberGenerator()
    {
      this->m_randomGenerator = 
        itk::Statistics::MersenneTwisterRandomVariateGenerator::New();
    }
    
    void Initialize()
    {
      this->m_randomGenerator->Initialize();
    }
    
    void Initialize(int seed)
    {
      this->m_randomGenerator->Initialize(seed);
    }

    /** this function allows the random number generator to be used as
     * a functor (eg, with random_shuffle)
     */
    unsigned int operator() (unsigned int max)
    {
      return this->m_randomGenerator->GetIntegerVariate(max-1);
    }
  private:
    itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer
    m_randomGenerator;
  };

  // purposely not implemented
  JackknifeClassificationErrorEstimatorBase(const Self&); 
  // purposely not implemented
  void operator=(const Self&);                    

  LocalRandomNumberGenerator m_RandomNumberGenerator;
};

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "JackknifeClassificationErrorEstimatorBase.txx"
#endif

#endif // JackknifeClassificationErrorEstimatorBase
