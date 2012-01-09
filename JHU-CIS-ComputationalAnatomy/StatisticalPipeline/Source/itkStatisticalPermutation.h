/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkStatPerm_h
#define __itkStatPerm_h

#include "itkObject.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h" 
#include "itkVectorContainer.h"

namespace itk
{

/** \class DifferentiatingStatisticFunction
 * \brief Ranks two populations
 * 
 * Specify a statistical function for computing the differentiating statistic.
 *
 * \author Michael Bowers
 *
 * \ingroup Operators
 */

template <
    typename TPopulationPointSetType,
    typename TRankValueType  >
class ITK_EXPORT DifferentiatingStatisticFunction : public Object
{
  // This function takes in two populations and ranks them
public:
  /** Standard class typedefs. */
  typedef DifferentiatingStatisticFunction      Self;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;

  /** Type definitions for the PointSet. */
  typedef TPopulationPointSetType   InputPointSetType;
  /** type for collection of point sets. */
  typedef VectorContainer<unsigned int, InputPointSetType>
    MeasurementSetType;
  typedef typename MeasurementSetType::Pointer MeasurementSetTypePointer;

  /** Type definitions for the Stat Rank. */
  typedef TRankValueType   StatRankValueType;
  /** type for stat rank collection. */
  typedef vnl_vector< StatRankValueType >
    StatRankType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(DifferentiatingStatisticFunction, Object);

  /** Evaluate the function on two populations */
  virtual void Evaluate (
    const MeasurementSetTypePointer X,
    int cntlCount,
    StatRankType& T0) const = 0;

protected:  
  DifferentiatingStatisticFunction() {}
  ~DifferentiatingStatisticFunction() {}
  void PrintSelf(std::ostream& os, Indent indent) const
    {}  

private:

};


/** \class SimpleStatPerm
 * \brief Ranks two populations
 * 
 * Specify a statistical function for computing the differentiating statistic.
 *
 * \author Michael Bowers
 *
 * \ingroup Operators
 */

template <
    typename TPopulationPointSetType,
    typename TRankValueType >
class ITK_EXPORT SimpleStatPerm :
  public DifferentiatingStatisticFunction<TPopulationPointSetType, TRankValueType >
{
  // This function takes in two populations and ranks them
public:
  /** Standard class typedefs. */
  typedef SimpleStatPerm                        Self;
  typedef DifferentiatingStatisticFunction<TPopulationPointSetType, TRankValueType >
                                                Superclass;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;

  /** Type definitions for the PointSet. */
  typedef TPopulationPointSetType   InputPointSetType;
  /** type for collection of point sets. */
  typedef VectorContainer< unsigned int, InputPointSetType >
    MeasurementSetType;
  typedef typename MeasurementSetType::Pointer MeasurementSetTypePointer;

  /** Type definitions for the Stat Rank. */
  typedef TRankValueType   StatRankValueType;
  /** type for stat rank collection. */
  typedef vnl_vector< StatRankValueType > StatRankType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(SimpleStatPerm, DifferentiatingStatisticFunction);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Evaluate the function on two populations */
  virtual void Evaluate (
    const MeasurementSetTypePointer X,
    int cntlCount,
    StatRankType& T0) const;

protected:
  SimpleStatPerm() {}
  ~SimpleStatPerm() {}
  void PrintSelf(std::ostream& os, Indent indent) const
    { Superclass::PrintSelf( os, indent ); }  

private:

};


/** \class SimpleStatPerm
 * \brief Ranks two populations
 * 
 * Specify a statistical function for computing the differentiating statistic.
 *
 * \author Michael Bowers
 *
 * \ingroup Operators
 */

template <
    typename TPopulationPointSetType,
    typename TRankValueType >
class ITK_EXPORT VectorRankSum :
  public DifferentiatingStatisticFunction<TPopulationPointSetType, TRankValueType >
{
  // This function takes in two populations and ranks them
  // operates single column vnl_matrix

public:
  /** Standard class typedefs. */
  typedef VectorRankSum                        Self;
  typedef DifferentiatingStatisticFunction<TPopulationPointSetType, TRankValueType >
                                                Superclass;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;

  /** Type definitions for the PointSet. */
  /** The TPopulationPointSetType needs to be a vnl_vector instantiated type */
  typedef TPopulationPointSetType InputPointSetType;
  /** type for collection of point sets. */
  typedef VectorContainer< unsigned int, InputPointSetType >
    MeasurementSetType;
  typedef typename MeasurementSetType::Pointer MeasurementSetTypePointer;

  /** Type definitions for the Stat Rank. */
  typedef TRankValueType   StatRankValueType;
  /** type for stat rank collection. */
  typedef vnl_vector< StatRankValueType > StatRankType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(VectorRankSum, DifferentiatingStatisticFunction);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Evaluate the function on two populations */
  virtual void Evaluate (
    const MeasurementSetTypePointer X,
    int cntlCount,
    StatRankType& T0) const;

protected:
  VectorRankSum() {}
  ~VectorRankSum() {}
  void PrintSelf(std::ostream& os, Indent indent) const
    { Superclass::PrintSelf( os, indent ); }  

private:

};


class RandomPermutation;
class Histogram;
template <
    typename TPopulationPointSetType,
    typename TRankValueType,
    typename StatFunctionType = SimpleStatPerm<TPopulationPointSetType, TRankValueType >
  >
class ITK_EXPORT StatPerm : public Object 
{
public:
  /** Standard class typedefs. */
  typedef StatPerm          Self;
  typedef Object                          Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(StatPerm, Object);

