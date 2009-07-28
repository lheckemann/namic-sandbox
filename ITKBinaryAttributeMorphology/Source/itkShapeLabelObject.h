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
#ifndef __itkShapeLabelObject_h
#define __itkShapeLabelObject_h

#include "itkLabelObject.h"
#include "itkLabelMap.h"
#include "itkAffineTransform.h"
#include "itkMatrix.h"

namespace itk
{

/** \class ShapeLabelObject
 * \brief TODO
 *
 *
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/1926/584  or 
 * http://www.insight-journal.org/browse/publication/176
 *
 *
 * \ingroup TODO 
 */


namespace Functor {

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


/** \class ShapeLabelObject
 *  \brief A Label object to store the common attributes related to the shape of the object
 *
 * ShapeLabelObject stores  the common attributes related to the shape of the object
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * \ingroup DataRepresentation 
 */
template < class TLabel, unsigned int VImageDimension >
class ITK_EXPORT ShapeLabelObject : public LabelObject< TLabel, VImageDimension >
{
public:
  /** Standard class typedefs */
  typedef ShapeLabelObject                       Self;
  typedef LabelObject< TLabel, VImageDimension > Superclass;
  typedef typename Superclass::LabelObjectType   LabelObjectType;
  typedef SmartPointer<Self>                     Pointer;
  typedef SmartPointer<const Self>               ConstPointer;
  typedef WeakPointer<const Self>                ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ShapeLabelObject, LabelObject);

  typedef LabelMap< Self > LabelMapType;

  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

  typedef typename Superclass::IndexType IndexType;

  typedef TLabel LabelType;

  typedef typename Superclass::LineType LineType;

  typedef typename Superclass::LengthType LengthType;

  typedef typename Superclass::LineContainerType LineContainerType;

  typedef typename Superclass::AttributeType AttributeType;
  static const AttributeType SIZE=100;
  static const AttributeType PHYSICAL_SIZE=101;
  static const AttributeType REGION_ELONGATION=102;
  static const AttributeType SIZE_REGION_RATIO=103;
  static const AttributeType CENTROID=104;
  static const AttributeType REGION=105;
  static const AttributeType SIZE_ON_BORDER=106;
  static const AttributeType PHYSICAL_SIZE_ON_BORDER=107;
  static const AttributeType FERET_DIAMETER=108;
  static const AttributeType BINARY_PRINCIPAL_MOMENTS=109;
  static const AttributeType BINARY_PRINCIPAL_AXES=110;
  static const AttributeType BINARY_ELONGATION=111;
  static const AttributeType PERIMETER=112;
  static const AttributeType ROUNDNESS=113;
  static const AttributeType EQUIVALENT_RADIUS=114;
  static const AttributeType EQUIVALENT_PERIMETER=115;
  static const AttributeType EQUIVALENT_ELLIPSOID_RADIUS=116;
  static const AttributeType BINARY_FLATNESS=117;

  static AttributeType GetAttributeFromName( const std::string & s )
    {
    if( s == "Size" )
      {
      return SIZE;
      }
    else if( s == "PhysicalSize" )
      {
      return PHYSICAL_SIZE;
      }
    else if( s == "RegionElongation" )
      {
      return REGION_ELONGATION;
      }
    else if( s == "SizeRegionRatio" )
      {
      return SIZE_REGION_RATIO;
      }
    else if( s == "Centroid" )
      {
      return CENTROID;
      }
    else if( s == "Region" )
      {
      return REGION;
      }
    else if( s == "SizeOnBorder" )
      {
      return SIZE_ON_BORDER;
      }
    else if( s == "PhysicalSizeOnBorder" )
      {
      return PHYSICAL_SIZE_ON_BORDER;
      }
    else if( s == "FeretDiameter" )
      {
      return FERET_DIAMETER;
      }
    else if( s == "BinaryPrincipalMoments" )
      {
      return BINARY_PRINCIPAL_MOMENTS;
      }
    else if( s == "BinaryPrincipalAxes" )
      {
      return BINARY_PRINCIPAL_AXES;
      }
    else if( s == "BinaryElongation" )
      {
      return BINARY_ELONGATION;
      }
    else if( s == "Perimeter" )
      {
      return PERIMETER;
      }
    else if( s == "Roundness" )
      {
      return ROUNDNESS;
      }
    else if( s == "EquivalentRadius" )
      {
      return EQUIVALENT_RADIUS;
      }
    else if( s == "EquivalentPerimeter" )
      {
      return EQUIVALENT_PERIMETER;
      }
    else if( s == "EquivalentEllipsoidSize" )
      {
      return EQUIVALENT_ELLIPSOID_RADIUS;
      }
    else if( s == "BinaryFlatness" )
      {
      return BINARY_FLATNESS;
      }
    // can't recognize the name
    return Superclass::GetAttributeFromName( s );
    }

