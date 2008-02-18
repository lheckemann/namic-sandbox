/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: BSplineDeformableTransformToKernelTransformInverseCalculator.h,v $
  Language:  C++
  Date:      $Date: 2007/11/16 21:27:41 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __BSplineDeformableTransformToKernelTransformInverseCalculator_h
#define __BSplineDeformableTransformToKernelTransformInverseCalculator_h

#include "itkImage.h"
#include "itkCommand.h"

#include "itkBSplineDeformableTransform.h"
#include "itkKernelTransform.h"


namespace itk
{

template< class TImage >
class BSplineDeformableTransformToKernelTransformInverseCalculator : public Object
  {
  public:
    typedef BSplineDeformableTransformToKernelTransformInverseCalculator Self;
    typedef Object Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkTypeMacro(BSplineDeformableTransformToKernelTransformInverseCalculator, Object);

    itkNewMacro(Self);
  
    typedef typename TImage::PixelType PixelType;
    typedef typename TImage::RegionType RegionType;

    typedef itk::BSplineDeformableTransform<double, 3, 3> DirectTransformType;
    typedef itk::KernelTransform< double, 3 >             InverseTransformType;

    itkSetConstObjectMacro( DirectTransform, DirectTransformType );
    itkGetConstObjectMacro( DirectTransform, DirectTransformType );

    itkGetConstObjectMacro( InverseTransform, InverseTransformType );

    void ComputeInverseTransform();

  protected:
    BSplineDeformableTransformToKernelTransformInverseCalculator();
    virtual ~BSplineDeformableTransformToKernelTransformInverseCalculator();

    void PrintUncaughtError();

    void PrintError(ExceptionObject &e);


  private:

    DirectTransformType::ConstPointer   m_DirectTransform;
    InverseTransformType::Pointer       m_InverseTransform;


  }; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBSplineDeformableTransformToKernelTransformInverseCalculator.txx"
#endif

#endif //__BSplineDeformableTransformToKernelTransformInverseCalculator_H

