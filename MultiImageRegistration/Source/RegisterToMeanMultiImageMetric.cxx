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
#ifndef _RegisterToMeanMultiImageMetric_cxx
#define _RegisterToMeanMultiImageMetric_cxx

#include "RegisterToMeanMultiImageMetric.h"


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
RegisterToMeanMultiImageMetric < TFixedImage >::
RegisterToMeanMultiImageMetric()
{


}


/*
 * Initialize
 */
template <class TFixedImage> 
void
RegisterToMeanMultiImageMetric<TFixedImage>
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();

  // Number of Spatial Samples must be an even number
  // First half is the set A and second half is the set B
  this->m_NumberOfSpatialSamples -= this->m_NumberOfSpatialSamples%2;

  // Resize w_x_j
  W_moving.resize(this->m_NumberOfThreads);
  W_joint.resize(this->m_NumberOfThreads);
  for(int i=0; i<this->m_NumberOfThreads; i++)
  {
    W_moving[i].resize(this->m_NumberOfSpatialSamples);
    W_joint[i].resize(this->m_NumberOfSpatialSamples);
  }


  mean.resize(this->m_NumberOfSpatialSamples);

  m_MeanStandardDeviation = this->m_ImageStandardDeviation;// / sqrt( (double) this->m_NumberOfImages);
}

/*
 * Get the match Measure
 */
template < class TFixedImage >
typename RegisterToMeanMultiImageMetric < TFixedImage >::MeasureType
RegisterToMeanMultiImageMetric <TFixedImage >::
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
RegisterToMeanMultiImageMetric< TFixedImage >
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
RegisterToMeanMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue(const ParametersType & parameters) const
{

  //Make sure that each transform parameters are updated
  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < this->numberOfParameters; j++)
    {
      this->m_TransformParametersArray[i][j] = parameters[this->numberOfParameters * i + j];
    }
    this->m_TransformArray[i]->SetParameters(this->m_TransformParametersArray[i]);
  }
  
  // Update intensity values
  const int N = this->m_NumberOfImages;
  for(int i=0; i< this->m_NumberOfSpatialSamples; i++ )
  {
    for(int j=0; j<N; j++)
    {
      const MovingImagePointType mappedPoint = this->m_TransformArray[j]->TransformPoint(this->m_Sample[i].FixedImagePoint);
      if(this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint) )
      {
        this->m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(mappedPoint);
      }
    }
  }

  // compute the mean
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
RegisterToMeanMultiImageMetric < TFixedImage >
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
    for (int x_i=0; x_i<this->m_NumberOfSpatialSamples; x_i ++ )
    {

      double movingProbSum = 1e-5;
      double meanProbSum = 1e-5;
      double jointProbSum = 1e-5;

      for( int x_j=0; x_j < this->m_NumberOfSpatialSamples; x_j++)
      {

        // Compute d(x_i, x_j)
        double diff1 = (this->m_Sample[x_i].imageValueArray[l] - this->m_Sample[x_j].imageValueArray[l])
                              /this->m_ImageStandardDeviation;

        diff1 = this->m_KernelFunction[threadId]->Evaluate( diff1 );
        
        double diff2 = (mean[x_i] - mean[x_j]) / m_MeanStandardDeviation;
        diff2 = this->m_KernelFunction[threadId]->Evaluate( diff2 );

        // Evaluate the probability of d(x_a, x_b)
        movingProbSum += diff1;
        meanProbSum += diff2;
        jointProbSum += diff1*diff2;
      }
      
      this->m_value[threadId] += vcl_log( movingProbSum  );
      //this->m_value[threadId] += vcl_log( meanProbSum );
      this->m_value[threadId] -= vcl_log( jointProbSum );


    }  // End of sample loop
    this->m_value[threadId] += vcl_log( (double) this->m_NumberOfSpatialSamples );
  }

}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
typename RegisterToMeanMultiImageMetric < TFixedImage >::MeasureType
RegisterToMeanMultiImageMetric < TFixedImage >
::AfterGetThreadedValue() const
{

  MeasureType value = NumericTraits< RealType >::Zero;

  // Sum over the values returned by threads
  for( unsigned int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
  }
  value /= (double)( 1.0 * this->m_NumberOfSpatialSamples * this->m_NumberOfImages );

  return value;
}

/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void 
RegisterToMeanMultiImageMetric < TFixedImage >
::BeforeGetThreadedValueAndDerivative(const ParametersType & parameters) const
{
   Superclass::BeforeGetThreadedValueAndDerivative(parameters);


   // Compute W(x_j)
   const unsigned int N = this->m_NumberOfImages;

   // Update mean
   for(unsigned int i=0; i < this->m_NumberOfSpatialSamples; i++)
   {
     mean[i] = 0.0;
     for( int j=0; j< N; j++)
     {
       mean[i] += this->m_Sample[i].imageValueArray[j];
     }
     mean[i] /= (double) N;
   }

}

template < class TFixedImage >
void RegisterToMeanMultiImageMetric < TFixedImage >
::GetValueAndDerivative(const ParametersType & parameters,
                          MeasureType & value,
                          DerivativeType & derivative) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValueAndDerivative(parameters);
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;


  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallbackGetValueAndDerivative, &str);
  
  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Call a method that can be overridden by a subclass to perform
  // some calculations after all the threads have completed
  this->AfterGetThreadedValueAndDerivative(value, derivative);

}
/*
 * Get the match Measure
 */
