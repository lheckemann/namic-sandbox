#ifndef SVMDiscriminativeDirectionCalculator_TXX
#define SVMDiscriminativeDirectionCalculator_TXX

#include <itkLinearSVMKernel.h>
#include <itkRBFSVMKernel.h>

namespace itk{
namespace Statistics{

template<class TSupportVectorMachine>
void
SVMDiscriminativeDirectionCalculator<TSupportVectorMachine>::
CalculateDiscriminativeDirection(const MeasurementVectorType& inputVector,
                                 MeasurementVectorType& 
                                 discriminativeDirection,
                                 double& error) const
{
  std::string kernelClassName = 
    this->m_SupportVectorMachine->GetKernel()->GetNameOfClass();
  
  if (kernelClassName == "LinearSVMKernel")
  {
    CalculateDiscriminativeDirectionLinearKernel(inputVector,
                                                 discriminativeDirection,
                                                 error);
  }
  else if (kernelClassName == "RBFSVMKernel")
  {
    CalculateDiscriminativeDirectionRBFKernel(inputVector,
                                              discriminativeDirection,
                                              error);
  }
  else
  {
    itkExceptionMacro(<< "Invalid SVMKernel type: " << kernelClassName);
  }
}

template<class TSupportVectorMachine>
void
SVMDiscriminativeDirectionCalculator<TSupportVectorMachine>::
CalculateDiscriminativeDirection(unsigned int svIndex,
                                 MeasurementVectorType& 
                                 discriminativeDirection,
                                 double& error) const
{
  const MeasurementVectorType &sv = 
    this->m_SupportVectorMachine->GetSupportVector(svIndex);
  CalculateDiscriminativeDirection(sv,
                                   discriminativeDirection,
                                   error);
}

template<class TSupportVectorMachine>
void
SVMDiscriminativeDirectionCalculator<TSupportVectorMachine>::
CalculateDiscriminativeDirectionLinearKernel(const MeasurementVectorType& 
                                             inputVector,
                                             MeasurementVectorType& 
                                             discriminativeDirection,
                                             double& error) const
{
  //
  // zero out discriminativeDirection
  //
  unsigned int vectorDimension = inputVector.Size();
  for (unsigned int i = 0; i < vectorDimension; ++i)
  {
    discriminativeDirection[i] = 0;
  }

  //
  // In this case the discriminative direction is just w and the error
  // is 0 (equation 33). That is, 
  //
  // dx = \sum \alpha_k y_k x_k
  // (equation 32)
  //
  // where the sum is over the support vectors, alpha is the weight
  // for support vector k, y is the label for support vector k, and x
  // is support vector k
  //
  error = 0.0;
  unsigned int numSupportVectors = 
    this->m_SupportVectorMachine->GetNumberOfSupportVectors();
  for (unsigned int vecIndex = 0; vecIndex < numSupportVectors; ++vecIndex)
  {
    double alphaTimesLabel = 
      this->m_SupportVectorMachine->GetAlpha(vecIndex) *
      this->m_SupportVectorMachine->GetLabel(vecIndex);
    const MeasurementVectorType &sv = 
      this->m_SupportVectorMachine->GetSupportVector(vecIndex);

    for (unsigned int dimIndex = 0; dimIndex < vectorDimension; ++dimIndex)
    {
      discriminativeDirection[dimIndex] += alphaTimesLabel * sv[dimIndex];
    }
  }
}

template<class TSupportVectorMachine>
void
SVMDiscriminativeDirectionCalculator<TSupportVectorMachine>::
CalculateDiscriminativeDirectionRBFKernel(const MeasurementVectorType& 
                                          inputVector,
                                          MeasurementVectorType& 
                                          discriminativeDirection,
                                          double& error) const
{
  //
  // zero out discriminativeDirection
  //
  unsigned int vectorDimension = inputVector.Size();
  for (unsigned int i = 0; i < vectorDimension; ++i)
  {
    discriminativeDirection[i] = 0;
  }

  //
  // compute discriminitve direction
  //
  // dx    = \sum \alpha_k y_k K(x,x_k) (x-x_k)
  // (equation 35)
  //
  // where the sum is over the support vectors, alpha is the weight
  // for support vector k, y is the label for support vector k, x is
  // support vector k, and K is the kernel associated with the support
  // vector machine
  //
  unsigned int numSupportVectors = 
    this->m_SupportVectorMachine->GetNumberOfSupportVectors();

  // get a pointer to the kernel
  typename SupportVectorMachineType::KernelType::ConstPointer kernel = 
    this->m_SupportVectorMachine->GetKernel();

  for (unsigned int vecIndex = 0; vecIndex < numSupportVectors; ++vecIndex)
  {
    // compute \alpha_k * y_k
    double alphaTimesLabel = 
      this->m_SupportVectorMachine->GetAlpha(vecIndex) *
      this->m_SupportVectorMachine->GetLabel(vecIndex);

    // compute K(x,x_k)
    const MeasurementVectorType &sv = 
      this->m_SupportVectorMachine->GetSupportVector(vecIndex);
    double Kxx_k = kernel->Evaluate(inputVector, sv);

    for (unsigned int dimIndex = 0; dimIndex < vectorDimension; ++dimIndex)
    {
      discriminativeDirection[dimIndex] += 
        alphaTimesLabel * Kxx_k * (inputVector[dimIndex] - sv[dimIndex]);
    }
  }

  //
  // compute error
  // nb: gamma for the kernel is 1/gamma for the paper
  //
  // E(dx) = 2 \gamma - \| w \|^{-2} \|\nabla f_k^T(x) \| ^2
  // (equation 36)
  //

  //
  // compute squared norm of w
  //
  // \|w\|^2 = \sum_{i,j} \alpha_i \alpha_j y_i \y_j K(x_i,x_j)
  // (equation 20)
  //
  double normWSquared = 0.0;
  for (unsigned int i = 0; i < numSupportVectors; ++i)
  {
    const MeasurementVectorType &svi = 
      this->m_SupportVectorMachine->GetSupportVector(i);
    for (unsigned int j = i; j < numSupportVectors; ++j)
    {
      const MeasurementVectorType &svj = 
        this->m_SupportVectorMachine->GetSupportVector(j);
      normWSquared += 
        this->m_SupportVectorMachine->GetAlpha(i) *
        this->m_SupportVectorMachine->GetAlpha(j) *        
        this->m_SupportVectorMachine->GetLabel(i) *
        this->m_SupportVectorMachine->GetLabel(j) *                
        kernel->Evaluate(svi,svj);
    }
  }

  //
  // compute squared norm of \nabla f_k^T(x) 
  //
  // \nabla f_k^T(x) = dx
  // (equation 29)
  //
  double normdxSquared = 0.0;
  for (unsigned int i = 0; i < vectorDimension; ++i)
  {
    normdxSquared += discriminativeDirection[i] * discriminativeDirection[i];
  }

  //
  // compute error 
  // nb: gamma for the kernel is 1/gamma for the paper
  //
  typedef const RBFSVMKernel<MeasurementVectorType, double>* RBFKernelPtrType;
  RBFKernelPtrType rbfKernelPtr = 
    dynamic_cast<RBFKernelPtrType>(kernel.GetPointer());
  error = 2 * rbfKernelPtr->GetGamma() - normdxSquared / normWSquared;
}

} // end of namespace Statistics
} // end of namespace itk

#endif // SVMDiscriminativeDirectionCalculator_TXX
