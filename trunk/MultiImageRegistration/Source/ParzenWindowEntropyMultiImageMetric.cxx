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
#ifndef _ParzenWindowEntropyMultiImageMetric_cxx
#define _ParzenWindowEntropyMultiImageMetric_cxx

#include "ParzenWindowEntropyMultiImageMetric.h"


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
ParzenWindowEntropyMultiImageMetric < TFixedImage >::
    ParzenWindowEntropyMultiImageMetric()
{

  m_ImageStandardDeviation = 0.4;

  // Following initialization is related to
  // calculating image derivatives
  this->SetComputeGradient (false);  // don't use the default gradient for now

  this->m_BSplineTransformArray.resize(0);
  

  m_UseMask = false;
  m_NumberOfFixedImages = 0;
  m_NumberOfParametersPerdimension = 0;

}

/*
 * Constructor
 */
template < class TFixedImage >
ParzenWindowEntropyMultiImageMetric < TFixedImage >::
~ParzenWindowEntropyMultiImageMetric()
{
  


}



/*
 * Initialize
 */
template <class TFixedImage> 
void
ParzenWindowEntropyMultiImageMetric<TFixedImage>
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();

  // Set the kernel function
  m_KernelFunction.resize(this->m_NumberOfThreads);
  for(unsigned int i=0; i<this->m_NumberOfThreads; i++)
  {
    m_KernelFunction[i] =
        dynamic_cast < KernelFunction * >(GaussianKernelFunction::New().GetPointer ());
  }

  if (this->m_NumberOfSpatialSamples == 0)
  {
    this->SetNumberOfSpatialSamples(50);
  }

  //check whether there is a mask
  for (unsigned int j = 0; j < this->m_NumberOfImages; j++)
  {
    if ( this->m_ImageMaskArray[j] )
    {
      m_UseMask = true;
    }
  }



  m_TransformParametersArray.resize(this->m_NumberOfImages);
  for(unsigned int i=0; i<this->m_NumberOfImages; i++)
  {
    m_TransformParametersArray[i].set_size(this->numberOfParameters);
  }

  m_value.SetSize( this->m_NumberOfThreads );

  // Each thread has its own derivative pointer
  m_DerivativesArray.resize(this->m_NumberOfThreads);
  for(int i=0; i<this->m_NumberOfThreads; i++)
  {
    m_DerivativesArray[i].resize(this->m_NumberOfImages);
    for(unsigned int j=0; j<this->m_NumberOfImages; j++)
    {
      m_DerivativesArray[i][j].SetSize(this->numberOfParameters);
    }
  }

  // Each image has its own derivative calculator
  m_DerivativeCalculator.resize(this->m_NumberOfImages);
  for(unsigned int i=0; i<this->m_NumberOfImages; i++)
  {
    m_DerivativeCalculator[i] = DerivativeFunctionType::New ();
    m_DerivativeCalculator[i]->SetInputImage(this->m_ImageArray[i]);
  }

  // Sample the image domain
  randIter = new NonRepeatingRandomIterator(this->m_ImageArray[0], this->GetFixedImageRegion());

  randIter->SetNumberOfSamples(this->m_NumberOfSpatialSamples);
  randIter->ReinitializeSeed();
  randIter->GoToBegin();
  
  this->SampleFixedImageDomain(this->m_Sample);
  
  // Initialize the variables for regularization term
  if( this->m_UserBsplineDefined && this->m_Regularization &&
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
  else
  {
    this->m_Regularization = false;
  }

  // Initialize the variables for regularization term
  if( this->m_Regularization &&
      strcmp(this->m_TransformArray[0]->GetNameOfClass(), "UserBSplineDeformableTransform") )
  {
    itkExceptionMacro(<<"Cannot use regularization with transforms" <<
        " other than BSplineDeformableTransform" );
  }

  // If using regularization check that the Bspline Transform is supplied
  if( this->m_Regularization &&
      !strcmp(this->m_TransformArray[0]->GetNameOfClass(), "BSplineDeformableTransform") )
  {
    for(unsigned int i=0; i<this->m_NumberOfImages;i++)
    {
      if( !this->m_BSplineTransformArray[i] )
      {
        itkExceptionMacro(<<"Bspline Transform Array not initialized" );
      }
      
      if( (void *) this->m_BSplineTransformArray[i] != (void*)this->m_TransformArray[i])
      {
        itkExceptionMacro(<<"While using Bspline regularization transform array and Bspline array should have the pointers to the same transform" );
      }
    }

  }
  
}

