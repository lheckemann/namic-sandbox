/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGaussianGoodnessOfFitComponent.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:29:45 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGaussianGoodnessOfFitComponent_h
#define __itkGaussianGoodnessOfFitComponent_h

#include "itkGoodnessOfFitComponentBase.h"
#include "itkGaussianDensityFunction.h"
#include "itkFunctionBase.h"
#include "itkWeightedCovarianceCalculator.h"
#include "itkSymmetricEigenSystem.h"

namespace itk{ 
namespace Statistics{

/** \class GaussianGoodnessOfFitComponent 
 *  \brief is a GoodnessOfFitComponent for Gaussian distribution.
 *
 * Among the GoodnessOfFitComponentBase's methods, this class provides
 * implementations for the CalculateProjectionAxess, the
 * GetCumulativeProbability (univariate CDF), and the
 * GetProbabilityDensity (multivariate PDF)methods.
 *
 * The CalculateProjectionAxes method creats an array of projection
 * axes that are the eigen vectors generated from the weighted
 * covariance matrix of the resampled sample using a spherical kernel.
 *
 * \sa GoodnessOfFitComponentBase, GoodnessOfFitMixtureModelCostFunction
 */

template< class TInputSample >
class GaussianGoodnessOfFitComponent 
  : public GoodnessOfFitComponentBase< TInputSample > 
{
public:
  /** Standard class typedefs */
  typedef GaussianGoodnessOfFitComponent Self;
  typedef GoodnessOfFitComponentBase< TInputSample > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(GaussianGoodnessOfFitComponent, 
               GoodnessOfFitComponentBase) ;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self) ;

  /** REMOVED: THE StaticConstMacro for this method has been removed to 
   * allow the measurement vector length to be specified at run time.
   *
   * Please use the Get macros to access the MeasurementVectorLength
   * instead. Note that GetMeasurementVectorSize() will return 0 unless
   * you have plugged in the input sample using the SetInputSample() 
   * method
   *
   * NOTE: This means that you will no longer be able to get the 
   * MeasurementVectorLength as a static const member any more.
   */
   //itkStaticConstMacro(MeasurementVectorSize, unsigned int,
   //                    TInputSample::MeasurementVectorSize) ;
  /** Typedefs from input sample */


  typedef typename TInputSample::MeasurementType MeasurementType ;
  typedef typename TInputSample::MeasurementVectorType MeasurementVectorType ;

  /** Typedefs from Superclass */
  typedef typename Superclass::CenterType CenterType ;
  typedef typename Superclass::RadiusType RadiusType ;
  typedef typename Superclass::MeanType MeanType ;
  typedef typename Superclass::StandardDeviationType StandardDeviationType ;
  typedef typename Superclass::ResampledSampleType ResampledSampleType ;
  typedef typename Superclass::ProjectionAxisArrayType ProjectionAxisArrayType;
  typedef Array< double > ParametersType ;

  /** Weight function type. The density values are used as weights of 
   * each instance (measurement vector) for the Covariance calulator */
  typedef GaussianDensityFunction< MeasurementVectorType > 
  ProbabilityDensityFunctionType ;

  typedef typename ProbabilityDensityFunctionType::CovarianceType CovarianceType ;

  /** Type of the covariance calculator. the output of this calculator is
   * a covariance matrix that is used as the input of the Projection 
   * calculator */
  typedef WeightedCovarianceCalculator< ResampledSampleType > 
  CovarianceCalculatorType ;

  /** Default projection axis calculator type
   * NOTE: Eigen analysis is done with itk::SymmetricEigenAnalysis. We do not
   * need the MeasurementVectorSize as a template parameter to use this class.*/
  typedef Array< double > EigenValuesArrayType;
  typedef SymmetricEigenAnalysis< ProjectionAxisArrayType, EigenValuesArrayType >
          ProjectionAxisCalculatorType;
  //typedef SymmetricEigenSystem< double, 
  //                              itkGetStaticConstMacro(MeasurementVectorSize) >
  //ProjectionAxisCalculatorType ;

  /** Gets the size of parameters which consists of mean
   * and standard deviation */
  unsigned int GetNumberOfParameters() const
  { return (unsigned int)(itkGetStaticConstMacro(MeasurementVectorSize) + 1) ; }

  /** Sets the component distribution parameters */
  void SetParameters(const ParametersType &parameter) ;

  /** Gets the center point for the neighborhood sampling */
  CenterType* GetCenter() ;

  /** Gets the radius for the neighborhood sampling */
  RadiusType* GetRadius() ;
  
  /** Gets the mean of the distributon */
  MeanType* GetMean() ;

  /** Gets the standard deviation of the distribution */
  StandardDeviationType* GetStandardDeviation() ;

  /** Univariate (standard) cumulative probability function */
  double GetCumulativeProbability(double x) const ;

  /** Multivariate probability density function */
  double GetProbabilityDensity(MeasurementVectorType &measurements) const ;

  /** Prints all the parameters. Usually for debugging. */
  void PrintParameters(std::ostream &os) const ;

  /** Gets the full distribution parameters which consists of
   * mean vector and covariance matrix in a single array */
  ParametersType GetFullParameters() const ;

protected:
  GaussianGoodnessOfFitComponent() ;
  virtual ~GaussianGoodnessOfFitComponent() ;
  virtual void PrintSelf(std::ostream& os, Indent indent) const ;

  /** Calculates the base axes for projection */
  virtual void CalculateProjectionAxes() ;

private:
  typename ProbabilityDensityFunctionType::Pointer 
  m_ProbabilityDensityFunction ;
  typename CovarianceCalculatorType::Pointer m_CovarianceCalculator ;
  typename ProjectionAxisCalculatorType::Pointer m_ProjectionAxisCalculator ;

  MeanType m_Mean ;
  CenterType m_Center ;
  RadiusType m_Radius ;
  StandardDeviationType m_StandardDeviation ;
  CovarianceType m_Covariance ;

  unsigned int m_NumberOfParameters ;

  int m_LongestAxisIndex ;
  double m_LargestEigenValue ;
} ; // end of class

} // end of namespace Statistics 
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGaussianGoodnessOfFitComponent.txx"
#endif

#endif

