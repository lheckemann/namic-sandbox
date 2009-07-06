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
#ifndef __itkStatisticsLabelObject_h
#define __itkStatisticsLabelObject_h

#include "itkShapeLabelObject.h"
#include "itkHistogram.h"

namespace itk
{


namespace Functor {

template< class TLabelObject >
class ITK_EXPORT MinimumLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetMinimum();
    }
};

template< class TLabelObject >
class ITK_EXPORT MaximumLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetMaximum();
    }
};

template< class TLabelObject >
class ITK_EXPORT MeanLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetMean();
    }
};

template< class TLabelObject >
class ITK_EXPORT SumLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetSum();
    }
};

template< class TLabelObject >
class ITK_EXPORT SigmaLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetSigma();
    }
};

template< class TLabelObject >
class ITK_EXPORT VarianceLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetVariance();
    }
};

template< class TLabelObject >
class ITK_EXPORT MedianLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetMedian();
    }
};

template< class TLabelObject >
class ITK_EXPORT MaximumIndexLabelObjectAccessor
{
public:
  typedef TLabelObject                        LabelObjectType;
  typedef typename LabelObjectType::IndexType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetMaximumIndex();
    }
};

template< class TLabelObject >
class ITK_EXPORT MinimumIndexLabelObjectAccessor
{
public:
  typedef TLabelObject                        LabelObjectType;
  typedef typename LabelObjectType::IndexType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetMinimumIndex();
    }
};

template< class TLabelObject >
class ITK_EXPORT CenterOfGravityLabelObjectAccessor
{
public:
  typedef TLabelObject                        LabelObjectType;
  typedef typename LabelObjectType::PointType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetCenterOfGravity();
    }
};


/*template< class TLabelObject >
class ITK_EXPORT CentralMomentsLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef typename LabelObjectType::MatrixType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetCentralMoments();
    }
};*/

template< class TLabelObject >
class ITK_EXPORT PrincipalMomentsLabelObjectAccessor
{
public:
  typedef TLabelObject                         LabelObjectType;
  typedef typename LabelObjectType::VectorType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetPrincipalMoments();
    }
};

template< class TLabelObject >
class ITK_EXPORT PrincipalAxesLabelObjectAccessor
{
public:
  typedef TLabelObject                         LabelObjectType;
  typedef typename LabelObjectType::MatrixType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetPrincipalAxes();
    }
};

template< class TLabelObject >
class ITK_EXPORT KurtosisLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetKurtosis();
    }
};

template< class TLabelObject >
class ITK_EXPORT SkewnessLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetSkewness();
    }
};

template< class TLabelObject >
class ITK_EXPORT ElongationLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetElongation();
    }
};

template< class TLabelObject >
class ITK_EXPORT HistogramLabelObjectAccessor
{
public:
  typedef TLabelObject                              LabelObjectType;
  typedef typename LabelObjectType::HistogramType * AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetHistogram();
    }
};

template< class TLabelObject >
class ITK_EXPORT FlatnessLabelObjectAccessor
{
public:
  typedef TLabelObject LabelObjectType;
  typedef double       AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetFlatness();
    }
};

}


/** \class StatisticsLabelObject
 *  \brief A Label object to store the common attributes related to the statistics of the object
 *
 * StatisticsLabelObject stores  the common attributes related to the statistics of the object
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * \ingroup DataRepresentation 
 */
template < class TLabel, unsigned int VImageDimension >
class ITK_EXPORT StatisticsLabelObject : public ShapeLabelObject< TLabel, VImageDimension >
{
public:
  /** Standard class typedefs */
  typedef StatisticsLabelObject                       Self;
  typedef ShapeLabelObject< TLabel, VImageDimension > Superclass;
  typedef typename Superclass::LabelObjectType        LabelObjectType;
  typedef SmartPointer<Self>                          Pointer;
  typedef SmartPointer<const Self>                    ConstPointer;
  typedef WeakPointer<const Self>                     ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(StatisticsLabelObject, LabelObject);

