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
#ifndef _CongealingMultiImageMetric_cxx
#define _CongealingMultiImageMetric_cxx

#include "VarianceMultiImageMetric.h"


#include "itkCovariantVector.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "vnl/vnl_math.h"
#include "itkGaussianKernelFunction.h"


namespace itk
{

/*
 * Constructor
 */
  template < class TFixedImage >
    VarianceMultiImageMetric < TFixedImage >::VarianceMultiImageMetric ()
  {

    m_NumberOfSpatialSamples = 0;
    this->SetNumberOfSpatialSamples (50);

    m_KernelFunction =
      dynamic_cast <
      KernelFunction * >(GaussianKernelFunction::New ().GetPointer ());

    m_FixedImageStandardDeviation = 0.4;
    m_MovingImageStandardDeviation = 0.4;
    //m_ImageStandardDeviationArray.resize(2);
    //for( int i =0; i<m_ImageStandardDeviationArray.size(); i++)
    //  m_ImageStandardDeviationArray[i] = 0.4;

    m_MinProbability = 0.0001;

    //
    // Following initialization is related to
    // calculating image derivatives
    this->SetComputeGradient (false);  // don't use the default gradient for now
    m_DerivativeCalculator = DerivativeFunctionType::New ();

  }


  template < class TFixedImage >
    void
    VarianceMultiImageMetric < TFixedImage >::PrintSelf (std::ostream & os,
               Indent indent) const
  {
    Superclass::PrintSelf (os, indent);
    os << indent << "NumberOfSpatialSamples: ";
    os << m_NumberOfSpatialSamples << std::endl;
    os << indent << "FixedImageStandardDeviation: ";
    os << m_FixedImageStandardDeviation << std::endl;
    os << indent << "MovingImageStandardDeviation: ";
    os << m_MovingImageStandardDeviation << std::endl;
//   for( int i=0; i<m_ImageStandardDeviationArray.size(); i++){
//     os << indent << "ImageStandardDeviation: " << i << " ";
//     os << m_ImageStandardDeviationArray[i] << std::endl;
//   }
    os << indent << "KernelFunction: ";
    os << m_KernelFunction.GetPointer () << std::endl;
  }


/*
 * Set the number of spatial samples
 */
  template < class TFixedImage >
    void
    VarianceMultiImageMetric <
    TFixedImage >::SetNumberOfSpatialSamples (unsigned int num)
  {
    if (num == m_NumberOfSpatialSamples)
      return;

    this->Modified ();

    // clamp to minimum of 1
    m_NumberOfSpatialSamples = ((num > 1) ? num : 1);

    // resize the storage vectors
    m_SampleA.resize (m_NumberOfSpatialSamples);
    m_SampleB.resize (m_NumberOfSpatialSamples);
    for (int i = 0; i < m_NumberOfSpatialSamples; i++)
      {
  m_SampleA[i].imageValueArray.resize (this->m_NumberOfImages);
  m_SampleB[i].imageValueArray.resize (this->m_NumberOfImages);
      }

  }


/*
 * Uniformly sample the fixed image domain. Each sample consists of:
 *  - the fixed image value
 *  - the corresponding moving image value
 */
  template < class TFixedImage >
    void
    VarianceMultiImageMetric <
    TFixedImage >::
    SampleFixedImageDomain (SpatialSampleContainer & samples) const
  {

    m_SampleA.resize (m_NumberOfSpatialSamples);
    for (int i = 0; i < m_NumberOfSpatialSamples; i++)
    m_SampleA[i].imageValueArray.resize (this->m_NumberOfImages);

    typedef ImageRandomConstIteratorWithIndex < FixedImageType > RandomIterator;
    //RandomIterator randIter( this->m_FixedImage, this->GetFixedImageRegion() );
    RandomIterator randIter (this->m_ImageArray[0],
           this->GetFixedImageRegion ());

    randIter.SetNumberOfSamples (m_NumberOfSpatialSamples);
    randIter.GoToBegin ();

    typename SpatialSampleContainer::iterator iter;
    typename SpatialSampleContainer::const_iterator end = samples.end ();

    bool allOutside = true;

    this->m_NumberOfPixelsCounted = 0;  // Number of pixels that map into the 
    // fixed and moving image mask, if specified
    // and the resampled fixed grid after 
    // transformation. 

    // Number of random picks made from the portion of fixed image within the fixed mask
    unsigned long numberOfFixedImagePixelsVisited = 0;
    unsigned long dryRunTolerance =
      this->GetFixedImageRegion ().GetNumberOfPixels ();



    for (iter = samples.begin (); iter != end; ++iter)
      {
  // Get sampled index
  FixedImageIndexType index = randIter.GetIndex ();
  // Get sampled fixed image value
    (*iter).imageValueArray[0] = randIter.Get ();
  // Translate index to point
//     this->m_FixedImage->TransformIndexToPhysicalPoint( index,
//                                                  (*iter).FixedImagePointValue );
    this->m_ImageArray[0]->TransformIndexToPhysicalPoint (index,
                (*iter).
                FixedImagePointValue);

  //cout << (*iter).imageValueArray.size() << " "<< samples.size() << endl;
  //cin >> allOutside;

  // If not inside the fixed mask, ignore the point
  if (this->m_FixedImageMask &&
      !this->m_FixedImageMask->IsInside ((*iter).FixedImagePointValue))
    {
      ++randIter;    // jump to another random position
      continue;
    }

  if (allOutside)
    {
      ++numberOfFixedImagePixelsVisited;
      if (numberOfFixedImagePixelsVisited > dryRunTolerance)
        {
    // We randomly visited as many points as is the size of the fixed image
    // region.. Too may samples mapped ouside.. go change your transform
    itkExceptionMacro
      (<<"Too many samples mapped outside the moving buffer");
        }
    }

//     MovingImagePointType mappedPoint = 
//       this->m_Transform->TransformPoint( (*iter).FixedImagePointValue );
  /** look here */
  for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      MovingImagePointType mappedPoint =
        this->m_TransformArray[j]->TransformPoint ((*iter).
               FixedImagePointValue);
      //cout << (*iter).imageValueArray[j] << endl;
      // If the transformed point after transformation does not lie within the 
      // MovingImageMask, skip it.
      if (this->m_MovingImageMask &&
    !this->m_MovingImageMask->IsInside (mappedPoint))
        {
    ++randIter;
    continue;
        }

      // The interpolator does not need to do bounds checking if we have masks, 
      // since we know that the point is within the fixed and moving masks. But
      // a crazy user can specify masks that are bigger than the image. Then we
      // will need bounds checking.. So keep this anyway.
//     if( this->m_Interpolator->IsInsideBuffer( mappedPoint ) )
      if (this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint))
        {
//       (*iter).MovingImageValue = this->m_Interpolator->Evaluate( mappedPoint );
    (*iter).imageValueArray[j] =
      this->m_InterpolatorArray[j]->Evaluate (mappedPoint);
    this->m_NumberOfPixelsCounted++;
    allOutside = false;
        }
      else
        {
    (*iter).imageValueArray[j] = 0;
        }
    }
  // Jump to random position
  ++randIter;

      }

    if (allOutside)
      {
  // if all the samples mapped to the outside throw an exception
  itkExceptionMacro
    (<<"All the sampled point mapped to outside of the moving image");
      }
  }


