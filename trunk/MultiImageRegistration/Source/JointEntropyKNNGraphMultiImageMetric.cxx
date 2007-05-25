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
#ifndef _JointEntropyKNNGraphMultiImageMetric_cxx
#define _JointEntropyKNNGraphMultiImageMetric_cxx

#include "JointEntropyKNNGraphMultiImageMetric.h"


#include "itkCovariantVector.h"

#include "vnl/vnl_math.h"
#include "itkGaussianKernelFunction.h"
#include <cmath>

namespace itk
{

/*
 * Constructor
 */
template < class TFixedImage >
JointEntropyKNNGraphMultiImageMetric < TFixedImage >::
    JointEntropyKNNGraphMultiImageMetric()
{



  m_MinProbability = 1e-10;
  
  // Following initialization is related to
  // calculating image derivatives
  this->SetComputeGradient (false);  // don't use the default gradient for now

  this->m_BSplineTransformArray.resize(0);
  m_Regularization = false;
  m_RegularizationFactor = 1e-5;
  m_NumberOfParametersPerdimension = 0;

  m_UseMask = false;
  m_NumberOfFixedImages = 0;


}

/*
 * Constructor
 */
template < class TFixedImage >
JointEntropyKNNGraphMultiImageMetric < TFixedImage >::
~JointEntropyKNNGraphMultiImageMetric()
{
  


}



/*
 * Initialize
 */
template <class TFixedImage> 
void
JointEntropyKNNGraphMultiImageMetric<TFixedImage>
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();
  // We wont use the default sample so deallocate it
  Superclass::Finalize();

  if (this->m_NumberOfSpatialSamples == 0)
  {
    this->SetNumberOfSpatialSamples(50);
  }

  //check whether there is a mask
  for (int j = 0; j < this->m_NumberOfImages; j++)
  {
    if ( this->m_ImageMaskArray[j] )
    {
      m_UseMask = true;
    }
  }


  m_TransformParametersArray.resize(this->m_NumberOfImages);
  for(int i=0; i<this->m_NumberOfImages; i++)
  {
    m_TransformParametersArray[i].set_size(this->numberOfParameters);
  }
  //Get number of threads
  m_NumberOfThreads = this->GetNumberOfThreads();

  m_value.SetSize( m_NumberOfThreads );

  // Each thread has its own derivative pointer
  m_DerivativesArray.resize(m_NumberOfThreads);
  for(int i=0; i<m_NumberOfThreads; i++)
  {
    m_DerivativesArray[i].resize(this->m_NumberOfImages);
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      m_DerivativesArray[i][j].SetSize(this->numberOfParameters);
    }
  }

  // Each image has its own derivative calculator
  m_DerivativeCalculator.resize(this->m_NumberOfImages);
  for(int i=0; i<this->m_NumberOfImages; i++)
  {
    // separate for each thread
    m_DerivativeCalculator[i].resize(m_NumberOfThreads);
    for(int j=0; j<m_NumberOfThreads; j++)
    {
      m_DerivativeCalculator[i][j] = DerivativeFunctionType::New ();
      m_DerivativeCalculator[i][j]->SetInputImage(this->m_ImageArray[i]);
    }
  }

  // Sample the image domain
  randIter = new NonRepeatingRandomIterator(this->m_ImageArray[0], this->GetFixedImageRegion());

  randIter->SetNumberOfSamples(this->m_NumberOfSpatialSamples);
  randIter->GoToBegin();
  
    // Initialize the variables for regularization term
  if( this->m_UserBsplineDefined && m_Regularization &&
      strcmp(this->m_TransformArray[0]->GetNameOfClass(), "UserBSplineDeformableTransform") )
  {
    itkExceptionMacro(<<"Cannot use regularization with transforms" <<
        " other than BSplineDeformableTransform" );
  }


  // Initialize the variables for regularization term
  if( this->m_UserBsplineDefined )
  {
    // Get nonzero indexes
    numberOfWeights = this->m_BSplineTransformArray[0]->GetNumberOfAffectedWeights();
    bsplineIndexes.set_size(numberOfWeights);
    m_NumberOfParametersPerdimension = this->m_BSplineTransformArray[0]->GetNumberOfParametersPerDimension();
  }


  //Prepare the gradient filters if Regularization is on
  if(m_Regularization)
  {
    
  }

  // initialize KNN graph
  dataPts = annAllocPts(this->m_NumberOfSpatialSamples, this->m_NumberOfImages);       // allocate data points
  dists = new ANNdistArray[this->m_NumberOfSpatialSamples];               // near neighbor distances
  nnIdx = new ANNidxArray[this->m_NumberOfSpatialSamples];
  for(int i=0; i<this->m_NumberOfSpatialSamples; i++)
  {
    nnIdx[i] = new ANNidx[m_NumberOfNearestNeigbors+1];                 // allocate near neigh indices
    dists[i] = new ANNdist[m_NumberOfNearestNeigbors+1];                // allocate near neighbor dists
  }

  m_FixedImagePointArray = new FixedImagePointType[this->m_NumberOfSpatialSamples];
  m_MappedPointsArray = new MovingImagePointType*[this->m_NumberOfSpatialSamples];
  for(int i=0;i<this->m_NumberOfSpatialSamples;i++)
  {
    m_MappedPointsArray[i] = new MovingImagePointType[this->m_NumberOfImages];
  }

  // Get samples for the first iteration
  GetSamples();

  
  // Generate kd-tree for the first run
  kdTree = new ANNkd_tree(               // build search structure
               dataPts,             // the data points
               this->m_NumberOfSpatialSamples,                // number of points
               this->m_NumberOfImages);                // dimension of space

  for(int i=0;i<this->m_NumberOfSpatialSamples; i++)
  {
    // Search the nearest k points in A that are closest to x_i
    kdTree->annkSearch(                 // search
            dataPts[i],                // query point
            m_NumberOfNearestNeigbors+1,      // number of near neighbors
            nnIdx[i],                     // nearest neighbors (returned)
            dists[i],                     // distance (returned)
            m_ErrorBound);                   // error bound


  }

  delete kdTree;
  
}