  typedef LabelMap< Self > LabelMapType;

  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

  typedef typename Superclass::IndexType IndexType;

  typedef Point< double, ImageDimension > PointType;

  typedef TLabel LabelType;

  typedef typename Superclass::LineType LineType;

  typedef typename Superclass::LengthType LengthType;

  typedef typename Superclass::LineContainerType LineContainerType;

  typedef Matrix< double, ImageDimension, ImageDimension >   MatrixType;

  typedef Vector< double, ImageDimension > VectorType;

  typedef typename itk::Statistics::Histogram< double > HistogramType;

  typedef typename Superclass::AttributeType AttributeType;
  static const AttributeType MINIMUM=200;
  static const AttributeType MAXIMUM=201;
  static const AttributeType MEAN=202;
  static const AttributeType SUM=203;
  static const AttributeType SIGMA=204;
  static const AttributeType VARIANCE=205;
  static const AttributeType MEDIAN=206;
  static const AttributeType MAXIMUM_INDEX=207;
  static const AttributeType MINIMUM_INDEX=208;
  static const AttributeType CENTER_OF_GRAVITY=209;
//  static const AttributeType CENTRAL_MOMENTS=210;
  static const AttributeType PRINCIPAL_MOMENTS=211;
  static const AttributeType PRINCIPAL_AXES=212;
  static const AttributeType KURTOSIS=213;
  static const AttributeType SKEWNESS=214;
  static const AttributeType ELONGATION=215;
  static const AttributeType HISTOGRAM=216;
  static const AttributeType FLATNESS=217;

  static AttributeType GetAttributeFromName( const std::string & s )
    {
    if( s == "Minimum" )
      {
      return MINIMUM;
      }
    else if( s == "Maximum" )
      {
      return MAXIMUM;
      }
    else if( s == "Mean" )
      {
      return MEAN;
      }
    else if( s == "Sum" )
      {
      return SUM;
       }
    else if( s == "Sigma" )
      {
      return SIGMA;
      }
    else if( s == "Variance" )
      {
      return VARIANCE;
      }
    else if( s == "Median" )
      {
      return MEDIAN;
      }
    else if( s == "MaximumIndex" )
      {
      return MAXIMUM_INDEX;
      }
    else if( s == "MinimumIndex" )
      {
      return MINIMUM_INDEX;
      }
    else if( s == "CenterOfGravity" )
      {
      return CENTER_OF_GRAVITY;
      }
/*    else if( s == "CentralMoments" )
      {
      return CENTRAL_MOMENTS;
      }*/
    else if( s == "PrincipalMoments" )
      {
      return PRINCIPAL_MOMENTS;
      }
    else if( s == "PrincipalAxes" )
      {
      return PRINCIPAL_AXES;
      }
    else if( s == "Kurtosis" )
      {
      return KURTOSIS;
      }
    else if( s == "Skewness" )
      {
      return SKEWNESS;
      }
    else if( s == "Elongation" )
      {
      return ELONGATION;
      }
    else if( s == "Histogram" )
      {
      return HISTOGRAM;
      }
    else if( s == "Flatness" )
      {
      return FLATNESS;
      }
    // can't recognize the name
    return Superclass::GetAttributeFromName( s );
    }

