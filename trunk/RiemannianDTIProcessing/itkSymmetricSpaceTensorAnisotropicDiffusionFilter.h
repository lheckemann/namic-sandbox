/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVectorGradientAnisotropicDiffusionImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:59 $
  Version:   $Revision: 1.20 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSymmetricSpaceTensorAnisotropicDiffusionFilter_h_
#define __itkSymmetricSpaceTensorAnisotropicDiffusionFilter_h_

#include "itkExceptionObject.h"
#include "itkAnisotropicDiffusionImageFilter.h"
#include "itkSymmetricSpaceTensorAnisotropicDiffusionFunction.h"
#include "itkDenseFiniteDifferenceImageFilter.h"
#include "SymmetricSpaceTensorGeometry.h"

namespace itk {

  /** \class SymmetricSpaceTensorAnisotropicDiffusionFilter
   * Implements Anisotropic Diffusion in the Symmetric Space
   * for Tensors.
   * For more details:
   * Refer: "A Riemannian Framework for Tensor Computing"
   *         Intl. Journal of Computer Vision"
   *         Pennec et. al. March 2005.
   *
   * \ingroup ImageEnhancement
   */
  template <class TInputImage, class TOutputImage>
class ITK_EXPORT SymmetricSpaceTensorAnisotropicDiffusionFilter
  : public AnisotropicDiffusionImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef SymmetricSpaceTensorAnisotropicDiffusionFilter Self;
  typedef AnisotropicDiffusionImageFilter<TInputImage, TOutputImage>
  Superclass;
  typedef DenseFiniteDifferenceImageFilter<TInputImage, TOutputImage>
    DenseFiniteDifferenceFilterclass;

  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  /** The pixel type of the output image will be used in computations.
   * Inherited from the superclass. */
  typedef typename Superclass::PixelType PixelType;

  /** The value type of a time step. */
  typedef typename DenseFiniteDifferenceFilterclass::TimeStepType TimeStepType;
  
  /* The type of out put Image for this filter */
  typedef typename Superclass::OutputImageType OutputImageType;
  
  /** The container type for the update buffer. */
  typedef OutputImageType UpdateBufferType;
  
  /** The type of region used for multithreading */
  typedef typename UpdateBufferType::RegionType ThreadRegionType;

/**Type of Value in a tensor (6 doubles or floats)*/
  typedef typename TensorGeometry<float,3>::TensorType TensorType;

   /** Instantiation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information. */
  itkTypeMacro(SymmetricSpaceTensorAnisotropicDiffusionFilter,
               AnisotropicDiffusionImageFilter);
  

  /** Determine the image dimension from the  superclass. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      Superclass::ImageDimension );
 
protected:
  SymmetricSpaceTensorAnisotropicDiffusionFilter()
  {
    typename SymmetricSpaceTensorAnisotropicDiffusionFunction<UpdateBufferType>::Pointer p        
      = SymmetricSpaceTensorAnisotropicDiffusionFunction<UpdateBufferType>::New();
    this->SetDifferenceFunction(p);
  }
  ~SymmetricSpaceTensorAnisotropicDiffusionFilter() {}

private:
  SymmetricSpaceTensorAnisotropicDiffusionFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  /**  Does the actual work of updating the output from the UpdateContainer over
   *  an output region supplied by the multithreading mechanism.
   *  Overloaded the ApplyUpdate in DenseFiniteDifferenceImageFilter
   *  to support update in Symmetric Space.
   */ 
  virtual
  void ThreadedApplyUpdate(TimeStepType dt,
                           const ThreadRegionType &regionToProcess,
                           int threadId);
};

} // end namspace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSymmetricSpaceTensorAnisotropicDiffusionFilter.txx"
#endif


#endif