/*
 * Finalize
 */
template <class TFixedImage> 
void
JointEntropyKNNGraphMultiImageMetric<TFixedImage>
::Finalize(void)
{
  // delete tree
  annDeallocPts(dataPts);
  for(int i=0; i<this->m_NumberOfSpatialSamples; i++)
  {
    delete nnIdx[i];                 // allocate near neigh indices
    delete dists[i];                // allocate near neighbor dists
    delete[] m_MappedPointsArray[i];
  }
  delete[] dists;               // near neighbor distances
  delete[] nnIdx;

  delete[] m_MappedPointsArray;
  delete[] m_FixedImagePointArray;
  
  // deallocate randomiterator
  delete randIter;

 //Finalize superclass
  Superclass::Finalize();
}

template < class TFixedImage >
void JointEntropyKNNGraphMultiImageMetric < TFixedImage >::
PrintSelf (std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf (os, indent);
  os << indent << "NumberOfSpatialSamples: ";
  os << this->m_NumberOfSpatialSamples << std::endl;
  os << indent << "FixedImageStandardDeviation: ";
  os << m_ImageStandardDeviation << std::endl;
  os << indent << "MovingImageStandardDeviation: ";
  os << m_KernelFunction.GetPointer () << std::endl;
}


/*
 * Uniformly sample the fixed image domain. Each sample consists of:
 *  - the sampled point
 *  - Corresponding moving image intensity values
 */
template < class TFixedImage >
void JointEntropyKNNGraphMultiImageMetric < TFixedImage >::
GetSamples() const
{

  bool allOutside = true;

  // Number of random picks made from the portion of fixed image within the fixed mask
  unsigned long numberOfFixedImagePixelsVisited = 0;
  unsigned long dryRunTolerance = 3*this->GetFixedImageRegion().GetNumberOfPixels();

  FixedImageIndexType index;

  for (int i=0; i<this->m_NumberOfSpatialSamples; i++)
  {
    // Get sampled index
    index = randIter->GetIndex();
    // Translate index to point
    this->m_ImageArray[0]->TransformIndexToPhysicalPoint(index, m_FixedImagePointArray[i]);

    // Check the total number of sampled points
    ++numberOfFixedImagePixelsVisited;
    if (numberOfFixedImagePixelsVisited > dryRunTolerance)
    {
      // We randomly visited as many points as is the size of the fixed image
      // region.. Too may samples mapped outside.. go change your transform
      itkExceptionMacro(<<"Too many samples mapped outside the moving buffer");
    }


    //Check whether all points are inside mask
    bool allPointsInside = true;
    bool pointInsideMask = false;
    for (int j = 0; j < this->m_NumberOfImages && allPointsInside; j++)
    {
      m_MappedPointsArray[i][j] = this->m_TransformArray[j]->TransformPoint(m_FixedImagePointArray[i]);

      //check whether sampled point is in one of the masks
      if ( this->m_ImageMaskArray[j] && !this->m_ImageMaskArray[j]
           ->IsInside (m_MappedPointsArray[i][j])  )
      {
        pointInsideMask = true;
      }

      allPointsInside = allPointsInside && this->m_InterpolatorArray[j]
          ->IsInsideBuffer (m_MappedPointsArray[i][j]);
    }

    // If not all points are inside continue to the next random sample
    if (allPointsInside == false || (m_UseMask && pointInsideMask == false) )
    {
      ++(*randIter);
      i--;
      continue;
    }

    // write the mapped samples intensity values inside an array
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      dataPts[i][j] = this->m_InterpolatorArray[j]->Evaluate(m_MappedPointsArray[i][j]);
      allOutside = false;
    }
    // Jump to random position
    ++(*randIter);

  }

  if (allOutside)
  {
    // if all the samples mapped to the outside throw an exception
    itkExceptionMacro(<<"All the sampled point mapped to outside of the moving image");
  }

}