  static std::string GetNameFromAttribute( const AttributeType & a )
    {
    switch( a )
      {
      case MINIMUM:
        return "Minimum";
        break;
      case MAXIMUM:
        return "Maximum";
        break;
      case MEAN:
        return "Mean";
        break;
      case SUM:
        return "Sum";
        break;
      case SIGMA:
        return "Sigma";
        break;
      case VARIANCE:
        return "Variance";
        break;
      case MEDIAN:
        return "Median";
        break;
      case MAXIMUM_INDEX:
        return "MaximumIndex";
        break;
      case MINIMUM_INDEX:
        return "MinimumIndex";
        break;
      case CENTER_OF_GRAVITY:
        return "CenterOfGravity";
        break;
/*      case CENTRAL_MOMENTS:
        return "CentralMoments";
        break;*/
      case PRINCIPAL_MOMENTS:
        return "PrincipalMoments";
        break;
      case PRINCIPAL_AXES:
        return "PrincipalAxes";
        break;
      case KURTOSIS:
        return "Kurtosis";
        break;
      case SKEWNESS:
        return "Skewness";
        break;
      case ELONGATION:
        return "Elongation";
        break;
      case HISTOGRAM:
        return "Histogram";
        break;
      case FLATNESS:
        return "Flatness";
        break;
      }
    // can't recognize the name
    return Superclass::GetNameFromAttribute( a );
    }

  typedef ImageRegion< ImageDimension > RegionType;

  typedef typename Superclass::CentroidType CentroidType;


  virtual void CopyAttributesFrom( const LabelObjectType * lo )
    {
    Superclass::CopyAttributesFrom( lo );

    // copy the data of the current type if possible
    const Self * src = dynamic_cast<const Self *>( lo );
    if( src == NULL )
      {
      return;
      }
    m_Minimum = src->m_Minimum;
    m_Maximum = src->m_Maximum;
    m_Mean = src->m_Mean;
    m_Sum = src->m_Sum;
    m_Sigma = src->m_Sigma;
    m_Variance = src->m_Variance;
    m_Median = src->m_Median;
    m_MaximumIndex = src->m_MaximumIndex;
    m_MinimumIndex = src->m_MinimumIndex;
    m_CenterOfGravity = src->m_CenterOfGravity;
//    m_CentralMoments = src->m_CentralMoments;
    m_PrincipalMoments = src->m_PrincipalMoments;
    m_PrincipalAxes = src->m_PrincipalAxes;
    m_Kurtosis = src->m_Kurtosis;
    m_Skewness = src->m_Skewness;
    m_Elongation = src->m_Elongation;
    m_Histogram = src->m_Histogram;
    m_Flatness = src->m_Flatness;
    }

//   itkGetConstMacro( Minimum, double );
//   itkSetMacro( Minimum, double );
  const double & GetMinimum() const
    {
    return m_Minimum;
    }

  void SetMinimum( const double & v )
    {
    m_Minimum = v;
    }

//   itkGetConstMacro( Maximum, double );
//   itkSetMacro( Maximum, double );
  const double & GetMaximum() const
    {
    return m_Maximum;
    }

  void SetMaximum( const double & v )
    {
    m_Maximum = v;
    }

//   itkGetConstMacro( Mean, double );
//   itkSetMacro( Mean, double );
  const double & GetMean() const
    {
    return m_Mean;
    }

  void SetMean( const double & v )
    {
    m_Mean = v;
    }

//   itkGetConstMacro( Sum, double );
//   itkSetMacro( Sum, double );
  const double & GetSum() const
    {
    return m_Sum;
    }

  void SetSum( const double & v )
    {
    m_Sum = v;
    }

//   itkGetConstMacro( Sigma, double );
//   itkSetMacro( Sigma, double );
  const double & GetSigma() const
    {
    return m_Sigma;
    }

  void SetSigma( const double & v )
    {
    m_Sigma = v;
    }

//   itkGetConstMacro( Variance, double );
//   itkSetMacro( Variance, double );
  const double & GetVariance() const
    {
    return m_Variance;
    }

  void SetVariance( const double & v )
    {
    m_Variance = v;
    }

//   itkGetConstMacro( Median, double );
//   itkSetMacro( Median, double );
  const double & GetMedian() const
    {
    return m_Median;
    }

  void SetMedian( const double & v )
    {
    m_Median = v;
    }

//   itkGetConstMacro( MaximumIndex, IndexType );
//   itkSetMacro( MaximumIndex, IndexType );
  const IndexType & GetMaximumIndex() const
    {
    return m_MaximumIndex;
    }

