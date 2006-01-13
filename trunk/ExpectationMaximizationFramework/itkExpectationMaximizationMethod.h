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

#include "itkProcessObject.h"


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
 * The outputs of the method are the parameters of the model and the
 * posterior probabilities of the unobserved variables.
 *
 * It is expected that the template arguments of this class will be
 * types deriving from itk::DataObject. If you want to use types that
 * do not derive from DataObject, it is possible to wrap them using
 * the DataObjectDecorator.
 *
 * The algorithm is based on the chapter by R. Neal and G. Hinton: "A
 * View of the EM Algorithm that Justifies Incremental, Sparse, and
 * other Variants" in M. I. Jordan, Learning in Graphical Models,
 * Kluwer, 1998.  The article can be downloaded from
 * citeseer.ist.psu.edu/neal98view.html
 *
 * \note This work is part of the National Alliance for Medical Image
 * Computing (NAMIC), funded by the National Institutes of Health
 * through the NIH Roadmap for Medical Research, Grant U54 EB005149.
 * Information on the National Centers for Biomedical Computing
 * can be obtained from http://nihroadmap.nih.gov/bioinformatics.  
 * 
 */
template <typename TObservationsZ, typename TUnobservedVariablesYPosterior, typename TParametersTheta >
class ITK_EXPORT ExpectationMaximizationMethod : public ProcessObject 
{
public:
  
  typedef TObservationsZ                    ObservationsType;
  typedef TUnobservedVariablesYPosterior    UnobservedVariablesPosteriorType;
  typedef TParametersTheta                  ParametersType;

  typedef ExpectationMaximizationMethod     Self;
  typedef ProcessObject                     Superclass;
  typedef SmartPointer< Self >              Pointer;
  typedef SmartPointer< const Self >        ConstPointer;


  //
  // No itkNewMacro() because this class is abstract
  // 


  itkTypeMacro( ExpectationMaximizationMethod, ProcessObject );



public:

  /**
   * Set the observations for the problem.
   */
  void SetObservations(const ObservationsType *observations);


  /**
   * Get the observations for the problem. The observations are
   * typically referred to as $Z$.
   */
  const ObservationsType* GetObservations() const;

  
  /**
   * Get the current set of parameters. THe parameters are typically
   * referred to as $\theta$.
   */
  const ParametersType *GetParameters() const;


  /**
   * Get the posterior probability of the unobserved variables.  Note
   * that the unobserved variables are not computed but rather the
   * posterior probability of the unobserved variables.  In the
   * unobserved variable in an indicator variable (segment or class
   * identifier) then the output corresponds to the expected value of
   * the unobserved data.
   */
  const  UnobservedVariablesPosteriorType *GetUnobservedVariablesPosterior() const;
  

  /**
   * This process object has two outputs of different types.  Need to
   * provide a method to construct each outputs.
   */
  virtual DataObjectPointer MakeOutput(unsigned int idx);

  /** Set the maximum of iterations. This is part of the termination
   * criterion. */
  itkSetMacro( MaximumNumberOfIterations, unsigned long );
  itkGetMacro( MaximumNumberOfIterations, unsigned long );


protected:

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

  /**
   * Convergence criteria
   */
  virtual bool Converged() const = 0;
  

  /**
   * Method to be overloaded by derived classes that need to perform
   * initializations before entering the iteration loop in GenerateData().
   */
  virtual void Initialize();
 

protected:
  ExpectationMaximizationMethod();
  virtual ~ExpectationMaximizationMethod() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  
  /** Method invoked by the pipeline in order to trigger the EM algorithm */
  void  GenerateData ();
  
private:
  ExpectationMaximizationMethod(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  unsigned long m_MaximumNumberOfIterations;
};


} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkExpectationMaximizationMethod.txx"
#endif

#endif