/*
 * Get the match Measure
 */
  template < class TFixedImage >
    typename VarianceMultiImageMetric < TFixedImage >::MeasureType
    VarianceMultiImageMetric <
    TFixedImage >::GetValue(const ParametersType & parameters) const
  {

    int N = this->m_NumberOfImages;
    ParametersType currentParam (this->m_TransformArray[0]->
         GetNumberOfParameters ());

    for (int i = 0; i < N; i++)
      {
      for (int j = 0;
       j < this->m_TransformArray[i]->GetNumberOfParameters (); j++)
        {
        currentParam[j] = parameters[i * N + j];
        }
      this->m_TransformArray[i]->SetParameters (currentParam);
      }



    // collect sample set A
    this->SampleFixedImageDomain (m_SampleA);

    //Calculate variance and mean
    double measure = 0.0;

    typename SpatialSampleContainer::const_iterator iter;
    typename SpatialSampleContainer::const_iterator end = m_SampleA.end ();

    double squareSum, meanSum;
    for (iter = m_SampleA.begin (); iter != end; ++iter)
      {
  squareSum = 0.0;
  meanSum = 0.0;
  for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      squareSum +=
        (*iter).imageValueArray[j] * (*iter).imageValueArray[j];
      meanSum += (*iter).imageValueArray[j] / N;
    }

  measure += (squareSum - meanSum * meanSum) / N;

      }        // end of sample loop
    measure = measure / ((double) m_NumberOfSpatialSamples);

    return measure;

}


/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void 
VarianceMultiImageMetric < TFixedImage >::BeforeGetThreadedValue () const
{
  int numberOfThreads = this->GetNumberOfThreads();

  m_Sum.SetSize( numberOfThreads );
  m_SumOfSquares.SetSize( numberOfThreads );

  m_Sum.Fill( NumericTraits<RealType>::Zero() );
  m_SumOfSquares.Fill( NumericTraits<RealType>::Zero() );

  // collect sample set A
  this->SampleFixedImageDomain (m_SampleA);

}