template < class TFixedImage >
void 
RegisterToMeanMultiImageMetric < TFixedImage >
::GetThreadedValueAndDerivative(int threadId) const
{

  const unsigned int N = this->m_NumberOfImages;
  const int numberOfThreads = this->GetNumberOfThreads();

  /** The tranform parameters vector holding i'th images parameters 
  Copy parameters in to a collection of arrays */
  MeasureType value = NumericTraits < MeasureType >::Zero;

  //Initialize the derivative array to zero
  for(int i=0; i<this->m_NumberOfImages;i++)
  {
    this->m_DerivativesArray[threadId][i].Fill(0.0);
  }
  this->m_value[threadId] = 0.0;

  
  // Calculate the derivative
  // Set the image for derivatives
  for( int l=threadId; l < N; l += this->m_NumberOfThreads)
  {

    for (int x_i=0; x_i<this->m_NumberOfSpatialSamples; x_i ++ )
    {
      // Initialize the weight in the denominator to zero
      W_moving[threadId][x_i] = 1e-5;
      W_joint[threadId][x_i] = 1e-5;
      double meanProbSum = 1e-5;

      for( int x_j=0; x_j < this->m_NumberOfSpatialSamples; x_j++)
      {

        // Compute d(x_i, x_j)
        double diff1 = (this->m_Sample[x_i].imageValueArray[l] - this->m_Sample[x_j].imageValueArray[l])
                              /this->m_ImageStandardDeviation;

        diff1 = this->m_KernelFunction[threadId]->Evaluate( diff1 );
        
        double diff2 = (mean[x_i] - mean[x_j]) / m_MeanStandardDeviation;
        diff2 = this->m_KernelFunction[threadId]->Evaluate( diff2 );

        // Evaluate the probability of d(x_a, x_b)
        meanProbSum += diff2;

        // Update the weights
        W_moving[threadId][x_i] += diff1;
        W_joint[threadId][x_i] += diff1*diff2;
      }
      
      this->m_value[threadId] += vcl_log( W_moving[threadId][x_i] );
      //this->m_value[threadId] += vcl_log( meanProbSum );
      this->m_value[threadId] -= vcl_log( W_joint[threadId][x_i]  );


    }  // End of sample loop
    this->m_value[threadId] += vcl_log ((double)this->m_NumberOfSpatialSamples);



    for (int x_i=0; x_i<this->m_NumberOfSpatialSamples; x_i ++ )
    {

      // Sum over the second sample set
      double weightMoving = 0.0;
      double weightJoint = 0.0;

      for (int x_j=0; x_j<this->m_NumberOfSpatialSamples; x_j++ )
      {

        // Compute dir(x_i, x_j)
        const double dir1 = (this->m_Sample[x_i].imageValueArray[l] - this->m_Sample[x_j].imageValueArray[l]);

        // Compute d(x_i, x_j)
        double diff1 = dir1 / this->m_ImageStandardDeviation;

        diff1 = this->m_KernelFunction[threadId]->Evaluate( diff1 );
        weightMoving += diff1 / W_moving[threadId][x_i] * dir1;
        
        double diff2 = (mean[x_i] - mean[x_j]) / m_MeanStandardDeviation;
        diff2 = this->m_KernelFunction[threadId]->Evaluate( diff2 );
        weightJoint += diff1 * diff2 / W_joint[threadId][x_i] * dir1;


      }  // End of sample loop B

      // Get the derivative for this sample
      this->UpdateSingleImageParameters( this->m_DerivativesArray[threadId][l], this->m_Sample[x_i],
                                         weightMoving-weightJoint, l, threadId);
    }
    
    
  }
}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
void RegisterToMeanMultiImageMetric < TFixedImage >
::AfterGetThreadedValueAndDerivative(MeasureType & value,
                           DerivativeType & derivative) const
{

  value = NumericTraits< RealType >::Zero;

  derivative.set_size(this->numberOfParameters * this->m_NumberOfImages);
  derivative.Fill (0.0);

  // Sum over the values returned by threads
  for( unsigned int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      for(int k=0; k<this->numberOfParameters; k++)
      {
        derivative[j * this->numberOfParameters + k] += this->m_DerivativesArray[i][j][k];
      }
    }
  }
  value /= 1.0 * (double) this->m_NumberOfSpatialSamples * (double) this->m_NumberOfImages;
  derivative /= (double) (-1.0*this->m_NumberOfSpatialSamples * this->m_NumberOfImages *
                          this->m_ImageStandardDeviation * this->m_ImageStandardDeviation);

  //Set the mean to zero
  //Remove mean
  DerivativeType sum (this->numberOfParameters);
  sum.Fill(0.0);
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    for (int j = 0; j < this->numberOfParameters; j++)
    {
      sum[j] += derivative[i * this->numberOfParameters + j];
    }
  }

  
  for (int i = 0; i < this->m_NumberOfImages * this->numberOfParameters; i++)
  {
    derivative[i] -= sum[i % this->numberOfParameters] / (double) this->m_NumberOfImages;
  }
  
}

// Callback routine used by the threading library. This routine just calls
// the GetThreadedValue() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
RegisterToMeanMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValueAndDerivative( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValueAndDerivative( threadId );


  return ITK_THREAD_RETURN_VALUE;
}

}        // end namespace itk


#endif