  /** Type definitions for the PointSet. */
  typedef TPopulationPointSetType   InputPointSetType;
  /** type for collection of point sets. */
  typedef VectorContainer< unsigned int, InputPointSetType >
    MeasurementSetType;
  /** type for collection of point sets. */
  typedef typename MeasurementSetType::Pointer MeasurementSetTypePointer;

  /** Pointer type for the Stat Function. */
  typedef typename StatFunctionType::Pointer   StatFunctionPointer;

  /** Type definitions for the Stat Rank. */
  typedef TRankValueType   StatRankValueType;
  /** type for stat rank collection. */
  typedef vnl_vector< StatRankValueType > StatRankType;

  /** types for the output. */
  typedef vnl_vector< int > QuantileByIterationType;

  typedef enum { Minimums, Maximums, MaxAbsoluteValues } MaxMinEnumType;

  /** Set the input point set. */
  /** Control and patient input */
  void SetInput( const MeasurementSetTypePointer controlSet,
                 const MeasurementSetTypePointer patientSet );

  /** Set and get the min/max setting. */
  itkSetMacro( MinMax, MaxMinEnumType );
  itkGetMacro( MinMax, MaxMinEnumType );

  /** Set and get the number permutations. */
  itkSetMacro( NumPerm, int );
  itkGetMacro( NumPerm, int );

  /** Set pointer to the Kernel object. */
  void SetStatFunctionClass( StatFunctionPointer func );

  /** Compute the concurrent ranksum tests with p-value
      computed with random permutations. */
  void Compute(void);

  /** Return the results. */
  itkGetMacro(PValue, double);
  itkGetMacro(ObtainedQuantile, double);
  itkGetConstReferenceMacro(StatVector, StatRankType);
  itkGetConstReferenceMacro(QuantileByIteration, QuantileByIterationType);

protected:
  StatPerm();
  virtual ~StatPerm() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

private:

  StatPerm(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MeasurementSetTypePointer m_SubjectData;
  int                       m_CtrlCount;

  int                       m_NumPerm;
  MaxMinEnumType            m_MinMax;
  StatFunctionPointer       m_StatFunction;

  bool                      m_StatPermCalculated;
  double                    m_PValue;
  double                    m_ObtainedQuantile;

  StatRankType              m_StatVector;
  QuantileByIterationType   m_QuantileByIteration;

  RandomPermutation*        m_RandGen;

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkStatisticalPermutation.txx"
#endif

#endif
