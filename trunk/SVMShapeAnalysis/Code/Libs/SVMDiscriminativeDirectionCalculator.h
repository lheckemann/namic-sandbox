#ifndef SVMDiscriminativeDirectionCalculator_H
#define SVMDiscriminativeDirectionCalculator_H

#include <itkObject.h>
#include <itkObjectFactory.h>

namespace itk{
namespace Statistics{

/**
 * SVMDiscriminativeDirectionCalculator computes the discriminative
 * direction of a vector (or of a support vector) given a trained
 * support vector machine.  Discriminative directions for SVMs were
 * introduced in "Detection and analysis of statistical differences in
 * anatomical shape" by P Golland, W Grimson, M Shenton, and R Kikinis
 * (Medical Imaga Analysis 9 (2005) 69--86).
 *
 * The user should set the support vector machine and then call one of
 * the CalculateDiscriminativeDirection methods.
 *
 * Calculation is supported for linear and rbf kernels.  
 */

template <class TSupportVectorMachine>
class ITK_EXPORT SVMDiscriminativeDirectionCalculator 
  : public itk::Object
{
public:
  /** standard itk class typedefs */
  typedef SVMDiscriminativeDirectionCalculator       Self;
  typedef Object                                     Superclass;
  typedef SmartPointer<Self>                         Pointer;
  typedef SmartPointer<const Self>                   ConstPointer;

  /** sample typedef alias */
  typedef TSupportVectorMachine                      SupportVectorMachineType;
  
  /** the type of measurement vector used by the SVM */
  typedef typename SupportVectorMachineType::MeasurementVectorType 
  MeasurementVectorType;
  
  /** standard ITK macros */
  itkNewMacro(Self);
  itkTypeMacro(SVMDiscriminativeDirectionCalculator, Object);

  /** constructor/destructor */
  SVMDiscriminativeDirectionCalculator()  {};
  ~SVMDiscriminativeDirectionCalculator() {};

  /** access support vector machine that is used to calculate
   *  discriminitve directions
   */
  itkSetConstObjectMacro(SupportVectorMachine, SupportVectorMachineType);
  itkGetConstObjectMacro(SupportVectorMachine, SupportVectorMachineType);

  /** compute the discriminative direction (and assiciated error) of
   *  the classifier at the point specified by the input vector
   */
  void
  CalculateDiscriminativeDirection(const MeasurementVectorType& inputVector,
                                   MeasurementVectorType& 
                                   discriminativeDirection,
                                   double& error) const;

  /** compute the discriminative direction (and assiciated error) of
   *  the classifier at the support vector specified by the index
   */
  void
  CalculateDiscriminativeDirection(unsigned int svIndex,
                                   MeasurementVectorType& 
                                   discriminativeDirection,
                                   double& error) const;

private:
  // purposely not implemented
  SVMDiscriminativeDirectionCalculator(const Self&); 
  // purposely not implemented  
  void operator=(const Self&);            

  /** compute the discriminative direction (and associated error) of
   *  the linear SVM classifier at the point specified by the input
   *  vector
   */
  void
  CalculateDiscriminativeDirectionLinearKernel(const MeasurementVectorType& 
                                               inputVector,
                                               MeasurementVectorType& 
                                               discriminativeDirection,
                                               double& error) const;

  /** compute the discriminative direction (and associated error) of
   *  the RBF SVM classifier at the point specified by the input
   *  vector
   */
  void
  CalculateDiscriminativeDirectionRBFKernel(const MeasurementVectorType& 
                                            inputVector,
                                            MeasurementVectorType& 
                                            discriminativeDirection,
                                            double& error) const;

  /** the parameters of this trained support vector machine are used
   *  to compute the discriminative directions
   */
  typename SupportVectorMachineType::ConstPointer m_SupportVectorMachine;
};

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "SVMDiscriminativeDirectionCalculator.txx"
#endif

#endif // SVMDiscriminativeDirectionCalculator_H