/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
typename VarianceMultiImageMetric < TFixedImage >::MeasureType
VarianceMultiImageMetric < TFixedImage >::AfterGetThreadedValue () const
{

  const unsigned int numberOfThreads = this->GetNumberOfThreads();

  RealType sum = NumericTraits< RealType >::Zero;
  RealType sumOfSquares = NumericTraits< RealType >::Zero;

  for( unsigned int i=0; i < numberOfThreads; i++ )
    {
    sum += m_Sum[i];
    sumOfSquares += m_SumOfSquares[i];
    } 

  const RealType mean = sum / numberOfThreads;

  const RealType variance = (sumOfSquares - mean * mean) / numberOfThreads;

  return variance;
}


/*
 * Get the match Measure
 */
  template < class TFixedImage >
void
VarianceMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  const unsigned int N = this->m_NumberOfImages;

  ParametersType currentParam (this->m_TransformArray[0]->
       GetNumberOfParameters ());

  // Extract the subset of parameters that are relevant 
  // for this image.
  for (unsigned int i = 0; i < N; i++)
    {
    for (unsigned int j = 0;
     j < this->m_TransformArray[i]->GetNumberOfParameters (); j++)
      {
      currentParam[j] = parameters[i * N + j];
      }
    this->m_TransformArray[i]->SetParameters (currentParam);
    }



  //Calculate variance and mean
  double measure = 0.0;

  typename SpatialSampleContainer::const_iterator iter;
  typename SpatialSampleContainer::const_iterator end = m_SampleA.end ();

  // FIXME: Use only the section of samples that are relevant to this thread ******
  for (iter = m_SampleA.begin (); iter != end; ++iter)
    {

    double sum = 0.0;
    double sumOfSquares = 0.0;

    for( unsigned int j = 0; j < this->m_NumberOfImages; j++)
      {
      const double value = (*iter).imageValueArray[j];

      sum += value;
      sumOfSquares += value * value;
      }

    const double mean = sum / N;

    const double variance = (sumOfSquares - mean * mean) / N;

    this->m_Sum[threadId] += variance;

    this->m_SumOfSquares[threadId] = variance * variance;

    }        // end of sample loop

}


/*
 * Get the both Value and Derivative Measure
 */
  template < class TFixedImage >
    void
    VarianceMultiImageMetric <
    TFixedImage >::GetValueAndDerivative (const ParametersType & parameters,
            MeasureType & value,
            DerivativeType & derivative) const
  {

    double N = (double) this->m_NumberOfImages;

  /** The tranform parameters vector holding i'th images parameters 
      Copy parameters in to a collection of arrays */
//   DerivativeType temp2( 2*this->m_NumberOfImages );
//   temp2.Fill( 0 );
//   derivative = temp2;

      value = NumericTraits < MeasureType >::Zero;

//   unsigned int numberOfParameters = this->m_Transform->GetNumberOfParameters();
    unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters ();
    DerivativeType temp (numberOfParameters * this->m_NumberOfImages);
      temp.Fill (0.0);
      derivative = temp;

    // collect sample set A
      this->SampleFixedImageDomain (m_SampleA);
    // calculate the mutual information
    typename SpatialSampleContainer::iterator aiter;
    typename SpatialSampleContainer::const_iterator aend = m_SampleA.end ();

    // make sure the transform has the current parameters
//   this->m_Transform->SetParameters( parameters );
    DerivativeType derivA (numberOfParameters);
    DerivativeType sum (numberOfParameters);
    ParametersType currentParam (numberOfParameters);


    for (int i = 0; i < this->m_NumberOfImages; i++)
      {
      for (int j = 0;
           j < this->m_TransformArray[i]->GetNumberOfParameters (); j++)
        {
        currentParam[j] = parameters[numberOfParameters * i + j];
        }

    this->m_TransformArray[i]->SetParameters (currentParam);


  // set the DerivativeCalculator
  //m_DerivativeCalculator->SetInputImage( this->m_MovingImage );
  m_DerivativeCalculator->SetInputImage (this->m_ImageArray[i]);

  //Calculate value
  double measure = 0.0;
  double meanSum = 0.0;
  double squareSum = 0.0;
  for (aiter = m_SampleA.begin (); aiter != aend; ++aiter)
    {
      squareSum = 0.0;
      meanSum = 0.0;
      for (int j = 0; j < this->m_NumberOfImages; j++)
        {
    squareSum +=
      (*aiter).imageValueArray[j] * (*aiter).imageValueArray[j];
    meanSum += (*aiter).imageValueArray[j] / N;
        }
      measure += (squareSum - meanSum * meanSum) / N;

    }      // end of sample loop
  value = measure / ((double) m_NumberOfSpatialSamples);



  //Calculate derivative
  double derI = 0;
  sum.Fill (0.0);
  for (aiter = m_SampleA.begin (); aiter != aend; ++aiter)
    {
      meanSum = 0;
      derI = 0;
      for (int j = 0; j < this->m_NumberOfImages; j++)
        meanSum += (*aiter).imageValueArray[j] / N;

      //for(int j=0; j<this->m_NumberOfImages;j++)
      derI =
        2.0 / ((double) m_NumberOfSpatialSamples) / N *
        ((*aiter).imageValueArray[i] - meanSum);
      //cout << meanSum << " " << derI << endl;

      // get the image derivative for this B sample
       this->CalculateDerivatives ((*aiter).FixedImagePointValue,
           derivA,i);
      //this->CalculateDerivatives (this->m_TransformArray[i]->TransformPoint((*aiter).FixedImagePointValue),
  //        derivA,i);
      
      //  cout << derivA << endl << derI << endl << meanSum;
      //cin >> meanSum;

      sum -= derivA * derI;
    }      // end of sample B loop
//     int dummy;
//     cout << this->m_TransformArray[i];
//     cout << this->m_InterpolatorArray[i] << " " << this->m_ImageArray[i];
//     cout << sum;
//     cin >> dummy;
  //copy the properpart of the derivative
  for (int j = 0;
       j < this->m_TransformArray[i]->GetNumberOfParameters (); j++)
    derivative[i * numberOfParameters + j] = sum[j];


  //begin += this->m_TransformArray[i]->GetNumberOfParameters();
//   cout << this->m_TransformArray[i] << endl;
//   cout << meanSum << endl << parameters << endl << sum;
//    int x;
//   cin >> x;
      }
//   derivative[0] = .0;
//   derivative[1] = .0;
    //Remove mean
    sum.Fill (0.0);
    for (int i = 0; i < this->m_NumberOfImages; i++)
      for (int j = 0; j < numberOfParameters; j++)
  sum[j] +=
    derivative[i * numberOfParameters + j] / this->m_NumberOfImages;
//   cout << derivative << endl << sum[0] << " " << sum[1] << endl;
    for (int i = 0; i < this->m_NumberOfImages * numberOfParameters; i++)
      derivative[i] -= sum[i % numberOfParameters];
//   cout << endl << derivative;
//   int x;
//   cin >> x;
//   derivative[0] = .01;
//   derivative[1] = -.0;
//   derivative[2] = .0;
//   derivative[3] = -.01;
//   derivative[4] = .0;
//   derivative[5] = -.0;
//   derivative[6] = .01;
//   derivative[7] = -.01;

    //  cin >> derivative;
  }


