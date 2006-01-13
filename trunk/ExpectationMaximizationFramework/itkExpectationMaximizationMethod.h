/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkExpectationMaximizationMethod.h,v $
  Language:  C++
  Date:      $Date: 2006/1/12 21:57:22 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkExpectationMaximizationMethod_h
#define __itkExpectationMaximizationMethod_h

namespace itk
{

/** \class ExpectationMaximizationMethod
 * \brief Base class for Expectation Maximization Algorithms
 *
 * ExpectationMaximizationMethod is the base class of a set of
 * algorithms...
 *
 * Expectation Maximization addresses the problem of observing a set
 * of random variables $Z$ but not the values of a set of random
 * variables $Y$, and finding the maximum likelihood estimate for the
 * parameters of a model of $Y$ and $Z$ based on the observations $Z$.
 *
 * THe outputs of the method are the parameters of the model and the
 * posterior probabilities of the unobserved variables.
 *
 */
template <typename TObservationsZ, typename TUnobservedVariablesY, typename TParametersTheta >
class ITK_EXPORT ExpectationMaximizationMethod : public ProcessObject 
{
public:
  typedef TObservationsZ   ObservationsType;
  typedef TUnobserveredY   UnobservedDataType;
  typedef TParametersTheta ParametersType;

  /**
   * Set the observations for the problem.
   */
  void SetObservations(const ObservationsType *observations)
    {
      if (observations && observations != this->GetInput(0).GetPointer() )
        {
        this->ProcessObject::SetNthInput(0, const_cast< ObservationType*>(observations));
        this->Modified();
        }
    }

  /**
   * Get the observations for the problem. The observations are
   * typically referred to as $Z$.
   */
  const ObservationsType* GetObservations() const
    { return this->GeInput(); }

  /**
   * Get the current set of parameters. THe parameters are typically
   * referred to as $\theta$.
   */
  const ParametersType *GetParameters() const
    { return this->GetOutput(0); }


  /**
   * Get the posterior probability of the unobserved variables.  Note
   * that the unobserved variables are not computed but rather the
   * posterior probability of the unobserved variables.  In the
   * unobserved variable in an indicator variable (segment or class
   * identifier) then the output corresponds to the expected value of
   * the unobserved data.
   */
  const UnobservedVariablesType *GetUnobservedVariablesPosterior() const
    { return this->GetOutput(1); };
  

  /**
   * This process object has two outputs of different types.  Need to
   * provide a method to construct each outputs.
   */
  virtual DataObjectPointer MakeOutput(unsigned int idx);

  /**
   * Perform the E-Step. The E-Step computes the posterior
   * probability of the unobserved variables using the observations
   * and the current estimated parameters.
   *
   * This will modify output 1.
   */
  virtual void ComputeExpectation() = 0;
  
  /**
   * Perform the M-Step. The M-Step computes the maximum likelhood
   * estimate of the parameters using the observations and the
   * estimated unobserved variables.
   *
   * This will modify output 0.
   */
  virtual void ComputeMaximization() = 0;

  /** Set the maximum of iterations. This is part of the termination
   * criterion. */
  itkSetMacro(MaximumNumberOfIterations, long);
  itkGetMacro(MaximimNumberOfIterations, long);

  /**
   * Convergence criteria
   */
  virtual bool Converged() = 0;
  
protected:
  ExpectationMaximizationMethod();
  virtual ~ExpectationMaximizationMethod() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  
  /** Method invoked by the pipeline in order to trigger the EM algorithm */
  void  GenerateData ();
  
private:
  ExpectationMaximizationMethod(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  long m_MaximimNumberOfIterations;
};


} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkExpectationMaximizationMethod.txx"
#endif

#endif