  void SetMaximumIndex( const IndexType & v )
    {
    m_MaximumIndex = v;
    }

//   itkGetConstMacro( MinimumIndex, IndexType );
//   itkSetMacro( MinimumIndex, IndexType );
  const IndexType & GetMinimumIndex() const
    {
    return m_MinimumIndex;
    }

  void SetMinimumIndex( const IndexType & v )
    {
    m_MinimumIndex = v;
    }

//   itkGetConstMacro( CenterOfGravity, PointType );
//   itkSetMacro( CenterOfGravity, PointType );
  const PointType & GetCenterOfGravity() const
    {
    return m_CenterOfGravity;
    }

  void SetCenterOfGravity( const PointType & v )
    {
    m_CenterOfGravity = v;
    }

//   itkGetConstMacro( CentralMoments, MatrixType );
//   itkSetMacro( CentralMoments, MatrixType );
/*  const MatrixType & GetCentralMoments() const
    {
    return m_CentralMoments;
    }

  void SetCentralMoments( const MatrixType & v )
    {
    m_CentralMoments = v;
    }*/

//   itkGetConstMacro( PrincipalMoments, VectorType );
//   itkSetMacro( PrincipalMoments, VectorType );
  const VectorType & GetPrincipalMoments() const
    {
    return m_PrincipalMoments;
    }

  void SetPrincipalMoments( const VectorType & v )
    {
    m_PrincipalMoments = v;
    }

//   itkGetConstMacro( PrincipalAxes, MatrixType );
//   itkSetMacro( PrincipalAxes, MatrixType );
  const MatrixType & GetPrincipalAxes() const
    {
    return m_PrincipalAxes;
    }

  void SetPrincipalAxes( const MatrixType & v )
    {
    m_PrincipalAxes = v;
    }

//   itkGetConstMacro( Skewness, double );
//   itkSetMacro( Skewness, double );
  const double & GetSkewness() const
    {
    return m_Skewness;
    }

  void SetSkewness( const double & v )
    {
    m_Skewness = v;
    }

//   itkGetConstMacro( Kurtosis, double );
//   itkSetMacro( Kurtosis, double );
  const double & GetKurtosis() const
    {
    return m_Kurtosis;
    }

  void SetKurtosis( const double & v )
    {
    m_Kurtosis = v;
    }

//   itkGetConstMacro( Elongation, double );
//   itkSetMacro( Elongation, double );
  const double & GetElongation() const
    {
    return m_Elongation;
    }

  void SetElongation( const double & v )
    {
    m_Elongation = v;
    }

//   itkGetConstMacro( Histogram, double );
//   itkSetMacro( Histogram, double );
  const HistogramType * GetHistogram() const
    {
    return m_Histogram;
    }

  void SetHistogram( const HistogramType * v )
    {
    m_Histogram = v;
    }

//   itkGetConstMacro( Flatness, double );
//   itkSetMacro( Flatness, double );
  const double & GetFlatness() const
    {
    return m_Flatness;
    }

  void SetFlatness( const double & v )
    {
    m_Flatness = v;
    }


// some helper methods - not really required, but really useful!

  /** Affine transform for mapping to and from principal axis */
  typedef AffineTransform<double,itkGetStaticConstMacro(ImageDimension)> AffineTransformType;
  typedef typename AffineTransformType::Pointer      AffineTransformPointer;

