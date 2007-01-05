/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAffineInvariantCurvatureFlowImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2003/10/07 16:42:27 $
  Version:   $Revision: 1.32 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAffineInvariantCurvatureFlowImageFilter_txx_
#define __itkAffineInvariantCurvatureFlowImageFilter_txx_

#include "itkAffineInvariantCurvatureFlowImageFilter.h"
#include "itkExceptionObject.h"

namespace itk
{

/*
 * Constructor
 */
template <class TInputImage, class TOutputImage>
AffineInvariantCurvatureFlowImageFilter<TInputImage, TOutputImage>
::AffineInvariantCurvatureFlowImageFilter()
{

  this->SetNumberOfIterations(0);
  m_TimeStep   = 0.05f;

  typename AffineInvariantCurvatureFlowFunctionType::Pointer cffp;
  cffp = AffineInvariantCurvatureFlowFunctionType::New();

  this->SetDifferenceFunction( static_cast<FiniteDifferenceFunctionType *>( 
                                 cffp.GetPointer() ) );

}


/*
 * Standard PrintSelf method.
 */
template <class TInputImage, class TOutputImage>
void
AffineInvariantCurvatureFlowImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << std::endl;
  os << indent << "Time step: " << m_TimeStep;
  os << std::endl;
}


/*
 * Initialize the state of filter and equation before each iteration.
 */
template <class TInputImage, class TOutputImage>
void
AffineInvariantCurvatureFlowImageFilter<TInputImage, TOutputImage>
::InitializeIteration()
{

  // update variables in the equation object
  AffineInvariantCurvatureFlowFunctionType *f = 
    dynamic_cast<AffineInvariantCurvatureFlowFunctionType *>
    (this->GetDifferenceFunction().GetPointer());

  if ( !f )
    {
    itkExceptionMacro(<<"DifferenceFunction not of type AffineInvariantCurvatureFlowFunction");
    }

  f->SetTimeStep( m_TimeStep );

  // call superclass's version
  this->Superclass::InitializeIteration();           

  // progress feedback
  if ( this->GetNumberOfIterations() != 0 )
    {
    this->UpdateProgress(((float)(this->GetElapsedIterations()))
                         /((float)(this->GetNumberOfIterations())));
    }
  
}


/*
 * GenerateInputRequestedRegion
 */
template <class TInputImage, class TOutputImage>
void
AffineInvariantCurvatureFlowImageFilter<TInputImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass's implementation
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the input and output
  typename Superclass::InputImagePointer  inputPtr  = 
    const_cast< InputImageType * >( this->GetInput() );
  OutputImagePointer outputPtr = this->GetOutput();

  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // set the input requested region to be the same as
  // the output requested region
  inputPtr->SetRequestedRegion(
    outputPtr->GetRequestedRegion() );

}


/*
 * EnlargeOutputRequestedRegion
 */
template <class TInputImage, class TOutputImage>
void
AffineInvariantCurvatureFlowImageFilter<TInputImage, TOutputImage>
::EnlargeOutputRequestedRegion(
  DataObject * ptr )
{

  // convert DataObject pointer to OutputImageType pointer 
  OutputImageType * outputPtr;
  outputPtr = dynamic_cast<OutputImageType*>( ptr );

  // get input image pointer
  typename Superclass::InputImagePointer  inputPtr  = 
    const_cast< InputImageType * >( this->GetInput() );
  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // Get the size of the neighborhood on which we are going to operate.  This
  // radius is supplied by the difference function we are using.
  typename FiniteDifferenceFunctionType::RadiusType radius
    = this->GetDifferenceFunction()->GetRadius();

  for( unsigned int j = 0; j < ImageDimension; j++ )
    {
    radius[j] *= this->GetNumberOfIterations();
    }

  /*
   * NewOutputRequestedRegion = OldOutputRequestedRegion +
   * radius * m_NumberOfIterations padding on each edge
   */
  typename OutputImageType::RegionType outputRequestedRegion =
    outputPtr->GetRequestedRegion();

  outputRequestedRegion.PadByRadius( radius );
  outputRequestedRegion.Crop( outputPtr->GetLargestPossibleRegion() );
  
  outputPtr->SetRequestedRegion( outputRequestedRegion );

  
}


} // end namespace itk

#endif