/*
 * Finalize
 */
template <class TFixedImage> 
void
ParzenWindowEntropyMultiImageMetric<TFixedImage>
::Finalize(void)
{

  // deallocate randomiterator
  delete randIter;

 //Finalize superclass
  Superclass::Finalize();
}

template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >::
PrintSelf (std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf (os, indent);
  os << indent << "NumberOfSpatialSamples: ";
  os << this->m_NumberOfSpatialSamples << std::endl;
  os << indent << "FixedImageStandardDeviation: ";
  os << m_ImageStandardDeviation << std::endl;
  os << indent << "MovingImageStandardDeviation: ";
}


/*
 * Uniformly sample the fixed image domain. Each sample consists of:
 *  - the sampled point
 *  - Corresponding moving image intensity values
 */
template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >::
SampleFixedImageDomain (SpatialSampleContainer & samples) const
{

  bool allOutside = true;

  // Number of random picks made from the portion of fixed image within the fixed mask
  unsigned long numberOfFixedImagePixelsVisited = 0;
  unsigned long dryRunTolerance = 3*this->GetFixedImageRegion().GetNumberOfPixels();

  std::vector<MovingImagePointType>   mappedPointsArray(this->m_NumberOfImages);
  for(unsigned long int i=0; i<this->m_NumberOfSpatialSamples; i++)
  {
    // Get sampled index
    FixedImageIndexType index = randIter->GetIndex();
    // Translate index to point
    this->m_ImageArray[0]->TransformIndexToPhysicalPoint(index, this->m_Sample[i].FixedImagePoint);

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
    for(unsigned int j = 0; j < this->m_NumberOfImages && allPointsInside; j++)
    {
      mappedPointsArray[j] = this->m_TransformArray[j]->TransformPoint (this->m_Sample[i].FixedImagePoint);

      //check whether sampled point is in one of the masks
      if ( this->m_ImageMaskArray[j] && !this->m_ImageMaskArray[j]
           ->IsInside (mappedPointsArray[j])  )
      {
        pointInsideMask = true;
      }

      allPointsInside = allPointsInside && this->m_InterpolatorArray[j]
          ->IsInsideBuffer (mappedPointsArray[j]);
    }

    // If not all points are inside continue to the next random sample
    if(allPointsInside == false || (m_UseMask && pointInsideMask == false) )
    {
      ++(*randIter);
      i--;
      continue;
    }

    // write the mapped samples intensity values inside an array
    for(unsigned int j = 0; j < this->m_NumberOfImages; j++)
    {
      this->m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(mappedPointsArray[j]);
      allOutside = false;
    }
    // Jump to random position
    ++(*randIter);

  }

  if(allOutside)
  {
    // if all the samples mapped to the outside throw an exception
    itkExceptionMacro(<<"All the sampled point mapped to outside of the moving image");
  }

}


/*
 * Get the match Measure
 */
