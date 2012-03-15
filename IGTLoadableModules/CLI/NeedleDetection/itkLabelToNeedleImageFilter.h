/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHessian3DToNeedleImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2009-04-25 12:27:26 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLabelToNeedleImageFilter_h
#define __itkLabelToNeedleImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkAffineTransform.h"

namespace itk
{
/** \class LabelToNeedleImageFilter
 * \brief 
 * \ingroup IntensityImageFilters TensorObjects
 *
 */
  
template < typename  TInput, typename TOutput  >
class ITK_EXPORT LabelToNeedleImageFilter : public
ImageToImageFilter< TInput, TOutput >
{
public:
  /** Standard class typedefs. */
  typedef LabelToNeedleImageFilter Self;
  typedef ImageToImageFilter<
          TInput,
          TOutput >                               Superclass;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;
  
  typedef typename Superclass::InputImageType            InputImageType;
  typedef typename Superclass::OutputImageType           OutputImageType;
  typedef typename InputImageType::PixelType             InputPixelType;
  typedef typename OutputImageType::PixelType            OutputPixelType;
  
  typedef typename itk::AffineTransform< float, 3 >      NeedleTransformType;

  /** Image dimension = 3. */
  //itkStaticConstMacro(ImageDimension, unsigned int,
  //                    ::itk::GetImageDimension<InputImageType>::ImageDimension);
  //itkStaticConstMacro(InputPixelDimension, unsigned int,
  //                    InputPixelType::Dimension);

  /** Run-time type information (and related methods).   */
  itkTypeMacro( LabelToNeedleImageFilter, ImageToImageFilter );

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  itkSetMacro(MaxMinorAxisLength, float);
  itkGetConstMacro(MaxMinorAxisLength, float);

  itkSetMacro(MinPrincipalAxisLength, float);
  itkGetConstMacro(MinPrincipalAxisLength, float);

  itkSetMacro(AngleThreshold, double);
  itkGetConstMacro(AngleThreshold, double);

  inline void SetNormal(double x, double y, double z)
  {
    m_Normal[0] = x; m_Normal[1] = y; m_Normal[2] = z;
  }

  inline void GetNormal(double& x, double& y, double& z)
  {
    x = m_Normal[0]; y = m_Normal[1]; z = m_Normal[2];
  }

  inline void SetClosestPoint(double x, double y, double z)
  {
    m_ClosestPoint[0] = x; m_ClosestPoint[1] = y; m_ClosestPoint[2] = z;
  }

  inline void GetClosestPoint(double& x, double& y, double& z)
  {
    x = m_ClosestPoint[0]; y = m_ClosestPoint[1]; z = m_ClosestPoint[2];
  }

  inline NeedleTransformType * GetNeedleTransform()
  {
    return m_NeedleTransform;
  }

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(DoubleConvertibleToOutputCheck,
                  (Concept::Convertible<double, OutputPixelType>));
  /** End concept checking */
#endif


protected:
  LabelToNeedleImageFilter();
  ~LabelToNeedleImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Generate Data */
  void GenerateData( void );

private:
  LabelToNeedleImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  float m_MinPrincipalAxisLength;
  float m_MaxMinorAxisLength;
  double m_AngleThreshold;
  double m_Normal[3];
  double m_ClosestPoint[3];

  NeedleTransformType::Pointer m_NeedleTransform;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLabelToNeedleImageFilter.txx"
#endif
  
#endif