/*
 * Get the match measure derivative
 */
  template < class TFixedImage >
    void
    VarianceMultiImageMetric <
    TFixedImage >::GetDerivative (const ParametersType & parameters,
          DerivativeType & derivative) const
  {
    MeasureType value;
    // call the combined version
      this->GetValueAndDerivative (parameters, value, derivative);
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
    void
    VarianceMultiImageMetric <
    TFixedImage >::CalculateDerivatives (const FixedImagePointType & point,
           DerivativeType & derivatives, int i) const
  {

//   MovingImagePointType mappedPoint = this->m_Transform->TransformPoint( point );
    MovingImagePointType mappedPoint =
      this->m_TransformArray[i]->TransformPoint (point);

      CovariantVector < double, MovingImageDimension > imageDerivatives;

    if (m_DerivativeCalculator->IsInsideBuffer (mappedPoint))
      {
  imageDerivatives = m_DerivativeCalculator->Evaluate (mappedPoint);
      }
    else
      {
  derivatives.Fill (0.0);
  return;
      }

    typedef typename TransformType::JacobianType JacobianType;
//   const JacobianType& jacobian = this->m_Transform->GetJacobian( point );
    const JacobianType & jacobian =
      this->m_TransformArray[i]->GetJacobian (point);

//   unsigned int numberOfParameters = this->m_Transform->GetNumberOfParameters();
    unsigned int numberOfParameters =
      this->m_TransformArray[i]->GetNumberOfParameters ();

    for (unsigned int k = 0; k < numberOfParameters; k++)
      {
  derivatives[k] = 0.0;
  for (unsigned int j = 0; j < MovingImageDimension; j++)
    {
      derivatives[k] += jacobian[j][k] * imageDerivatives[j];
    }
      }

  }



/*
 * Reinitialize the seed of the random number generator
 */
  template < class TFixedImage >
    void VarianceMultiImageMetric < TFixedImage >::ReinitializeSeed ()
  {
    Statistics::MersenneTwisterRandomVariateGenerator::GetInstance ()->
      SetSeed ();
  }

/*
 * Reinitialize the seed of the random number generator
 */
  template < class TFixedImage >
    void VarianceMultiImageMetric < TFixedImage >::ReinitializeSeed (int seed)
  {
    Statistics::MersenneTwisterRandomVariateGenerator::GetInstance ()->
      SetSeed (seed);
  }





}        // end namespace itk


#endif