/*
 * Get the match Measure
 */
template < class TFixedImage >
typename JointEntropyKNNGraphMultiImageMetric < TFixedImage >::MeasureType
JointEntropyKNNGraphMultiImageMetric <TFixedImage >::
GetValue(const ParametersType & parameters) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValue(parameters);
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;


  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(ThreaderCallbackGetValue, &str);
  
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
JointEntropyKNNGraphMultiImageMetric< TFixedImage >
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
JointEntropyKNNGraphMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue(const ParametersType & parameters) const
{
  //Make sure that each transform parameters are updated
  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < this->numberOfParameters; j++)
    {
      m_TransformParametersArray[i][j] = parameters[this->numberOfParameters * i + j];
    }
    this->m_TransformArray[i]->SetParameters(m_TransformParametersArray[i]);
  }
  
  // Update intensity values
  MovingImagePointType mappedPoint;
  for(int i=0; i< this->m_NumberOfSpatialSamples; i++ )
  {
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      mappedPoint = this->m_TransformArray[j]->TransformPoint(m_FixedImagePointArray[i]);
      if(this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint) )
      {
        dataPts[i][j] = this->m_InterpolatorArray[j]->Evaluate(mappedPoint);
      }
    }
  }
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void 
JointEntropyKNNGraphMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  m_value[threadId] = 0.0;
  

  double dist;
  // Loop over samples
  for (int i=threadId; i<this->m_NumberOfSpatialSamples; i += m_NumberOfThreads )
  {

    dist = 1e-300;
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      const double diff = dataPts[i][j] - dataPts[nnIdx[i][m_NumberOfNearestNeigbors]][j];
      dist += diff*diff;
    }

    m_value[threadId] += sqrt(dist);
    
  }  // End of sample loop

}


/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
typename JointEntropyKNNGraphMultiImageMetric < TFixedImage >::MeasureType
JointEntropyKNNGraphMultiImageMetric < TFixedImage >
::AfterGetThreadedValue() const
{

  MeasureType value = NumericTraits< RealType >::Zero;

  // Sum over the values returned by threads
  for( unsigned int i=0; i < m_NumberOfThreads; i++ )
  {
    value += m_value[i];
  }

  //cout << value << endl;
  return value;
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void JointEntropyKNNGraphMultiImageMetric < TFixedImage >
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

// Callback routine used by the threading library. This routine just calls
// the GetThreadedValue() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyKNNGraphMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValueAndDerivative( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValueAndDerivative( threadId );


  return ITK_THREAD_RETURN_VALUE;
}


/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void 
JointEntropyKNNGraphMultiImageMetric < TFixedImage >
::BeforeGetThreadedValueAndDerivative(const ParametersType & parameters) const
{
  // cout << "checking derivative" << endl;
  // collect sample set
  GetSamples();

  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < this->numberOfParameters; j++)
    {
      m_TransformParametersArray[i][j] = parameters[this->numberOfParameters * i + j];
    }
    // cout << currentParam << endl;
    this->m_TransformArray[i]->SetParameters(m_TransformParametersArray[i]);
  }

  // Build the tree 
  // Generate kd-tree for the first run
  kdTree = new ANNkd_tree(               // build search structure
               dataPts,             // the data points
               this->m_NumberOfSpatialSamples,                // number of points
               this->m_NumberOfImages);                // dimension of space

  
  for(int i=0;i<this->m_NumberOfSpatialSamples; i++)
  {
    // Search the nearest k points in A that are closest to x_i
    kdTree->annkSearch(                 // search
            dataPts[i],                // query point
            m_NumberOfNearestNeigbors+1,      // number of near neighbors
            nnIdx[i],                     // nearest neighbors (returned)
            dists[i],                     // distance (returned)
            m_ErrorBound);                   // error bound


  }

  delete kdTree;
}


