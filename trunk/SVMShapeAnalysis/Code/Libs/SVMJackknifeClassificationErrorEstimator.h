#ifndef SVMJackknifeClassificationErrorEstimator_H
#define SVMJackknifeClassificationErrorEstimator_H

#include "JackknifeClassificationErrorEstimatorBase.h"
#include "itkSupportVectorMachine.h"
#include "itkSVMSolverBase.h"

namespace itk{
namespace Statistics{

/** \class JackknifeClassificationErrorEstimatorBase
 *
 * Concrete implementation of
 * JackknifeClassificationErrorEstimatorBase that uses a support
 * vector machine as a classifier.
 *
 * Users of this class should provide labeled training vectors (as a
 * MembershipSample), a support vector machine (SupportVectorMachine),
 * a support vector machine solver (SVMSolverBase), training and test
 * set sizes (for example leave-1-out), and a number of jackknife
 * iterations.
 *
 * At each jackknife iteration, the training data is randomly shuffled
 * and divided into training and test sets.  The classifier is trained
 * on the training sets and tested using the testing set.
 * Classification results are recorded in a confusion matrix and
 * misclassification histogram.
 *
 * See JackknifeClassificationErrorEstimatorBase for more details.
 *
 * By default, this class precomputes kernel inner products (for all
 * pairs of training vectors) before starting jackknife iterations.
 * This means that the kernel inner products don't have to be computed
 * over and over again at each iteration.  As a result, computation is
 * much faster.  This behavior can be turned off.
 *
 */
    
template<class TMembershipSample>
class ITK_EXPORT SVMJackknifeClassificationErrorEstimator 
  : public JackknifeClassificationErrorEstimatorBase<TMembershipSample> {
 public:
  /** Standard itk class typedefs */
  typedef SVMJackknifeClassificationErrorEstimator    Self;
  typedef JackknifeClassificationErrorEstimatorBase<TMembershipSample> 
  Superclass;
  typedef SmartPointer<Self>                          Pointer;
  typedef SmartPointer<const Self>                    ConstPointer;

  /** sample alias */
  typedef typename Superclass::MembershipSampleType   MembershipSampleType;

  /** the type of measurement vector stored in the sample */
  typedef typename Superclass::MeasurementVectorType  MeasurementVectorType;

  /** the type of sample used to train the classifier */
  typedef typename Superclass::TrainingSampleType     TrainingSampleType;

  /** the type of sample used to test the classifier */
  typedef typename Superclass::TestSampleType         TestSampleType;
  
  /** the type of support vector machine classifier */
  typedef itk::Statistics::
  SupportVectorMachine<MeasurementVectorType>         SVMType;

  /** the type of support vector machine solver (optimizer) */
  typedef itk::Statistics::
  SVMSolverBase<TrainingSampleType, TrainingSampleType> SVMSolverType;

  /** standard itk new and type macros */
  itkNewMacro(Self);
  itkTypeMacro(SVMJackknifeClassificationErrorEstimator, Object);

  /** set the support vector machine that will be trained */
  virtual void SetSupportVectorMachine(SVMType* _arg);
  /** set the support vector machine */
  itkGetConstObjectMacro(SupportVectorMachine, SVMType);

  /** access the membership sample that the random subsamples are
   *  generated from
   */
  virtual void SetMembershipSample(const MembershipSampleType* _arg);

  /** access the svm solver that will be used for training */
  itkSetObjectMacro(SVMSolver, SVMSolverType);
  /** access the svm solver that will be used for training */
  itkGetConstObjectMacro(SVMSolver, SVMSolverType);

  /** set precompute inner products. If true, a matrix of kernel
   *  products will be computed before the jackkknife iterations.  If
   *  false, the kernel products will be computed at every iteration.
   */ 
  itkSetMacro(PrecomputeKernelMatrix, bool);
  /** get precompute inner products. If true, a matrix of kernel
   *  products will be computed before the jackkknife iterations.  If
   *  false, the kernel products will be computed at every iteration.
   */ 
  itkGetMacro(PrecomputeKernelMatrix, bool);
  /** set precompute inner products. If true, a matrix of kernel
   *  products will be computed before the jackkknife iterations.  If
   *  false, the kernel products will be computed at every iteration.
   */ 
  itkBooleanMacro(PrecomputeKernelMatrix);

  /** start jackknife iterations */
  virtual void Update();

 protected:
  /** iterator for training sets */
  typedef typename Superclass::TrainingSampleIter TrainingSampleIter;

  /** iterator for training sets */
  typedef typename Superclass::TestSampleIter     TestSampleIter;

  /** const iterator for training sets */
  typedef typename Superclass::TrainingSampleConstIter TrainingSampleConstIter;

  /** const iterator for training sets */
  typedef typename Superclass::TestSampleConstIter TestSampleConstIter;

  SVMJackknifeClassificationErrorEstimator();
  ~SVMJackknifeClassificationErrorEstimator() {};

  /** compute a matrix of kernel inner products for all training data */
  virtual void PrecomputeKernelMatrix();

  /** copy results from the full kernel inner product matrix for those
   *  vectors that are currently being used to train a support vector
   *  machine
   */
  virtual void UpdateSolversKernelMatrix();

  /** Train the SVM on the current training set. 
   */
  virtual void Train();

  /** Called by the Test method to classifiy a particular element of
   *  the test set.  This method returns the index of the label of
   *  vector v---the index is the same as the index of the label in
   *  the MembershipSample.
   */
  virtual unsigned int Classify(unsigned int instanceIdentifier);  

 private:
  // purposely not implemented
  SVMJackknifeClassificationErrorEstimator(const Self&); 
  // purposely not implemented
  void operator=(const Self&);                    

  /** the support vector machine used for training and classification */
  typename SVMType::Pointer              m_SupportVectorMachine;
  
  /** the SVM solver used to train the SVM from training data */
  typename SVMSolverType::Pointer        m_SVMSolver;

  /** true if this class should precompute the matrix of kernel inner
   *  products 
   */
  bool                                   m_PrecomputeKernelMatrix;

  /** true if this the matrix of kernel inner products must be updated
   */
  bool                                   m_UpdateKernelMatrix;

  /** the matrix of kernel inner products for all data */
  typename SVMSolverType::VnlMatrixType  m_FullKernelMatrix;

  /** the matrix of kernel inner products for only the data currently
   *  used for training
   */
  typename SVMSolverType::VnlMatrixType  m_CurrentKernelMatrix;
};

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "SVMJackknifeClassificationErrorEstimator.txx"
#endif

#endif // SVMJackknifeClassificationErrorEstimator
