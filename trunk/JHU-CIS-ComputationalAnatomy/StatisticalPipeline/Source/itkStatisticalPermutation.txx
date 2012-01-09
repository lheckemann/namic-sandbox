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


#ifndef __itkStatPerm_txx
#define __itkStatPerm_txx
#ifdef _MSC_VER
#pragma warning( disable : 4288 )
#endif

#include "itkStatisticalPermutation.h"
#include "itkImageRandomNonRepeatingConstIteratorWithIndex.h"
#include "itkHistogram.h"
#include "vnl_index_sort.h"

// debug
#include <vnl/vnl_matlab_read.h>
#include <vcl_fstream.h>
// end debug

namespace itk
{
template <
    typename TPopulationPointSetType,
    typename TRankValueType  >
void SimpleStatPerm<TPopulationPointSetType, TRankValueType >::Evaluate(
    const MeasurementSetTypePointer X,
    int cntlCount,
    StatRankType& T0) const
{
}

template <
    typename TPopulationPointSetType,
    typename TRankValueType  >
void VectorRankSum<TPopulationPointSetType, TRankValueType >::Evaluate(
    const MeasurementSetTypePointer X,
    int cntlCount,
    StatRankType& T0) const
{
  if (X->Size() <= 0)
  {
    itkExceptionMacro("Empty data set.");
    return;
  }

  if (X->Size() <= cntlCount)
  {
    itkExceptionMacro("Patient Count <= zero.");
    return;
  }

  unsigned int setSize = X->Size();
  unsigned int measurementCount = X->ElementAt(0).rows();
  unsigned int ptCount = X->Size() - cntlCount;

  vnl_matrix<double> sortMatrix(measurementCount, setSize);
  vnl_matrix<TRankValueType> sortIndices(measurementCount, setSize);

  for (unsigned int colIx = 0; colIx < setSize; colIx++)
    sortMatrix.set_column(colIx, X->ElementAt(colIx).get_column(0));

//vcl_cout << "sortMatrix row1"  << vcl_endl << sortMatrix.get_row(0) << vcl_endl;
//vcl_cout << "sortMatrix col1"  << vcl_endl << sortMatrix.get_column(0) << vcl_endl;

  vnl_index_sort<double, TRankValueType> sorter;
  sorter.matrix_sort(
    vnl_index_sort<double, TRankValueType>::ByRow,
    sortMatrix, sortMatrix, sortIndices);

  T0.set_size(measurementCount);
  double accum = 0.0;
  double center = ptCount * (setSize + 1) / 2;
  for (unsigned int rowIx = 0; rowIx < measurementCount; rowIx++)
  {
    accum = 0.0;
    for (unsigned int colIx = 0; colIx < setSize; colIx++)
      if (sortIndices(rowIx, colIx) > cntlCount - 1) accum += colIx + 1;
    T0(rowIx) = accum - center;
  }
}

/**
 * Constructor
 */
template <
    typename TPopulationPointSetType,
    typename TRankValueType,
    typename StatFunctionType >
StatPerm<TPopulationPointSetType, TRankValueType, StatFunctionType>
::StatPerm()
{
  m_CtrlCount = 0;
  m_MinMax = MaxAbsoluteValues;
  this->m_NumPerm = 0;
  m_StatPermCalculated = false;
  m_PValue = 0.0;
  m_ObtainedQuantile = 0.0;
  m_StatFunction = StatFunctionType::New();
  m_RandGen = NULL;
  m_SubjectData = NULL;
}

template <
    typename TPopulationPointSetType,
    typename TRankValueType,
    typename StatFunctionType >
void StatPerm<TPopulationPointSetType, TRankValueType, StatFunctionType>
::SetStatFunctionClass( StatFunctionPointer func )
{
  m_StatFunction->Delete();
  m_StatFunction = StatFunctionPointer::New();
}

/**
 *
 */
template <
    typename TPopulationPointSetType,
    typename TRankValueType,
    typename StatFunctionType >
void
StatPerm<TPopulationPointSetType, TRankValueType, StatFunctionType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Number of Control:  " << this->m_CtrlCount << std::endl;
  os << indent << "Number of Patients:  " <<
      this->m_SubjectData->Size() - m_CtrlCount << std::endl;

  os << indent << "Number of Permutations:  " << this->m_NumPerm << std::endl;
  os << indent << "Min/Max Setting:  ";

  switch (this->m_MinMax)
  {
    case Minimums:
      os << "Minimums." << std::endl;
      break;
    case Maximums:
      os << "Maximums." << std::endl;
      break;
    case MaxAbsoluteValues:
      os << "MaxAbsoluteValues." << std::endl;
      break;
  }

  if (!this->m_StatPermCalculated)
    {
    os << indent << "PValue:  " << m_PValue << std::endl;
    os << indent << "PValue:  " << m_ObtainedQuantile << std::endl;
    }
}

  /** Set the input point set. */
