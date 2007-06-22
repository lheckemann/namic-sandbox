/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMutualInformationImageToImageMetric.txx,v $
  Language:  C++
  Date:      $Date: 2006/03/19 04:36:55 $
  Version:   $Revision: 1.60 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _RegisterToMeanHistogramMultiImageMetric_cxx
#define _RegisterToMeanHistogramMultiImageMetric_cxx

#include "RegisterToMeanHistogramMultiImageMetric.h"


#include "itkCovariantVector.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "vnl/vnl_math.h"
#include "itkGaussianKernelFunction.h"
#include <cmath>

namespace itk
{

/*
 * Constructor
 */
template < class TFixedImage >
RegisterToMeanHistogramMultiImageMetric < TFixedImage >::
RegisterToMeanHistogramMultiImageMetric()
{
   m_HistogramSamplesPerBin = 50;

}


/*
 * Initialize
 */
template <class TFixedImage> 
void
RegisterToMeanHistogramMultiImageMetric<TFixedImage>
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();

  // Number of Spatial Samples must be an even number
  // First half is the set A and second half is the set B
  this->m_NumberOfSpatialSamples -= this->m_NumberOfSpatialSamples%2;


  mean.resize(this->m_NumberOfSpatialSamples);

  m_MeanStandardDeviation = this->m_ImageStandardDeviation;// / sqrt( (double) this->m_NumberOfImages);

  // Initialize the histogram
  m_HistogramArray.resize(this->m_NumberOfThreads);
  for(int i=0; i<this->m_NumberOfThreads; i++)
  {
    m_HistogramArray[i] = HistogramType::New();
  }

  // Set the minimum and maximum of input
  double min = 1e308;
  double max = -1e308;
  for(int i=0; i<this->m_NumberOfSpatialSamples; i++)
  {
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      if( this->m_Sample[i].imageValueArray[j] > max)
      {
        max = this->m_Sample[i].imageValueArray[j];
      }
      
      if( this->m_Sample[i].imageValueArray[j] < min)
      {
        min = this->m_Sample[i].imageValueArray[j];
      }
    }
  }
  m_LowerBound[0] = min - 0.05;
  m_LowerBound[1] = min - 0.05;
  m_UpperBound[0] = max + 0.1;
  m_UpperBound[1] = max + 0.1;
  m_HistogramSize.Fill((int)sqrt(this->m_NumberOfSpatialSamples/m_HistogramSamplesPerBin));
  
}

/*
 * Get the match Measure
 */
template < class TFixedImage >
typename RegisterToMeanHistogramMultiImageMetric < TFixedImage >::MeasureType
RegisterToMeanHistogramMultiImageMetric <TFixedImage >::
GetValue(const ParametersType & parameters) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValue(parameters);
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;


  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallbackGetValue, &str);
  
  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Call a method that can be overridden by a subclass to perform
  // some calculations after all the threads have completed
  return this->AfterGetThreadedValue();

}

// Callback routine used by the threading library. This routine just calls
// the GetThreadedValue() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
RegisterToMeanHistogramMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValue( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValue( threadId );


  return ITK_THREAD_RETURN_VALUE;
}



