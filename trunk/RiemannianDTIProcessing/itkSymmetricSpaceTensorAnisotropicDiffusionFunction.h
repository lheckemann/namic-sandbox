/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSymmetricSpaceTensorAnisotropicDiffusionFunction.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:58 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSymmetricSpaceTensorAnisotropicDiffusionFunction_h_
#define __itkSymmetricSpaceTensorAnisotropicDiffusionFunction_h_

#include "itkAnisotropicDiffusionFunction.h"
#include "itkVector.h"
#include "itkSymmetricSecondRankTensor.h"
//Namic Includes-------------------
#include "SymmetricSpaceTensorGeometry.h"
namespace itk {

/** \class SymmetricSpaceTensorAnisotropicDiffusionFunction
 *
 * This class implements 
 * AnisotropicDiffusionFunction for Tensors in the Symmetric Space. 
 * This class overrides the following two functions
 * 
 * 1.  CalculateUpdate 
 * 2.  ComputeGradientMagnitudeSquared
 *  
 * These notion of Gradient and Laplacian in the Symmetric Space
 * is different from that in the Euclidean Space.Hence we
 * overload these two functions to perform Anisotropic
 * Diffusion on Tensors in the Symmetric Space.
 *
 * For more details:
 * Refer: "A Riemannian Framework for Tensor Computing"
 *         Intl. Journal of Computer Vision"
 *         Pennec et. al. March 2005.
 *
 * Note we are representing Symmetric Tensors as 
 * a 6 component vector. The vectors are then transformed to the the Symmetric
 * Space to calculate the Laplacian and the Gradient Magnitude.
 
 *  \ingroup FiniteDifferenceFunctions
 *  \ingroup ImageEnhancement
 *
 * \sa AnisotropicDiffusionFunction
 * \sa ScalarAnisotropicDiffusionFunction
 * */
template <class TImage>
class ITK_EXPORT SymmetricSpaceTensorAnisotropicDiffusionFunction :
    public AnisotropicDiffusionFunction<TImage>
{
public:
  /** Standard class typedefs. */
  typedef SymmetricSpaceTensorAnisotropicDiffusionFunction   Self;
  typedef AnisotropicDiffusionFunction<TImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro(SymmetricSpaceTensorAnisotropicDiffusionFunction,
               AnisotropicDiffusionFunction);
  
  /** Inherit some parameters from the superclass type */
  typedef typename Superclass::ImageType        ImageType;
  typedef typename Superclass::PixelType        PixelType;
  typedef typename Superclass::TimeStepType     TimeStepType;
  typedef typename Superclass::RadiusType       RadiusType;
  typedef typename Superclass::NeighborhoodType NeighborhoodType;
  typedef typename Superclass::FloatOffsetType FloatOffsetType;

    /** Type of a value in a vector (double, float, etc.) */
  typedef typename PixelType::ValueType  ScalarValueType;
  /**Type of Value in a tensor (6 doubles or floats)*/
  typedef typename TensorGeometry<float,3>::TensorType TensorType;

  /** Inherit some parameters from the superclass type */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      Superclass::ImageDimension );
  itkStaticConstMacro(VectorDimension, unsigned int, PixelType::Dimension );

  /** Compute the average gradient magnitude squared. */
  virtual void CalculateAverageGradientMagnitudeSquared(TImage *);
  /** Compute the equation value. */
  virtual PixelType ComputeUpdate(const NeighborhoodType &neighborhood,
                                  void * globalData,
                                  const FloatOffsetType & offset = FloatOffsetType(0.0));
  
  /** This method is called prior to each iteration of the solver. */
  virtual void InitializeIteration()
  {
    m_K = this->GetAverageGradientMagnitudeSquared() * this->GetConductanceParameter() *
      this->GetConductanceParameter() * -2.0f;
  }

protected:
  SymmetricSpaceTensorAnisotropicDiffusionFunction() ;
  ~SymmetricSpaceTensorAnisotropicDiffusionFunction() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  { Superclass::PrintSelf(os,indent); }
  double computeGradientSquaredFromNeighbors(ConstNeighborhoodIterator<TImage> it,
                                                     long dimension);

  
private:

  SymmetricSpaceTensorAnisotropicDiffusionFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  //Helper functions for Symmetric Space derivative and Norm calculations
  PixelType GetSymmetricSpaceDerivative(PixelType reference,PixelType neighbor);
  double GetSymmetricSpaceNormSquared(PixelType reference,PixelType neighbor);

  static double m_MIN_NORM;
  /** Modified global average gradient magnitude term. */
  ScalarValueType m_K;
  unsigned long int m_Stride[ImageDimension];
  unsigned long int m_Center;


  
};

}// end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSymmetricSpaceTensorAnisotropicDiffusionFunction.txx"
#endif

#endif
