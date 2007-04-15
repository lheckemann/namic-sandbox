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
#ifndef _JointEntropyKNNMultiImageMetric_cxx
#define _JointEntropyKNNMultiImageMetric_cxx

#include "JointEntropyKNNMultiImageMetric.h"


#include "itkCovariantVector.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "vnl/vnl_math.h"
#include "itkGaussianKernelFunction.h"
#include <cmath>

#include <ANN/ANN.h>             // ANN declarations

namespace itk
{

/*
 * Constructor
 */
template < class TFixedImage >
JointEntropyKNNMultiImageMetric < TFixedImage >::
JointEntropyKNNMultiImageMetric()
{
  m_NumberOfNearestNeigbors = 100;
  m_ErrorBound = 0.1;
}

/*
 * Destructor
 */
template < class TFixedImage >
JointEntropyKNNMultiImageMetric < TFixedImage >::
~JointEntropyKNNMultiImageMetric()
{
}


/*
 * Initialize
 */
template <class TFixedImage>
void
JointEntropyKNNMultiImageMetric<TFixedImage>
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();

  // Number of Spatial Samples must be an even number
  // First half is the set A and second half is the set B
  this->m_NumberOfSpatialSamples -= this->m_NumberOfSpatialSamples%2;

  // Resize w_x_j
  W_x_j.resize(this->m_NumberOfSpatialSamples);

  // Resize derivative container
  const unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();
  m_DerivativeArray.resize(this->m_NumberOfImages);
  for(int l=0; l<this->m_NumberOfImages; l++)
  {
    m_DerivativeArray[l].resize(this->m_NumberOfSpatialSamples);
    for(int i=0; i<this->m_NumberOfSpatialSamples; i++)
    {
      m_DerivativeArray[l][i].set_size(numberOfParameters);
    }
  }

  // Initialize KNN parameters
  dists.resize(this->m_NumberOfSpatialSamples/2);               // near neighbor distances
  nnIdx.resize(this->m_NumberOfSpatialSamples/2);
  for(int i=0; i<this->m_NumberOfSpatialSamples/2; i++)
  {
    nnIdx[i] = new ANNidx[m_NumberOfNearestNeigbors];                 // allocate near neigh indices
    dists[i] = new ANNdist[m_NumberOfNearestNeigbors];                // allocate near neighbor dists
  }

  // getvalueandderivativehelper
  weigtsArray.resize(this->m_NumberOfSpatialSamples);
  for(int i=0; i<this->m_NumberOfSpatialSamples/2; i++)
  {
    weigtsArray[i].set_size(this->m_NumberOfImages);
  }


}

/*
 * Get the match Measure
 */