  static std::string GetNameFromAttribute( const AttributeType & a )
    {
    switch( a )
      {
      case SIZE:
        return "Size";
        break;
      case PHYSICAL_SIZE:
        return "PhysicalSize";
        break;
      case REGION_ELONGATION:
        return "RegionElongation";
        break;
      case SIZE_REGION_RATIO:
        return "SizeRegionRatio";
        break;
      case CENTROID:
        return "Centroid";
      case REGION:
        return "Region";
        break;
      case SIZE_ON_BORDER:
        return "SizeOnBorder";
        break;
      case PHYSICAL_SIZE_ON_BORDER:
        return "PhysicalSizeOnBorder";
        break;
      case FERET_DIAMETER:
        return "FeretDiameter";
        break;
      case BINARY_PRINCIPAL_MOMENTS:
        return "BinaryPrincipalMoments";
        break;
      case BINARY_PRINCIPAL_AXES:
        return "BinaryPrincipalAxes";
        break;
      case BINARY_ELONGATION:
        return "BinaryElongation";
        break;
      case PERIMETER:
        return "Perimeter";
        break;
      case ROUNDNESS:
        return "Roundness";
        break;
      case EQUIVALENT_RADIUS:
        return "EquivalentRadius";
        break;
      case EQUIVALENT_PERIMETER:
        return "EquivalentPerimeter";
        break;
      case EQUIVALENT_ELLIPSOID_RADIUS:
        return "EquivalentEllipsoidSize";
        break;
      case BINARY_FLATNESS:
        return "BinaryFlatness";
        break;
      }
    // can't recognize the name
    return Superclass::GetNameFromAttribute( a );
    }

  typedef ImageRegion< ImageDimension > RegionType;

  typedef typename itk::Point<double, ImageDimension> CentroidType;

  typedef Matrix< double, ImageDimension, ImageDimension >   MatrixType;

  typedef Vector< double, ImageDimension > VectorType;

  const RegionType & GetRegion() const
    {
    return m_Region;
    }

  void SetRegion( const RegionType & v )
    {
    m_Region = v;
    }

  const double & GetPhysicalSize() const
    {
    return m_PhysicalSize;
    }

  void SetPhysicalSize( const double & v )
    {
    m_PhysicalSize = v;
    }

  const unsigned long & GetSize() const
    {
    return m_Size;
    }

  void SetSize( const unsigned long & v )
    {
    m_Size = v;
    }

  const CentroidType & GetCentroid() const
    {
    return m_Centroid;
    }

  void SetCentroid( const CentroidType & centroid )
    {
    m_Centroid = centroid;
    }

  const double & GetRegionElongation() const
    {
    return m_RegionElongation;
    }

  void SetRegionElongation( const double & v )
    {
    m_RegionElongation = v;
    }

  const double & GetSizeRegionRatio() const
    {
    return m_SizeRegionRatio;
    }

  void SetSizeRegionRatio( const double & v )
    {
    m_SizeRegionRatio = v;
    }

  const unsigned long & GetSizeOnBorder() const
    {
    return m_SizeOnBorder;
    }

  void SetSizeOnBorder( const unsigned long & v )
    {
    m_SizeOnBorder = v;
    }

  const double & GetPhysicalSizeOnBorder() const
    {
    return m_PhysicalSizeOnBorder;
    }

  void SetPhysicalSizeOnBorder( const double & v )
    {
    m_PhysicalSizeOnBorder = v;
    }

  const double & GetFeretDiameter() const
    {
    return m_FeretDiameter;
    }

  void SetFeretDiameter( const double & v )
    {
    m_FeretDiameter = v;
    }

  const VectorType & GetBinaryPrincipalMoments() const
    {
    return m_BinaryPrincipalMoments;
    }