template < class TFixedImage >
typename ParzenWindowEntropyMultiImageMetric < TFixedImage >::MeasureType
ParzenWindowEntropyMultiImageMetric <TFixedImage >::
GetValue(const ParametersType & parameters) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValue(parameters);
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;


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
ParzenWindowEntropyMultiImageMetric< TFixedImage >
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
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue(const ParametersType & parameters) const
{
  //Make sure that each transform parameters are updated
  // Loop over images
  for( unsigned int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (unsigned int j = 0; j < this->numberOfParameters; j++)
    {
      m_TransformParametersArray[i][j] = parameters[this->numberOfParameters * i + j];
    }
    this->m_TransformArray[i]->SetParameters(m_TransformParametersArray[i]);
  }

}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void 
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  // Update intensity values
  MovingImagePointType mappedPoint;
  for(unsigned long int i=threadId; i< this->m_NumberOfSpatialSamples; i += this->m_NumberOfThreads )
  {
    for(unsigned int j=0; j<this->m_NumberOfImages; j++)
    {
      mappedPoint = this->m_TransformArray[j]->TransformPoint(this->m_Sample[i].FixedImagePoint);
      if(this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint) )
      {
        this->m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(mappedPoint);
      }
    }
  } 

  //Calculate variance and mean
  m_value[threadId] = 0.0;
  double dSum;

  // Loop over the pixel stacks
  for(unsigned long int a=threadId; a<this->m_NumberOfSpatialSamples; a += this->m_NumberOfThreads )
  {

    for(unsigned int j = 0; j < this->m_NumberOfImages; j++)
    {
      dSum = 0.0;
      
      for(unsigned int k = 0; k < this->m_NumberOfImages; k++)
      {

        dSum += m_KernelFunction[threadId]->Evaluate( ( this->m_Sample[a].imageValueArray[j] - this->m_Sample[a].imageValueArray[k] )
                                                /m_ImageStandardDeviation );
      }
      dSum /= static_cast<double> (this->m_NumberOfImages );
      m_value[threadId]  += vcl_log( dSum );
      
    }

  }  // End of sample loop

  
}