template < class TFixedImage >
typename JointEntropyKNNMultiImageMetric < TFixedImage >::MeasureType
JointEntropyKNNMultiImageMetric <TFixedImage >::
GetValue(const ParametersType & parameters) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValue(parameters);
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;

  // Prepare the variables for getvalue
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallbackGetValueHelper, &str);
  this->GetMultiThreader()->SingleMethodExecute();
  
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
JointEntropyKNNMultiImageMetric< TFixedImage >
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
JointEntropyKNNMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue(const ParametersType & parameters) const
{

  Superclass::BeforeGetThreadedValue(parameters);

  const int N = this->m_NumberOfImages;
  const unsigned int sampleASize = this->m_NumberOfSpatialSamples / 2;
  const unsigned int sampleBSize = this->m_NumberOfSpatialSamples / 2;

  ANNpoint          queryPt;          // query point
  ANNkd_tree*       kdTree;              // search structure
  ANNpointArray     dataPts;          // data points

  // Initialize KNN parameters
  queryPt = annAllocPt(N);             // allocate query point
  dataPts = annAllocPts(this->m_NumberOfSpatialSamples/2, this->m_NumberOfImages);       // allocate data points
  
  // Update knn data values
  for(int x_i=0; x_i< sampleASize; x_i++ )
  {
    for(int j=0; j<N; j++)
    {
      dataPts[x_i][j] = this->m_Sample[x_i].imageValueArray[j];
    }
  }

  // Generate kd-tree
  kdTree = new ANNkd_tree(               // build search structure
               dataPts,             // the data points
               this->m_NumberOfSpatialSamples/2,                // number of points
               this->m_NumberOfImages);                // dimension of space

  for(int x_i=sampleBSize;x_i<this->m_NumberOfSpatialSamples; x_i++)
  {
    // Search the nearest k points in A that are closest to x_i
    for(int i=0; i<N; i++)
    {
      queryPt[i] = this->m_Sample[x_i].imageValueArray[i];
    }

    kdTree->annkSearch(                 // search
            queryPt,                // query point
            m_NumberOfNearestNeigbors,                      // number of near neighbors
            nnIdx[x_i-sampleBSize],                     // nearest neighbors (returned)
            dists[x_i-sampleBSize],                     // distance (returned)
            m_ErrorBound);                   // error bound


  }


  annDeallocPt(queryPt);
  annDeallocPts(dataPts);
  delete kdTree;
  annClose();
  
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void
JointEntropyKNNMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  const int N = this->m_NumberOfImages;
  const unsigned int size = this->m_NumberOfSpatialSamples;
  const unsigned int sampleASize = this->m_NumberOfSpatialSamples / 2;
  const unsigned int sampleBSize = this->m_NumberOfSpatialSamples / 2;
  /** The tranform parameters vector holding i'th images parameters
  Copy parameters in to a collection of arrays */

  const unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();


  //Calculate joint entropy
  this->m_value[threadId] = 0.0;
  // Loop over first sample set
  for (int x_i=sampleASize+threadId; x_i<sampleASize+sampleBSize; x_i += this->m_NumberOfThreads )
  {

    double probSum = 0.0;
    for( int j=0; j < m_NumberOfNearestNeigbors; j++)
    {
      // Evaluate the probability of d(x_a, x_b)
      const double diff =
          (this->m_Sample[x_i].imageValueArray - this->m_Sample[nnIdx[x_i-sampleASize][j]].imageValueArray).two_norm();
      probSum += this->m_KernelFunction->Evaluate(  diff / this->m_ImageStandardDeviation );
    }
    probSum /= (double)m_NumberOfNearestNeigbors;
    
    this->m_value[threadId] += vcl_log( probSum );

  }  // End of sample loop

}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
typename JointEntropyKNNMultiImageMetric < TFixedImage >::MeasureType
JointEntropyKNNMultiImageMetric < TFixedImage >
::AfterGetThreadedValue() const
{

  MeasureType value = NumericTraits< RealType >::Zero;

  // Sum over the values returned by threads
  for( unsigned int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
  }
  value /= (double)( -0.5 * this->m_NumberOfSpatialSamples );

  return value;
}

/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void
JointEntropyKNNMultiImageMetric < TFixedImage >
::BeforeGetThreadedValueAndDerivative(const ParametersType & parameters) const
{
   Superclass::BeforeGetThreadedValueAndDerivative(parameters);


  // Find KNN neigbors
  const int N = this->m_NumberOfImages;
  const unsigned int sampleASize = this->m_NumberOfSpatialSamples / 2;
  const unsigned int sampleBSize = this->m_NumberOfSpatialSamples / 2;

  ANNpoint          queryPt;          // query point
  ANNkd_tree*       kdTree;              // search structure
  ANNpointArray     dataPts;          // data points

  // Initialize KNN parameters
  queryPt = annAllocPt(N);             // allocate query point
  dataPts = annAllocPts(this->m_NumberOfSpatialSamples/2, this->m_NumberOfImages);       // allocate data points
  
  // Update knn data values
  for(int x_i=0; x_i< sampleASize; x_i++ )
  {
    for(int j=0; j<N; j++)
    {
      dataPts[x_i][j] = this->m_Sample[x_i].imageValueArray[j];
    }
  }

  // Generate kd-tree
  kdTree = new ANNkd_tree(               // build search structure
               dataPts,             // the data points
               this->m_NumberOfSpatialSamples/2,                // number of points
               this->m_NumberOfImages);                // dimension of space

  for(int x_i=sampleBSize;x_i<this->m_NumberOfSpatialSamples; x_i++)
  {
    // Search the nearest k points in A that are closest to x_i
    for(int i=0; i<N; i++)
    {
      queryPt[i] = this->m_Sample[x_i].imageValueArray[i];
    }

    kdTree->annkSearch(                 // search
            queryPt,                // query point
            m_NumberOfNearestNeigbors,                      // number of near neighbors
            nnIdx[x_i-sampleBSize],                     // nearest neighbors (returned)
            dists[x_i-sampleBSize],                     // distance (returned)
            m_ErrorBound);                   // error bound


  }


  annDeallocPt(queryPt);
  annDeallocPts(dataPts);
  delete kdTree;
  annClose();
  
}

