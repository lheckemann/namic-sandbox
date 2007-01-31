/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGaussianDerivativeImageFunction.txx,v $
  Language:  C++
  Date:      $Date: 2006/02/16 14:17:42 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSurfaceDerivativesImageFunction_txx
#define __itkSurfaceDerivativesImageFunction_txx

#include "itkSurfaceDerivativesImageFunction.h"
#include "itkDerivativeOperator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhood.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"

namespace itk
{

/** Set the Input Image */
template <class TInputImage, class TOutput>
SurfaceDerivativesImageFunction<TInputImage,TOutput>
::SurfaceDerivativesImageFunction()
{
  m_OperatorImageFunction = OperatorImageFunctionType::New();
  m_DifferencingDirection = 0; // Initializing to the default value
}

/** Print self method */
template <class TInputImage, class TOutput>
void
SurfaceDerivativesImageFunction<TInputImage,TOutput>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
  /*
  os << indent << "UseImageSpacing: " << m_UseImageSpacing << std::endl;

  os << indent << "Sigma: " << m_Sigma << std::endl;
     */
}

/** Set the input image */
template <class TInputImage, class TOutput>
void
SurfaceDerivativesImageFunction<TInputImage,TOutput>
::SetInputImage( const InputImageType * ptr )
{
  Superclass::SetInputImage(ptr);
  m_OperatorImageFunction->SetInputImage(ptr);
}

/** Evaluate the function at the specifed index */
template <class TInputImage, class TOutput>
typename SurfaceDerivativesImageFunction<TInputImage,TOutput>::OutputType
SurfaceDerivativesImageFunction<TInputImage, TOutput>
::EvaluateAtIndex(const IndexType& index) const
{
  OutputType surfaceDerivatives;
  // get the normal at the index passed in... whether backward or forward diff to get normal 
  // depends on 'm_DifferencingDirection'
  VectorType normalToUse;
  itk::ConstNeighborhoodIterator<InputImageType>::SizeType size;
  size.Fill(1);
  itk::ConstNeighborhoodIterator< InputImageType>  //, itk::ZeroFluxNeumannBoundaryCondition<InputImageType> > 
     iterator(size, (SurfaceDerivativesImageFunction<TInputImage, TOutput>::InputImageType *) m_OperatorImageFunction->GetInputImage(), m_OperatorImageFunction->GetInputImage()->GetRequestedRegion());
  int i1,i2;
  iterator.SetLocation(index);
  itk::ConstNeighborhoodIterator<InputImageType>::PixelType currPtValue = iterator.GetCenterPixel();
  std::cout << "\n curr= " << currPtValue;
  itk::ConstNeighborhoodIterator<InputImageType>::PixelType neighbor;
  double gradientNorm =0.0;
  if(m_DifferencingDirection==0) // Implies forward differencing desired
  {
  for(i1=0;i1<itkGetStaticConstMacro(ImageDimension2);i1++)
  {
    neighbor = iterator.GetNext(i1);
    normalToUse[i1] = (neighbor - currPtValue);
    std::cout << "\n RN = " << neighbor;
    gradientNorm+= normalToUse[i1]*normalToUse[i1];
  }
  }
  else
  {
  for(i1=0;i1<itkGetStaticConstMacro(ImageDimension2);i1++)
  {
    neighbor = iterator.GetPrevious(i1);
    normalToUse[i1] = (currPtValue - neighbor);
    std::cout << "\n LN = " << neighbor;
    gradientNorm+= normalToUse[i1]*normalToUse[i1];
  }  
  }
  // normalize the normal!!!
  gradientNorm = sqrt(gradientNorm);
  if (gradientNorm>0.0)
  {
  for(i1=0;i1<itkGetStaticConstMacro(ImageDimension2);i1++)
  {
    normalToUse[i1] /= gradientNorm;
  }
  }
  
  // Compute (I-NN') now
  for(i1=0;i1<itkGetStaticConstMacro(ImageDimension2);i1++)
  {
  for(i2=0;i2<itkGetStaticConstMacro(ImageDimension2);i2++)
  {
    surfaceDerivatives[i1][i2] = -1.0 * normalToUse[i1]*normalToUse[i2];
  }
  surfaceDerivatives[i1][i1] = 1 + surfaceDerivatives[i1][i1]; // adding 1 to the diagonal in -NN'
  }
  return surfaceDerivatives;

}

/** Evaluate the function at the specifed point */
template <class TInputImage, class TOutput>
typename SurfaceDerivativesImageFunction<TInputImage,TOutput>::OutputType
SurfaceDerivativesImageFunction<TInputImage,TOutput>
::Evaluate(const PointType& point) const
{
  IndexType index;
  this->ConvertPointToNearestIndex( point , index );
  return this->EvaluateAtIndex ( index );
}

/** Evaluate the function at specified ContinousIndex position.*/
template <class TInputImage, class TOutput>
typename SurfaceDerivativesImageFunction<TInputImage,TOutput>::OutputType
SurfaceDerivativesImageFunction<TInputImage,TOutput>
::EvaluateAtContinuousIndex(const ContinuousIndexType & cindex ) const
{
  IndexType index;
  this->ConvertContinuousIndexToNearestIndex( cindex, index  ); 
  return this->EvaluateAtIndex( index );
}


template <class TInputImage, class TOutput>
void
SurfaceDerivativesImageFunction<TInputImage,TOutput>
::SetDifferencingDirection(int direction)
{
  if(direction==0 || direction==1)
  {
    m_DifferencingDirection = direction;
  }
  /* else
  {
     display an error that this is an invalid direction 
      } */
}


} // end namespace itk

#endif
