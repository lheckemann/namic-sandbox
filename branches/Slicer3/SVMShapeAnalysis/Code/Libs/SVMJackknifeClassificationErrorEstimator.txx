#ifndef SVMJackknifeClassificationErrorEstimator_txx
#define SVMJackknifeClassificationErrorEstimator_txx

namespace itk{
namespace Statistics{

template<class TMembershipSample>
SVMJackknifeClassificationErrorEstimator<TMembershipSample>::
SVMJackknifeClassificationErrorEstimator()
{
  m_PrecomputeKernelMatrix       = false;
  m_SupportVectorMachine         = NULL;
  m_SVMSolver                    = NULL;
  m_UpdateKernelMatrix           = true;
  this->Modified();
}

template<class TMembershipSample>
void
SVMJackknifeClassificationErrorEstimator<TMembershipSample>::
SetSupportVectorMachine(SVMType* _arg)
{
  itkDebugMacro("setting SupportVectorMachine to " << _arg );
  if (this->m_SupportVectorMachine != _arg)
  {
    this->m_SupportVectorMachine = _arg;
    this->m_UpdateKernelMatrix  = true;
    this->Modified();
  }
} 

template<class TMembershipSample>
void
SVMJackknifeClassificationErrorEstimator<TMembershipSample>::
SetMembershipSample(const MembershipSampleType* _arg)
{
  if (this->m_MembershipSample != _arg)
  {
    Superclass::SetMembershipSample(_arg);
    this->m_UpdateKernelMatrix  = true;
  }
} 

template<class TMembershipSample>
void SVMJackknifeClassificationErrorEstimator<TMembershipSample>
::PrecomputeKernelMatrix()
{
  if (!this->m_UpdateKernelMatrix)
  {
    return;
  }

  unsigned int numVectors = this->m_MembershipSample->Size();
  this->m_FullKernelMatrix.set_size(numVectors, numVectors);

  // get a pointer to the kernel
  typename SVMType::KernelType::Pointer kernel = 
    this->m_SupportVectorMachine->GetKernel();

  unsigned int i = 0;
  unsigned int j = 0;
  for (typename MembershipSampleType::ConstIterator iIter = 
         this->m_MembershipSample->Begin();
       iIter != this->m_MembershipSample->End(); ++iIter, ++i)
  {
    j = i;
    for (typename MembershipSampleType::ConstIterator jIter = iIter;
         jIter != this->m_MembershipSample->End(); ++jIter, ++j)  
    {
      //double sign = iIter.GetClassLabel() == jIter.GetClassLabel() ? 1 : -1;
      this->m_FullKernelMatrix(i,j) = 
        kernel->Evaluate(iIter.GetMeasurementVector(), 
                         jIter.GetMeasurementVector());
      // the matrix is symmetric
      this->m_FullKernelMatrix(j,i) = this->m_FullKernelMatrix(i,j);
    }
  }

  if (this->m_OutputLevel >= Superclass::Everything)
  {
    std::cerr << "Kernel Product Matrix: " << std::endl;
    std::cerr << this->m_FullKernelMatrix << std::endl;
  }

  this->m_UpdateKernelMatrix = false;
}

template<class TMembershipSample>
void 
SVMJackknifeClassificationErrorEstimator<TMembershipSample>
::UpdateSolversKernelMatrix() 
{
  // create a matrix the proper size for this particular training set
  unsigned int trainingClass1Size = this->m_TrainingSets[0]->Size();
  unsigned int trainingClass2Size = this->m_TrainingSets[1]->Size();
  unsigned int totalTrainingSize  = trainingClass1Size + trainingClass2Size;

  if (totalTrainingSize != m_CurrentKernelMatrix.rows())
  {
    this->m_CurrentKernelMatrix.set_size(totalTrainingSize, totalTrainingSize);
  }

  // create a lookup table of instance ids
  typedef typename MembershipSampleType::InstanceIdentifier IDType;
  std::vector<IDType> idLookup;
  for (TrainingSampleConstIter sampleIter = this->m_TrainingSets.begin();
       sampleIter != this->m_TrainingSets.end(); ++sampleIter)
  {
    for (typename TrainingSampleType::ConstIterator 
           vecIter = (*sampleIter)->Begin();
         vecIter != (*sampleIter)->End();
         ++vecIter)
    {
      idLookup.push_back(vecIter.GetInstanceIdentifier());
    }
  }
  
  // copy data from full KernelMatrix to mini KernelMatrix
  for (unsigned int i = 0; i < totalTrainingSize; ++i)
  {
    for (unsigned int j = i; j < totalTrainingSize; ++j)
    {
      m_CurrentKernelMatrix(i, j) = 
        this->m_FullKernelMatrix(idLookup[i], idLookup[j]);
      m_CurrentKernelMatrix(j, i) = m_CurrentKernelMatrix(i, j);
    }
  }
  if (this->m_OutputLevel >= Superclass::Everything)
  {
    std::cerr << "Mini KernelProduct Matrix: " << std::endl 
              << m_CurrentKernelMatrix << std::endl;
  }
  this->m_SVMSolver->SetKMatrix(m_CurrentKernelMatrix);
}

template<class TMembershipSample>
unsigned int
SVMJackknifeClassificationErrorEstimator<TMembershipSample>::
Classify(unsigned int testIndex) 
{
  double classifierValue = 0.0;
  if (this->m_PrecomputeKernelMatrix)
  {
    //
    // if the kernel products have already been computed, reuse this
    // data
    //
    unsigned int numSupportVectors = 
      this->m_SupportVectorMachine->GetNumberOfSupportVectors();
    classifierValue = -this->m_SupportVectorMachine->GetB();
    for (unsigned int i = 0; i < numSupportVectors; ++i)
    {
      unsigned int svIndex = 
        this->m_SVMSolver->GetSupportVectorInstanceIdentifier(i);
      classifierValue += 
        this->m_SupportVectorMachine->GetAlpha(i) *
        this->m_SupportVectorMachine->GetLabel(i) *
        this->m_FullKernelMatrix(svIndex, testIndex);
    }
  }
  else
  {
    classifierValue = this->m_SupportVectorMachine->
      Classify(this->m_MembershipSample->GetMeasurementVector(testIndex));
  }

  return (classifierValue < 0.0 ? 1 : 0);
}

template<class TMembershipSample>
void
SVMJackknifeClassificationErrorEstimator<TMembershipSample>::
Train() 
{
  /*
    std::cerr << "Training Samples: " << std::endl;
    for (int i = 0; i < this->m_TrainingSetSize[0]; ++i) {
    std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
    std::cerr << "## 1 ##" << 
    this->m_TrainingSets[0]->GetMeasurementVectorByIndex(i)
    << std::endl << "## 2 ##" <<
    this->m_TrainingSets[1]->GetMeasurementVectorByIndex(i)
    << std::endl;
    }
  */

  this->m_SVMSolver->Initialize();
  this->m_SupportVectorMachine->Initialize();

  //
  // copy relevent precomputed distances to SVM solver
  //
  if (this->m_PrecomputeKernelMatrix)
  {
    this->UpdateSolversKernelMatrix();
  }

  this->m_SVMSolver->SetClass1TrainingSamples(this->m_TrainingSets[0]);
  this->m_SVMSolver->SetClass2TrainingSamples(this->m_TrainingSets[1]);
  this->m_SVMSolver->SetSVM(this->m_SupportVectorMachine);
  this->m_SVMSolver->Update();
}

template<class TMembershipSample>
void
SVMJackknifeClassificationErrorEstimator<TMembershipSample>::
Update() 
{
  if (this->m_PrecomputeKernelMatrix)
  {
    this->PrecomputeKernelMatrix();
  }
  Superclass::Update();
}

} // end namespace Statistics
} // end namespace itk
#endif // SVMJackknifeClassificationErrorEstimator_txx