  /** Get the affine transform from principal axes to physical axes
   * This method returns an affine transform which transforms from
   * the principal axes coordinate system to physical coordinates. */
  AffineTransformPointer GetPrincipalAxesToPhysicalAxesTransform() const
    {
    typename AffineTransformType::MatrixType matrix;
    typename AffineTransformType::OffsetType offset;
    for (unsigned int i = 0; i < ImageDimension; i++) 
      {
      offset[i]  = m_CenterOfGravity[i];
      for (unsigned int j = 0; j < ImageDimension; j++)
        {
        matrix[j][i] = m_PrincipalAxes[i][j];    // Note the transposition
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
  AffineTransformPointer GetPhysicalAxesToPrincipalAxesTransform(void) const
    {
    typename AffineTransformType::MatrixType matrix;
    typename AffineTransformType::OffsetType offset;
    for (unsigned int i = 0; i < ImageDimension; i++) 
      {
      offset[i]    = m_CenterOfGravity[i];
      for (unsigned int j = 0; j < ImageDimension; j++)
        {
        matrix[j][i] = m_PrincipalAxes[i][j];    // Note the transposition
        }
      }
  
    AffineTransformPointer result = AffineTransformType::New();
    result->SetMatrix(matrix);
    result->SetOffset(offset);
  
    AffineTransformPointer inverse = AffineTransformType::New();
    result->GetInverse(inverse);
  
    return inverse;
    }


protected:
  StatisticsLabelObject()
    {
    m_Minimum = 0;
    m_Maximum = 0;
    m_Mean = 0;
    m_Sum = 0;
    m_Sigma = 0;
    m_Variance = 0;
    m_Median = 0;
    m_MaximumIndex.Fill(0);
    m_MinimumIndex.Fill(0);
    m_CenterOfGravity.Fill(0);
//    m_CentralMoments.Fill(0);
    m_PrincipalMoments.Fill(0);
    m_PrincipalAxes.Fill(0);
    m_Kurtosis = 0;
    m_Skewness = 0;
    m_Elongation = 0;
    m_Histogram = NULL;
    m_Flatness = 0;
    }
  

  void PrintSelf(std::ostream& os, Indent indent) const
    {
    Superclass::PrintSelf( os, indent );

    os << indent << "Minimum: " << m_Minimum << std::endl;
    os << indent << "Maximum: " << m_Maximum << std::endl;
    os << indent << "Mean: " << m_Mean << std::endl;
    os << indent << "Sum: " << m_Sum << std::endl;
    os << indent << "Sigma: " << m_Sigma << std::endl;
    os << indent << "Variance: " << m_Variance << std::endl;
    os << indent << "Median: " << m_Median << std::endl;
    os << indent << "MaximumIndex: " << m_MaximumIndex << std::endl;
    os << indent << "MinimumIndex: " << m_MinimumIndex << std::endl;
    os << indent << "CenterOfGravity: " << m_CenterOfGravity << std::endl;
//    os << indent << "CentralMoments: " << std::endl << m_CentralMoments;
    os << indent << "PrincipalMoments: " << m_PrincipalMoments << std::endl;
    os << indent << "PrincipalAxes: " << std::endl << m_PrincipalAxes;
    os << indent << "Skewness: " << m_Skewness << std::endl;
    os << indent << "Kurtosis: " << m_Kurtosis << std::endl;
    os << indent << "Elongation: " << m_Elongation << std::endl;
    os << indent << "Histogram: ";
    if( m_Histogram.IsNull() )
      {
      os << "NULL" << std::endl;
      }
    else
      {
      m_Histogram->Print( os, indent );
      }
    os << indent << "Flatness: " << m_Flatness << std::endl;
    }

private:
  StatisticsLabelObject(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  double                               m_Minimum;
  double                               m_Maximum;
  double                               m_Mean;
  double                               m_Sum;
  double                               m_Sigma;
  double                               m_Variance;
  double                               m_Median;
  IndexType                            m_MaximumIndex;
  IndexType                            m_MinimumIndex;
  PointType                            m_CenterOfGravity;
//  MatrixType m_CentralMoments;
  VectorType                           m_PrincipalMoments;
  MatrixType                           m_PrincipalAxes;
  double                               m_Skewness;
  double                               m_Kurtosis;
  double                               m_Elongation;
  typename HistogramType::ConstPointer m_Histogram;
  double                               m_Flatness;

};

} // end namespace itk

#endif