/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
typename ParzenWindowEntropyMultiImageMetric < TFixedImage >::MeasureType
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::AfterGetThreadedValue() const
{

  MeasureType value = NumericTraits< RealType >::Zero;

  // Sum over the values returned by threads
  for( int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += m_value[i];
  }
  value /= (MeasureType) (-1.0 * this->m_NumberOfSpatialSamples*this->m_NumberOfImages);

  return value;
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >
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
ParzenWindowEntropyMultiImageMetric< TFixedImage >
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
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::BeforeGetThreadedValueAndDerivative(const ParametersType & parameters) const
{
  // cout << "checking derivative" << endl;
  // collect sample set
  this->SampleFixedImageDomain (this->m_Sample);

  // Loop over images
  for (unsigned int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (unsigned int j = 0; j < this->numberOfParameters; j++)
    {
      m_TransformParametersArray[i][j] = parameters[this->numberOfParameters * i + j];
    }
    // cout << currentParam << endl;
    this->m_TransformArray[i]->SetParameters(m_TransformParametersArray[i]);
  }

}


/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >
::AfterGetThreadedValueAndDerivative(MeasureType & value,
                           DerivativeType & derivative) const
{

  value = NumericTraits< RealType >::Zero;

  derivative.set_size(this->numberOfParameters * this->m_NumberOfImages);
  derivative.Fill (0.0);

  // Sum over the values returned by threads
  for( int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += m_value[i];
    for(unsigned int j=0; j<this->m_NumberOfImages; j++)
    {
      for(unsigned int k=0; k<this->numberOfParameters; k++)
      {
        derivative[j * this->numberOfParameters + k] += m_DerivativesArray[i][j][k]; 
      }
    }
  }
  value /= (double) ( -1.0 * this->m_NumberOfSpatialSamples * this->m_NumberOfImages );
  derivative /= (double) (this->m_NumberOfSpatialSamples * this->m_NumberOfImages *
                     m_ImageStandardDeviation * m_ImageStandardDeviation );
  
  //Set the mean to zero
  //Remove mean
  DerivativeType sum (this->numberOfParameters);
  sum.Fill(0.0);
  for (unsigned int i = 0; i < this->m_NumberOfImages; i++)
  {
    for (unsigned int j = 0; j < this->numberOfParameters; j++)
    {
      sum[j] += derivative[i * this->numberOfParameters + j];
    }
  }

  
  for (unsigned int i = 0; i < this->m_NumberOfImages * this->numberOfParameters; i++)
  {
    derivative[i] -= sum[i % this->numberOfParameters] / (double) this->m_NumberOfImages;
  }


}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void 
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::GetThreadedValueAndDerivative(int threadId) const
{
  

  //Initialize the derivative array to zero
  for(unsigned int i=0; i<this->m_NumberOfImages;i++)
  {
    m_DerivativesArray[threadId][i].Fill(0.0);
  }


  //Calculate variance and mean
  m_value[threadId] = 0.0;
  std::vector<double> W_j(this->m_NumberOfImages);

  // Loop over the pixel stacks
  for(unsigned long int x=threadId; x<this->m_NumberOfSpatialSamples; x += this->m_NumberOfThreads )
  {
    // Calculate the entropy
    for(unsigned int j = 0; j < this->m_NumberOfImages; j++)
    {
      W_j[j] = 0.0;
      
      for(unsigned int k = 0; k < this->m_NumberOfImages; k++)
      {

        W_j[j] += m_KernelFunction[threadId]->Evaluate( ( this->m_Sample[x].imageValueArray[j] - this->m_Sample[x].imageValueArray[k] ) /
            m_ImageStandardDeviation );

      }
      m_value[threadId]  += vcl_log( W_j[j] / (double) (this->m_NumberOfImages ) );
      
    }


    // Calculate derivative
    for(unsigned int l = 0; l < this->m_NumberOfImages; l++)
    {

      double weight = 0.0;
      for(unsigned int j=0; j<this->m_NumberOfImages; j++)
      {

        const double diff = (this->m_Sample[x].imageValueArray[l] - this->m_Sample[x].imageValueArray[j] );
        const double g = m_KernelFunction[threadId]->Evaluate( diff / m_ImageStandardDeviation );

        weight += (1.0/W_j[l] + 1.0/W_j[j]) * g * diff;
      }
      
      // Get the derivative for this sample
      UpdateSingleImageParameters( m_DerivativesArray[threadId][l], this->m_Sample[x], weight, l, threadId);
    }
  }  // End of sample loop


}




/*
 * Get the match measure derivative
 */
template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >
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
void ParzenWindowEntropyMultiImageMetric < TFixedImage >::
UpdateSingleImageParameters( DerivativeType & inputDerivative, const SpatialSample& sample, const RealType& weight, const int& imageNumber, const int& threadID) const
{

  const CovarientType gradient = m_DerivativeCalculator[imageNumber]->Evaluate(
                                                this->m_TransformArray[imageNumber]->TransformPoint (sample.FixedImagePoint) );
  //typedef FixedArray < double, MovingImageDimension > FixedArrayType;
  //const FixedArrayType gradient = this->m_GradientInterpolatorArray[imageNumber]->Evaluate(sample.mappedPointsArray[imageNumber]);

  typedef typename TransformType::JacobianType JacobianType;


  if(this->m_UserBsplineDefined == false )
  {
    const JacobianType & jacobian =
        this->m_TransformArray[imageNumber]->GetJacobian( sample.FixedImagePoint);

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
    // Get nonzero indexes
    typedef itk::Array<RealType> WeigtsType;
    WeigtsType bsplineWeights(numberOfWeights);
    this->m_BSplineTransformArray[imageNumber]->GetJacobian(sample.FixedImagePoint, bsplineIndexes, bsplineWeights);

    for (int k = 0; k < numberOfWeights; k++)
    {

        for (unsigned int j = 0; j < MovingImageDimension; j++)
        {

          inputDerivative[j*m_NumberOfParametersPerdimension + bsplineIndexes[k]] += bsplineWeights[k] * gradient[j] * weight;
        }

    }

  }

}



}        // end namespace itk


#endif