  void SetBinaryPrincipalMoments( const VectorType & v )
    {
    m_BinaryPrincipalMoments = v;
    }

  const MatrixType & GetBinaryPrincipalAxes() const
    {
    return m_BinaryPrincipalAxes;
    }

  void SetBinaryPrincipalAxes( const MatrixType & v )
    {
    m_BinaryPrincipalAxes = v;
    }

  const double & GetBinaryElongation() const
    {
    return m_BinaryElongation;
    }

  void SetBinaryElongation( const double & v )
    {
    m_BinaryElongation = v;
    }

  const double & GetPerimeter() const
    {
    return m_Perimeter;
    }

  void SetPerimeter( const double & v )
    {
    m_Perimeter = v;
    }

  const double & GetRoundness() const
    {
    return m_Roundness;
    }

  void SetRoundness( const double & v )
    {
    m_Roundness = v;
    }

  const double & GetEquivalentRadius() const
    {
    return m_EquivalentRadius;
    }

  void SetEquivalentRadius( const double & v )
    {
    m_EquivalentRadius = v;
    }

  const double & GetEquivalentPerimeter() const
    {
    return m_EquivalentPerimeter;
    }

  void SetEquivalentPerimeter( const double & v )
    {
    m_EquivalentPerimeter = v;
    }

  const VectorType & GetEquivalentEllipsoidSize() const
    {
    return m_EquivalentEllipsoidSize;
    }

  void SetEquivalentEllipsoidSize( const VectorType & v )
    {
    m_EquivalentEllipsoidSize = v;
    }

  const double & GetBinaryFlatness() const
    {
    return m_BinaryFlatness;
    }

  void SetBinaryFlatness( const double & v )
    {
    m_BinaryFlatness = v;
    }


  // some helper methods - not really required, but really useful!

  /** Affine transform for mapping to and from principal axis */
  typedef AffineTransform<double,itkGetStaticConstMacro(ImageDimension)> AffineTransformType;
  typedef typename AffineTransformType::Pointer      AffineTransformPointer;

  /** Get the affine transform from principal axes to physical axes
   * This method returns an affine transform which transforms from
   * the principal axes coordinate system to physical coordinates. */
  AffineTransformPointer GetBinaryPrincipalAxesToPhysicalAxesTransform() const
    {
    typename AffineTransformType::MatrixType matrix;
    typename AffineTransformType::OffsetType offset;
    for (unsigned int i = 0; i < ImageDimension; i++) 
      {
      offset[i]  = m_Centroid[i];
      for (unsigned int j = 0; j < ImageDimension; j++)
        {
        matrix[j][i] = m_BinaryPrincipalAxes[i][j];    // Note the transposition
        }
      }
  
    AffineTransformPointer result = AffineTransformType::New();
      
    result->SetMatrix(matrix);
    result->SetOffset(offset);
  
    return result;
    }

  /** Get the affine transform from physical axes to principal axes
   * This method returns an affine transform which transforms from
   * the physical coordinate system to the principal axes coordinate
   * system. */
  AffineTransformPointer GetPhysicalAxesToBinaryPrincipalAxesTransform(void) const
    {
    typename AffineTransformType::MatrixType matrix;
    typename AffineTransformType::OffsetType offset;
    for (unsigned int i = 0; i < ImageDimension; i++) 
      {
      offset[i]    = m_Centroid[i];
      for (unsigned int j = 0; j < ImageDimension; j++)
        {
        matrix[j][i] = m_BinaryPrincipalAxes[i][j];    // Note the transposition
        }
      }
  
    AffineTransformPointer result = AffineTransformType::New();
    result->SetMatrix(matrix);
    result->SetOffset(offset);
  
    AffineTransformPointer inverse = AffineTransformType::New();
    result->GetInverse(inverse);
  
    return inverse;
    }


