/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMahalanobisDistanceMetric.h,v $
  Language:  C++
  Date:      $Date: 2005/11/21 02:40:48 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMahalanobisDistanceMetric_h
#define __itkMahalanobisDistanceMetric_h

#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_ref.h>
#include <vnl/vnl_transpose.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_determinant.h>
#include "itkArray.h"

#include "itkDistanceMetric.h"


namespace itk{ 
namespace Statistics{

/** \class MahalanobisDistanceMetric
 * \brief MahalanobisDistanceMetric class computes a Mahalanobis distance given a mean and covariance.
 *
 * This class keeps parameter to define MahalanobisDistance Density Function
 * and has method to return the probability density of an instance.
 * MeasurementVectorSize is the dimension of measurement space. The computed
 * distance is returned in a double type.
 *
 * \sa DistanceMetric
 *
 */

template< class TVector >
class ITK_EXPORT MahalanobisDistanceMetric :
    public DistanceMetric< TVector >
{
public:
  /** Standard class typedefs */
  typedef MahalanobisDistanceMetric                 Self;
  typedef DistanceMetric< TVector >                 Superclass ;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;

  /** Strandard macros */
  itkTypeMacro(MahalanobisDistanceMetric, DistanceMetric);
  itkNewMacro(Self);
  
  /** Typedef alias for the measurement vectors */
  typedef TVector MeasurementVectorType ;

  /** Typedef to represent the length of measurement vectors */
  typedef typename Superclass::MeasurementVectorSizeType MeasurementVectorSizeType;

  /** Type used for representing the mean vector */
  typedef vnl_vector<double>  MeanVectorType;

  /** Type used for representing the covariance matrix */
  typedef vnl_matrix<double>  CovarianceMatrixType;

  /**  Set the length of each measurement vector. */
  virtual void SetMeasurementVectorSize( const MeasurementVectorSizeType );
  
  /** Method to set mean */
  void SetMean(const MeanVectorType &mean) ;
  void SetMean(const Array< double > &mean) ;
  
  /** Method to get mean */
  const MeanVectorType & GetMean() const;

  /**
   * Method to set covariance matrix
   * Also, this function calculates inverse covariance and pre factor of 
   * MahalanobisDistance Distribution to speed up GetProbability */
  void SetCovariance(const CovarianceMatrixType &cov); 
  
  /** Method to get covariance matrix */
  itkGetConstReferenceMacro( Covariance, CovarianceMatrixType );

  /**
   * Method to set covariance matrix
   * Also, this function calculates inverse covariance and pre factor of 
   * MahalanobisDistance Distribution to speed up GetProbability */
  void SetInverseCovariance(const CovarianceMatrixType &invcov); 
  
  /** Method to get covariance matrix */
  itkGetConstReferenceMacro( InverseCovariance, CovarianceMatrixType );

  /** Method to set the number of samples */
  itkSetMacro( NumberOfSamples, double );

  /** Method to get the number of samples */
  itkGetMacro( NumberOfSamples, double );

  /**
   * Method to get probability of an instance. The return value is the
   * value of the density function, not probability. */
  double Evaluate(const MeasurementVectorType &measurement) const;
  
protected:
  MahalanobisDistanceMetric(void) ;
  virtual ~MahalanobisDistanceMetric(void) {}
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  MeanVectorType        m_Mean;              // mean
  CovarianceMatrixType  m_Covariance;        // covariance matrix

  // inverse covariance matrix which is automatically calculated 
  // when covariace matirx is set.  This speed up the GetProbability()
  CovarianceMatrixType  m_InverseCovariance; 

  // Number of samples defining this density
  double m_NumberOfSamples;
  // pre_factor which is automatically calculated 
  // when covariace matirx is set.  This speeds up the GetProbability()  
  double       m_PreFactor;
  double       m_Epsilon;
  double       m_DoubleMax;
 
  mutable vnl_matrix< double > m_TempVec;
  mutable vnl_matrix< double > m_TempMat;

  void CalculateInverseCovariance(); 

  /** Gets the distance between x1 and x2 points This concept does not apply to
   * the MahalanobisDistance, therefore the method is declared private and
   * purposely not implemented. */
  virtual double Evaluate(
    const MeasurementVectorType &x1, const MeasurementVectorType &x2) const; // purposely not implemented
  
};

} // end of namespace Statistics
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMahalanobisDistanceMetric.txx"
#endif

#endif