/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
void JointEntropyKNNGraphMultiImageMetric < TFixedImage >
::AfterGetThreadedValueAndDerivative(MeasureType & value,
                           DerivativeType & derivative) const
{

  value = NumericTraits< RealType >::Zero;

  derivative.set_size(this->numberOfParameters * this->m_NumberOfImages);
  derivative.Fill (0.0);

  // Sum over the values returned by threads
  for( unsigned int i=0; i < m_NumberOfThreads; i++ )
  {
    value += m_value[i] / (double) m_NumberOfThreads;
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      for(int k=0; k<this->numberOfParameters; k++)
      {
        derivative[j * this->numberOfParameters + k] += m_DerivativesArray[i][j][k];
      }
    }
  }

  
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


/*
 * Get the match Measure
 */
template < class TFixedImage >
void 
JointEntropyKNNGraphMultiImageMetric < TFixedImage >
::GetThreadedValueAndDerivative(int threadId) const
{

  MeasureType value = NumericTraits < MeasureType >::Zero;

  //Initialize the derivative array to zero
  for(int i=0; i<this->m_NumberOfImages;i++)
  {
    m_DerivativesArray[threadId][i].Fill(0.0);
  }


  //Calculate variance and mean
  m_value[threadId] = 0.0;

  // Loop over the pixel stacks
  double dist;
  for (int i=0; i<this->m_NumberOfSpatialSamples; i++ )
  {
    // Calculate the entropy
    dist = 1e-300;
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      const double diff = dataPts[i][j] - dataPts[nnIdx[i][m_NumberOfNearestNeigbors]][j];
      dist += diff*diff;
    }
    dist = sqrt(dist);
    
    m_value[threadId] += dist;


    // Calculate derivative
    for (int j = threadId; j < this->m_NumberOfImages; j+=this->m_NumberOfThreads)
    {
      // Get the derivative for this sample
      UpdateImageParameters( m_DerivativesArray[threadId][j], i,
                                   1.0*(dataPts[i][j] - dataPts[nnIdx[i][m_NumberOfNearestNeigbors]][j]) / dist
                                   , j, threadId);
      UpdateImageParameters( m_DerivativesArray[threadId][j], nnIdx[i][m_NumberOfNearestNeigbors],
                                   -1.0*(dataPts[i][j] - dataPts[nnIdx[i][m_NumberOfNearestNeigbors]][j]) / dist
                                   , j, threadId);
    }
  }  // End of sample loop


}




/*
 * Get the match measure derivative
 */
template < class TFixedImage >
void JointEntropyKNNGraphMultiImageMetric < TFixedImage >
::GetDerivative (const ParametersType & parameters,
          DerivativeType & derivative) const
{
  MeasureType value;
  // call the combined version
  this->GetValueAndDerivative(parameters, value, derivative);
}



/*
 * Calculate derivatives of the image intensity with respect
 * to the transform parmeters.
 *
 * This should really be done by the mapper.
 *
 * This is a temporary solution until this feature is implemented
 * in the mapper. This solution only works for any transform
 * that support GetJacobian()
 */
template < class TFixedImage >
void JointEntropyKNNGraphMultiImageMetric < TFixedImage >::
UpdateImageParameters( DerivativeType & inputDerivative, const int& index, const RealType& weight, const int& imageNumber, const int& threadID) const
{

  const CovarientType gradient = m_DerivativeCalculator[imageNumber][threadID]->Evaluate(m_MappedPointsArray[index][imageNumber]);
  //typedef FixedArray < double, MovingImageDimension > FixedArrayType;
  //const FixedArrayType gradient = this->m_GradientInterpolatorArray[imageNumber]->Evaluate(sample.mappedPointsArray[imageNumber]);

  typedef typename TransformType::JacobianType JacobianType;


  if(this->m_UserBsplineDefined == false)
  {
    const JacobianType & jacobian =
        this->m_TransformArray[imageNumber]->GetJacobian( m_FixedImagePointArray[index]);

    double currentValue;
    for (unsigned int k = 0; k < this->numberOfParameters; k++)
    {
      currentValue = 0.0;
      for (unsigned int j = 0; j < MovingImageDimension; j++)
      {
        currentValue += jacobian[j][k] * gradient[j];
      }
      inputDerivative[k] += currentValue*weight;
    }
  }
  else
  {
    // Get nonzero indexex

    typedef itk::Array<RealType> WeigtsType;
    const WeigtsType & jacobian =
                this->m_BSplineTransformArray[imageNumber]->GetJacobian(m_FixedImagePointArray[index], bsplineIndexes);

    for (unsigned int k = 0; k < numberOfWeights; k++)
    {

        for (unsigned int j = 0; j < MovingImageDimension; j++)
        {

          inputDerivative[j*m_NumberOfParametersPerdimension + bsplineIndexes[k]] += jacobian[k]
                                                                        * gradient[j] * weight;
        }

    }
  }

}


}        // end namespace itk


#endif