template < class TFixedImage >
void JointEntropyKNNMultiImageMetric < TFixedImage >
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

  // Compute the derivatives
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallbackGetValueAndDerivativeHelper, &str);
  
  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  
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
JointEntropyKNNMultiImageMetric < TFixedImage >
::GetThreadedValueAndDerivative(int threadId) const
{

  double N = (double) this->m_NumberOfImages;
  int numberOfThreads = this->GetNumberOfThreads();
  const unsigned int sampleASize = this->m_NumberOfSpatialSamples / 2;
  const unsigned int sampleBSize = this->m_NumberOfSpatialSamples / 2;

  
  /** The tranform parameters vector holding i'th images parameters
  Copy parameters in to a collection of arrays */
  MeasureType value = NumericTraits < MeasureType >::Zero;

  const unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();

  //Initialize the derivative to zero
  this->m_derivativeArray[threadId].Fill(0.0);
  this->m_value[threadId] = 0.0;
  DerivativeType deriv(numberOfParameters);
  Array<double> weight(this->m_NumberOfImages);

  std::vector<int> counts(sampleASize);
  for(int i=0; i<sampleASize; i++)
  {
    counts[i] = 0;
  }
  // Calculate the derivative
  for (int x_i=sampleASize+threadId; x_i<sampleASize+sampleBSize; x_i+= this->m_NumberOfThreads )
  {
    
    // Compute W(x_j)
    W_x_j[x_i] = 0.0;
    // Sum over the second sample set
    weight.Fill(0.0);
    for (int j=0; j<m_NumberOfNearestNeigbors; j++ )
    {

      const unsigned int x_j = nnIdx[x_i-sampleASize][j];
      counts[x_j]++;
      
      // Compute d(x_i, x_j)
      const double diff =
                   (this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray).two_norm();

      // Compute G(d(x_i,x_j))
      const double G = this->m_KernelFunction->Evaluate( diff / this->m_ImageStandardDeviation );

      W_x_j[x_i] += G;
      weight += G* (this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray);
    }

    //Calculate joint entropy
    this->m_value[threadId] += vcl_log( W_x_j[x_i] / (double)m_NumberOfNearestNeigbors );
    weight /= W_x_j[x_i];

    for( int l=0; l<this->m_NumberOfImages; l++ )
    {

      //Copy the proper part of the derivative
      for (int n = 0; n < numberOfParameters; n++)
      {
        this->m_derivativeArray[threadId][l * numberOfParameters + n] += weight[l]*m_DerivativeArray[l][x_i][n];
      }
    }  // End of sample loop B
  }


  // Second term
  for (int j=0; j<m_NumberOfNearestNeigbors; j++ )
  {

    // Sum over the second sample set
    for (int x_i=sampleASize+threadId; x_i<sampleASize+sampleBSize; x_i+= this->m_NumberOfThreads )
    {
      
      const unsigned int x_j = nnIdx[x_i-sampleASize][j];

      // Compute d(x_i, x_j)
      const double diff =
          (this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray).two_norm();

      // Compute G(d(x_i,x_j))
      const double G = this->m_KernelFunction->Evaluate( diff / this->m_ImageStandardDeviation );

      weigtsArray[x_j] += G / W_x_j[x_i] *
               (this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray);
    }
  }

  for(int x_j=0; x_j<sampleASize; x_j++)
  {
    for( int l=0; l<this->m_NumberOfImages; l++ )
    {
      //Copy the proper part of the derivative
      for (int n = 0; n < numberOfParameters; n++)
      {
        this->m_derivativeArray[threadId][l * numberOfParameters + n] -= weigtsArray[x_j][l]*m_DerivativeArray[l][x_j][n] * (double)(counts[x_j]/m_NumberOfNearestNeigbors);
      }
    }  // End of sample loop B
  }

  this->m_value[threadId] /= -1.0 * (double) sampleBSize;
}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
void JointEntropyKNNMultiImageMetric < TFixedImage >
::AfterGetThreadedValueAndDerivative(MeasureType & value,
                           DerivativeType & derivative) const
{


  const int numberOfParameters = this->m_TransformArray[0]->GetNumberOfParameters();

  // Sum over the values returned by threads
  value = this->m_value[0];
  derivative = this->m_derivativeArray[0];
  for( unsigned int i=1; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
    derivative += this->m_derivativeArray[i];
  }
  derivative /= (double) (this->m_NumberOfSpatialSamples / 2.0 * this->m_ImageStandardDeviation * this->m_ImageStandardDeviation);

  //Set the mean to zero
  //Remove mean
  DerivativeType temp(numberOfParameters);
  temp.Fill(0.0);
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    for (int j = 0; j < numberOfParameters; j++)
    {
      temp[j] += derivative[i * numberOfParameters + j];
    }
  }

  
  for (int i = 0; i < this->m_NumberOfImages * numberOfParameters; i++)
  {
    derivative[i] -= temp[i % numberOfParameters] / (double) this->m_NumberOfImages;
  }
  
} 