  virtual void CopyAttributesFrom( const LabelObjectType * lo )
    {
    Superclass::CopyAttributesFrom( lo );

    // copy the data of the current type if possible
    const Self * src = dynamic_cast<const Self *>( lo );
    if( src == NULL )
      {
      return;
      }
    m_Region = src->m_Region;
    m_Size = src->m_Size;
    m_PhysicalSize = src->m_PhysicalSize;
    m_Centroid = src->m_Centroid;
    m_RegionElongation = src->m_RegionElongation;
    m_SizeRegionRatio = src->m_SizeRegionRatio;
    m_SizeOnBorder = src->m_SizeOnBorder;
    m_PhysicalSizeOnBorder = src->m_PhysicalSizeOnBorder;
    m_FeretDiameter = src->m_FeretDiameter;
    m_BinaryPrincipalMoments = src->m_BinaryPrincipalMoments;
    m_BinaryPrincipalAxes = src->m_BinaryPrincipalAxes;
    m_BinaryElongation = src->m_BinaryElongation;
    m_Perimeter = src->m_Perimeter;
    m_Roundness = src->m_Roundness;
    m_EquivalentRadius = src->m_EquivalentRadius;
    m_EquivalentPerimeter = src->m_EquivalentPerimeter;
    m_EquivalentEllipsoidSize = src->m_EquivalentEllipsoidSize;
    m_BinaryFlatness = src->m_BinaryFlatness;
    }

protected:
  ShapeLabelObject()
    {
    m_Size = 0;
    m_PhysicalSize = 0;
    m_Centroid.Fill(0);
    m_RegionElongation = 0;
    m_SizeRegionRatio = 0;
    m_SizeOnBorder = false;
    m_PhysicalSizeOnBorder = 0;
    m_FeretDiameter = false;
    m_BinaryPrincipalMoments.Fill(0);
    m_BinaryPrincipalAxes.Fill(0);
    m_BinaryElongation = 0;
    m_Perimeter = 0;
    m_Roundness = 0;
    m_EquivalentRadius = 0;
    m_EquivalentPerimeter = 0;
    m_EquivalentEllipsoidSize.Fill(0);
    m_BinaryFlatness = 0;
    }
  

  void PrintSelf(std::ostream& os, Indent indent) const
    {
    Superclass::PrintSelf( os, indent );

    os << indent << "Centroid: " << m_Centroid << std::endl;
    os << indent << "Region: ";
    m_Region.Print( os, indent );
    os << indent << "PhysicalSize: " << m_PhysicalSize << std::endl;
    os << indent << "Size: " << m_Size << std::endl;
    os << indent << "RegionElongation: " << m_RegionElongation << std::endl;
    os << indent << "SizeRegionRatio: " << m_SizeRegionRatio << std::endl;
    os << indent << "SizeOnBorder: " << m_SizeOnBorder << std::endl;
    os << indent << "PhysicalSizeOnBorder: " << m_PhysicalSizeOnBorder << std::endl;
    os << indent << "FeretDiameter: " << m_FeretDiameter << std::endl;
    os << indent << "BinaryPrincipalMoments: " << m_BinaryPrincipalMoments << std::endl;
    os << indent << "BinaryPrincipalAxes: " << std::endl << m_BinaryPrincipalAxes;
    os << indent << "BinaryElongation: " << m_BinaryElongation << std::endl;
    os << indent << "Perimeter: " << m_Perimeter << std::endl;
    os << indent << "Roundness: " << m_Roundness << std::endl;
    os << indent << "EquivalentRadius: " << m_EquivalentRadius << std::endl;
    os << indent << "EquivalentPerimeter: " << m_EquivalentPerimeter << std::endl;
    os << indent << "EquivalentEllipsoidSize: " << m_EquivalentEllipsoidSize << std::endl;
    os << indent << "BinaryFlatness: " << m_BinaryElongation << std::endl;
    }

private:
  ShapeLabelObject(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  RegionType    m_Region;
  unsigned long m_Size;
  double        m_PhysicalSize;
  CentroidType  m_Centroid;
  double        m_RegionElongation;
  double        m_SizeRegionRatio;
  unsigned long m_SizeOnBorder;
  double        m_PhysicalSizeOnBorder;
  double        m_FeretDiameter;
  VectorType    m_BinaryPrincipalMoments;
  MatrixType    m_BinaryPrincipalAxes;
  double        m_BinaryElongation;
  double        m_Perimeter;
  double        m_Roundness;
  double        m_EquivalentRadius;
  double        m_EquivalentPerimeter;
  VectorType    m_EquivalentEllipsoidSize;
  double        m_BinaryFlatness;

};

} // end namespace itk

#endif