/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void 
RegisterToMeanHistogramMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue(const ParametersType & parameters) const
{

  // Compute the mean of the parameters
  ParametersType parametersMean(this->numberOfParameters);
  parametersMean.Fill(0.0);
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < this->numberOfParameters; j++)
    {
      parametersMean[j] += parameters[this->numberOfParameters * i + j];
    }
  }
  parametersMean /= (double)this->m_NumberOfImages;
  // if affine
  if(this->numberOfParameters==12)
  {
    parametersMean[0] -= 1.0;
    parametersMean[4] -= 1.0;
    parametersMean[8] -= 1.0;
  }
  
  //Make sure that each transform parameters are updated
  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < this->numberOfParameters; j++)
    {
      this->m_TransformParametersArray[i][j] = parameters[this->numberOfParameters * i + j]-parametersMean[j];
    }
    this->m_TransformArray[i]->SetParameters(this->m_TransformParametersArray[i]);
  }
  
  // collect sample set
  this->SampleFixedImageDomain (this->m_Sample);

  // compute the mean
  const unsigned int N = this->m_NumberOfImages;
  for(int i=0; i< this->m_NumberOfSpatialSamples; i++ )
  {
    mean[i] = 0.0;
    for(int j=0; j<N; j++)
    {
      mean[i] += this->m_Sample[i].imageValueArray[j];
    }
    mean[i] /= (double) N;
  }
  
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void
RegisterToMeanHistogramMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  const int N = this->m_NumberOfImages;
  /** The tranform parameters vector holding i'th images parameters
  Copy parameters in to a collection of arrays */

  //Calculate the entropy
  this->m_value[threadId] = 0.0;
  // Loop over first sample set
  for( int l=threadId; l < N; l+= this->m_NumberOfThreads)
  {
    // Initialize the histogram
    m_HistogramArray[threadId]->Initialize( m_HistogramSize, m_LowerBound, m_UpperBound );
    // Sample for histogram
    typename HistogramType::MeasurementVectorType sample;


    // populate the histogram
    for (int x_i=0; x_i<this->m_NumberOfSpatialSamples; x_i++ )
    {

      sample[0] = mean[x_i];
      sample[1] = this->m_Sample[x_i].imageValueArray[l];
      m_HistogramArray[threadId]->IncreaseFrequency(sample, 1);
    
    }  // End of sample loop

    // Compute the probabilities
    MeasureType entropyX = NumericTraits<MeasureType>::Zero;
    MeasureType jointEntropy = NumericTraits<MeasureType>::Zero;
    HistogramFrequencyType totalFreq = m_HistogramArray[threadId]->GetTotalFrequency();

    for (unsigned int i = 0; i < m_HistogramSize[1]; i++)
    {
      HistogramFrequencyType freq = m_HistogramArray[threadId]->GetFrequency(i, 1);
      if (freq > 0)
      {
        entropyX += freq*vcl_log(freq);
      }
    }
    this->m_value[threadId] += -entropyX/static_cast<MeasureType>(totalFreq) + vcl_log(totalFreq);


    // Compute the probabilities
    entropyX = NumericTraits<MeasureType>::Zero;

    for (unsigned int i = 0; i < m_HistogramSize[0]; i++)
    {
      HistogramFrequencyType freq = m_HistogramArray[threadId]->GetFrequency(i, 0);
      if (freq > 0)
      {
        entropyX += freq*vcl_log(freq);
      }
    }
    this->m_value[threadId] += -entropyX/static_cast<MeasureType>(totalFreq) + vcl_log(totalFreq);

 
    HistogramIteratorType it = m_HistogramArray[threadId]->Begin();
    HistogramIteratorType end = m_HistogramArray[threadId]->End();
    while (it != end)
    {
      HistogramFrequencyType freq = it.GetFrequency();
      if (freq > 0)
      {
        jointEntropy += freq*vcl_log(freq);
      }
      ++it;
    }

    this->m_value[threadId] -= -jointEntropy/
        static_cast<MeasureType>(totalFreq) + vcl_log(totalFreq );

  }

}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
typename RegisterToMeanHistogramMultiImageMetric < TFixedImage >::MeasureType
RegisterToMeanHistogramMultiImageMetric < TFixedImage >
::AfterGetThreadedValue() const
{

  MeasureType value = NumericTraits< RealType >::Zero;

  // Sum over the values returned by threads
  for( unsigned int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
  }
  value /= (double)( -1.0 * this->m_NumberOfImages );

  return value;
}


template < class TFixedImage >
void RegisterToMeanHistogramMultiImageMetric < TFixedImage >
::GetValueAndDerivative(const ParametersType & parameters,
                          MeasureType & value,
                          DerivativeType & derivative) const
{

  // Currently we only compute value
  // Use simplex optimizer

  itkExceptionMacro(<<"RegisterToMeanHistogramMultiImageMetric does not support derivatives: Please use simplex optimizer");


}


}        // end namespace itk


#endif