// Callback routine used by the threading library. This routine just calls
// the GetThreadedValue() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyKNNMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValueAndDerivative( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValueAndDerivative( threadId );


  return ITK_THREAD_RETURN_VALUE;
}

// Callback routine used by the threading library. This routine just calls
// the ComputeDerivative() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyKNNMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValueAndDerivativeHelper( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetValueAndDerivativeHelper( threadId );


  return ITK_THREAD_RETURN_VALUE;
}

/*
 * Compute the derivatives before getthreadedvalueandDerivative
 */
template < class TFixedImage >
void
JointEntropyKNNMultiImageMetric < TFixedImage >
::GetValueAndDerivativeHelper(int threadId) const
{

  
  for( int l=0; l<this->m_NumberOfImages; l++ )
  {
    // Set the image for derivatives
    this->m_DerivativeCalcVector[threadId]->SetInputImage(this->m_ImageArray[l]);

    for (unsigned int i=threadId; i<this->m_NumberOfSpatialSamples; i+= this->m_NumberOfThreads )
    {
      // Compute the gradient
      this->CalculateDerivatives(this->m_Sample[i].FixedImagePoint, m_DerivativeArray[l][i], l, threadId);
    }

  }
}


// Callback routine used by the threading library. This routine just calls
// the ComputeDerivative() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyKNNMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValueHelper( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetValueHelper( threadId );


  return ITK_THREAD_RETURN_VALUE;
}
/*
 * Update the intensities before getthreadedvalue
 */
template < class TFixedImage >
void
JointEntropyKNNMultiImageMetric < TFixedImage >
::GetValueHelper(int threadId) const
{

  // Update intensity values
  const int N = this->m_NumberOfImages;
  const unsigned int size = this->m_NumberOfSpatialSamples;
  for(int i=threadId; i< size; i+=this->m_NumberOfThreads )
  {
    for(int j=0; j<N; j++)
    {
      const MovingImagePointType mappedPoint = this->m_TransformArray[j]->TransformPoint(this->m_Sample[i].FixedImagePoint);
      if(this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint) )
      {
        this->m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(mappedPoint);
      }
    }

    weigtsArray[i].Fill(0.0);
  }
  

  
}

}        // end namespace itk


#endif
