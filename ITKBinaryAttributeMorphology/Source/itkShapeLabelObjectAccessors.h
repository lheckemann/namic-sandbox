/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLabelObject.h,v $
  Language:  C++
  Date:      $Date: 2005/01/21 20:13:31 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkShapeLabelObjectFunctors_h
#define __itkShapeLabelObjectFunctors_h


namespace itk
{

namespace Functor 
{

template< class TLabelObject >
class ITK_EXPORT SizeLabelObjectAccessor
{
public:
  typedef TLabelObject  LabelObjectType;
  typedef unsigned long AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetSize();
    }
};

template< class TLabelObject >
class ITK_EXPORT PhysicalSizeLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetPhysicalSize();
    }
};

template< class TLabelObject >
class ITK_EXPORT RegionElongationLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetRegionElongation();
    }
};

template< class TLabelObject >
class ITK_EXPORT SizeRegionRatioLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetSizeRegionRatio();
    }
};

template< class TLabelObject >
class ITK_EXPORT SizeOnBorderLabelObjectAccessor
{
public:
  typedef TLabelObject  LabelObjectType;
  typedef unsigned long AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetSizeOnBorder();
    }
};

template< class TLabelObject >
class ITK_EXPORT PhysicalSizeOnBorderLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetPhysicalSizeOnBorder();
    }
};

template< class TLabelObject >
class ITK_EXPORT CentroidLabelObjectAccessor
{
public:
  typedef TLabelObject                           LabelObjectType;
  typedef typename LabelObjectType::CentroidType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetCentroid();
    }
};

template< class TLabelObject >
class ITK_EXPORT FeretDiameterLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetFeretDiameter();
    }
};

template< class TLabelObject >
class ITK_EXPORT BinaryPrincipalMomentsLabelObjectAccessor
{
public:
  typedef TLabelObject                         LabelObjectType;
  typedef typename LabelObjectType::VectorType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetBinaryPrincipalMoments();
    }
};

template< class TLabelObject >
class ITK_EXPORT BinaryPrincipalAxesLabelObjectAccessor
{
public:
  typedef TLabelObject                         LabelObjectType;
  typedef typename LabelObjectType::MatrixType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetBinaryPrincipalAxes();
    }
};

template< class TLabelObject >
class ITK_EXPORT BinaryElongationLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetBinaryElongation();
    }
};

template< class TLabelObject >
class ITK_EXPORT PerimeterLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetPerimeter();
    }
};

template< class TLabelObject >
class ITK_EXPORT RoundnessLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetRoundness();
    }
};

template< class TLabelObject >
class ITK_EXPORT EquivalentRadiusLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetEquivalentRadius();
    }
};

template< class TLabelObject >
class ITK_EXPORT EquivalentPerimeterLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetEquivalentPerimeter();
    }
};

template< class TLabelObject >
class ITK_EXPORT EquivalentEllipsoidSizeLabelObjectAccessor
{
public:
  typedef TLabelObject                         LabelObjectType;
  typedef typename LabelObjectType::VectorType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetEquivalentEllipsoidSize();
    }
};

template< class TLabelObject >
class ITK_EXPORT BinaryFlatnessLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetBinaryFlatness();
    }
};

}

} // end namespace itk

#endif