template <
    typename TPopulationPointSetType,
    typename TRankValueType,
    typename StatFunctionType >
void
StatPerm<TPopulationPointSetType, TRankValueType, StatFunctionType>
::SetInput( const MeasurementSetTypePointer controlSet,
            const MeasurementSetTypePointer patientSet )
{
  if (m_SubjectData.IsNotNull())
    m_SubjectData->Delete();
  m_SubjectData = MeasurementSetType::New();
  m_CtrlCount = controlSet->Size();

  m_SubjectData->Reserve(m_CtrlCount + patientSet->Size());

  for (int i = 0; i < m_CtrlCount; i++)
    m_SubjectData->SetElement(i, controlSet->ElementAt(i));

  for (unsigned int j = 0; j < patientSet->Size(); j++)
    m_SubjectData->SetElement(j + m_CtrlCount, patientSet->ElementAt(j));
}

/**
 * Compute the StatPerm
 */
template <
    typename TPopulationPointSetType,
    typename TRankValueType,
    typename StatFunctionType >
void
StatPerm<TPopulationPointSetType, TRankValueType, StatFunctionType>
::Compute()
{
  // check parameters
  if (m_NumPerm <= 0)
    {
    itkExceptionMacro("Need positive Number of Permutations.");
    return;
    }

  if (!m_StatFunction)
    {
    itkExceptionMacro("Need to specify Statistical Differentiating Function.");
    return;
    }

  // get dimensions
  unsigned int numSubj = m_SubjectData->Size();
  unsigned int setSize;
  unsigned int setDim;

  if (numSubj)
    {
    InputPointSetType& thisField = m_SubjectData->ElementAt(0);
    setSize = thisField.rows();
    setDim = thisField.cols();
    }
  else
    {
    itkExceptionMacro("No subject (Control or Patient) data.");
    return;
    }

  typedef Statistics::Histogram<int> HistogramType;

  if (!m_RandGen)  // construction performs the first permutation
    m_RandGen = new RandomPermutation(numSubj);
  else  if (m_RandGen->m_Size != numSubj)
    {
    delete m_RandGen;
    m_RandGen = new RandomPermutation(numSubj);
    }

  // set up T0, the stat rank vector
  this->m_StatFunction->Evaluate(m_SubjectData, m_CtrlCount, m_StatVector);

  switch (m_MinMax)
    {
    case Minimums:
      m_ObtainedQuantile = m_StatVector.min_value();
      break;
    case MaxAbsoluteValues:
      m_ObtainedQuantile = m_StatVector.inf_norm();
      break;
    case Maximums:
      m_ObtainedQuantile = m_StatVector.max_value();
      break;
    default:
      itkExceptionMacro("Unknown Max/Min type.");
      return;
      break;
    }

  m_QuantileByIteration.set_size(m_NumPerm);

  StatRankType justT;

  for (int ix = 0; ix < m_NumPerm; ix++)
    {
    MeasurementSetTypePointer shuffledMeasurements = MeasurementSetType::New();
    shuffledMeasurements->Reserve(m_SubjectData->Size());

#if(0)
// for debug, need controlled shuffle variables
// read in patient data from a MATLAB file
vnl_vector<double> shuffledData;
//vnl_vector<TRankValueType> shuffledData;
vcl_ifstream shuffledDataStream(
  "/cis/home/mbowers/projects/NCBCStatisticalPipeline/LaurentMATLAB/younes092009/younes/StatToolbox/StatMomentum/shuffle.mat",
    vcl_ifstream::binary);

if (!shuffledDataStream)
{
  vcl_cerr << "Could not open shuffle file " << vcl_endl;
  exit (-1);
}
if (!vnl_matlab_read_or_die(shuffledDataStream, shuffledData, "shuffledData"))
{
  vcl_cerr << "Could not read variable \"shuffledData\" from shuffledData file " << vcl_endl;
  exit (-1);
}

//vcl_cout << "Shuffled data"  << vcl_endl << shuffledData << vcl_endl;
for (unsigned int shuffleIx = 0; shuffleIx < m_SubjectData->Size(); shuffleIx++)
  shuffledMeasurements->SetElement(shuffleIx,
                        m_SubjectData->ElementAt((int)shuffledData[shuffleIx] - 1));
#else
    for (unsigned int shuffleIx = 0; shuffleIx < m_SubjectData->Size(); shuffleIx++)
      shuffledMeasurements->SetElement(shuffleIx,
                             m_SubjectData->ElementAt((*m_RandGen)[shuffleIx]));
#endif

    m_RandGen->Shuffle();
    this->m_StatFunction->Evaluate(shuffledMeasurements, m_CtrlCount, justT);

    switch (m_MinMax)
      {
      case Minimums:
        m_QuantileByIteration(ix) = justT.min_value();
        break;
      case MaxAbsoluteValues:
        m_QuantileByIteration(ix) = justT.inf_norm();
        break;
      case Maximums:
        m_QuantileByIteration(ix) = justT.max_value();
        break;
      default:
        itkExceptionMacro("Unknown Max/Min type.");
        return;
        break;
      }

// mrb debug        if (!((ix + 1) % 20))
    if (!((ix + 1) % 1000))
      {
      HistogramType::Pointer quantileHistogram = HistogramType::New();
      quantileHistogram->SetMeasurementVectorSize(1);
//quantileHistogram->Print(vcl_cout);
      Array<long unsigned int> histSize(1);
      histSize[0] = 1000;
      typename HistogramType::MeasurementVectorType measurement(1), minVal(1), maxVal(1);
      minVal(0) = m_QuantileByIteration.extract(ix+1).min_value() - 1;
      maxVal(0) = m_QuantileByIteration.extract(ix+1).max_value() + 1;
      quantileHistogram->SetClipBinsAtEnds(false);
      quantileHistogram->Initialize(histSize, minVal, maxVal);
//quantileHistogram->Print(vcl_cout);

      double sum = 0.0;
      double thisMeasurement;
      for (int sumIx = 0; sumIx < ix + 1; sumIx++)
        {
        thisMeasurement = m_QuantileByIteration(sumIx);
//vcl_cerr << thisMeasurement << vcl_endl;
        measurement(0) = thisMeasurement;
        if (!quantileHistogram->IncreaseFrequencyOfMeasurement(measurement, 1))
          vcl_cerr << "fail - thisMeasurement" << thisMeasurement << vcl_endl;
        sum += (thisMeasurement == m_ObtainedQuantile ? 0.5 : 0.0);
        switch (m_MinMax)
          {
          case Minimums:
            sum += (thisMeasurement < m_ObtainedQuantile ? 1.0 : 0.0);
            break;
          case MaxAbsoluteValues:
          case Maximums:
            sum += (thisMeasurement > m_ObtainedQuantile ? 1.0 : 0.0);
            break;
          default:
            itkExceptionMacro("Unknown Max/Min type.");
            return;
            break;
          }
        }
//quantileHistogram->Print(vcl_cout);
      double u = quantileHistogram->Quantile(0, 0.95);
//      quantileHistogram->Delete();
      m_PValue = sum / double (ix + 1);
      vcl_cout << "permutation " << ix + 1 << ": stat = " << m_QuantileByIteration(ix) << " ("
                << m_ObtainedQuantile << " on data), 5pct qtl: " << u << " p-val: " << m_PValue << vcl_endl;
      }
    }

  HistogramType::Pointer quantileHistogram = HistogramType::New();
  quantileHistogram->SetMeasurementVectorSize(1);
//quantileHistogram->Print(vcl_cout);
  Array<long unsigned int> histSize(1);
  histSize[0] = 1000;
  typename HistogramType::MeasurementVectorType measurement(1), minVal(1), maxVal(1);
  minVal(0) = m_QuantileByIteration.extract(m_NumPerm).min_value() - 1;
  maxVal(0) = m_QuantileByIteration.extract(m_NumPerm).max_value() + 1;
  quantileHistogram->Initialize(histSize, minVal, maxVal);

  double sum = 0.0;
  double thisMeasurement;
  for (int sumIx = 0; sumIx < m_NumPerm; sumIx++)
    {
    thisMeasurement = m_QuantileByIteration(sumIx);
//vcl_cerr << thisMeasurement << vcl_endl;
    measurement(0) = thisMeasurement;
    if (!quantileHistogram->IncreaseFrequencyOfMeasurement(measurement, 1))
      vcl_cerr << "fail - thisMeasurement" << thisMeasurement << vcl_endl;
    sum += (thisMeasurement == m_ObtainedQuantile ? 0.5 : 0.0);
    switch (m_MinMax)
      {
      case Minimums:
        sum += (thisMeasurement < m_ObtainedQuantile ? 1.0 : 0.0);
        break;
      case MaxAbsoluteValues:
      case Maximums:
        sum += (thisMeasurement > m_ObtainedQuantile ? 1.0 : 0.0);
        break;
      default:
        itkExceptionMacro("Unknown Max/Min type.");
        return;
        break;
      }
    }
  double u = quantileHistogram->Quantile(0, 0.95);
  m_PValue = sum / double (m_NumPerm);
  vcl_cout << "permutation " << m_NumPerm << ": stat = " << m_QuantileByIteration(m_NumPerm - 1) << " ("
            << m_ObtainedQuantile << " on data), 5pct qtl: " << u << " p-val: " << m_PValue << vcl_endl;

//  quantileHistogram->Delete();
  delete m_RandGen;
//  m_RandGen->Delete();
}

} // end namespace itk

#endif
