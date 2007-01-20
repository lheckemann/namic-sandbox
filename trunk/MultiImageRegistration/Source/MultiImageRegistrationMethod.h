/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCongealingRegistrationMethod.h,v $
  Language:  C++
  Date:      $Date: 2005/11/01 21:57:22 $
  Version:   $Revision: 1.19 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __MultiImageRegistration_h
#define __MultiImageRegistration_h

#include "itkProcessObject.h"
#include "itkImage.h"
#include "itkImageToImageMetric.h"
#include "itkSingleValuedNonLinearOptimizer.h"
#include "itkDataObjectDecorator.h"

// User defined headers
#include "VarianceMultiImageMetric.h"
#include "UserMacro.h"
#include <vector>

namespace itk
{

/** \class CongealingRegistrationMethod
 * \brief Base class for Image Registration Methods
 *
 * This Class define the generic interface for a registration method.
 *
 * This class is templated over the type of the two image to be
 * registered. A generic transform is used by this class. That allows
 * to select at run time the particular type of transformation that
 * is to be applied for registering the images.
 *
 * This method use a generic Metric in order to compare the two images.
 * the final goal of the registration method is to find the set of 
 * parameters of the Transformation that optimizes the metric.
 *
 * The registration method also support a generic optimizer that can
 * be selected at run-time. The only restriction for the optimizer is
 * that it should be able to operate in single-valued cost functions
 * given that the metrics used to compare images provide a single 
 * value as output.
 *
 * The terms : Fixed image and Moving image are used in this class
 * to indicate what image is being mapped by the transform.
 *
 * This class uses the coordinate system of the Fixed image as a reference
 * and searchs for a Transform that will map points from the space of the
 * Fixed image to the space of the Moving image.
 *
 * For doing so, a Metric will be continously applied to compare the Fixed
 * image with the Transformed Moving image. This process also requires to
 * interpolate values from the Moving image.
 *
 * \ingroup RegistrationFilters
 */
template <typename TImage >
class ITK_EXPORT MultiImageRegistrationMethod : public ProcessObject 
{
public:
  /** Standard class typedefs. */
  typedef MultiImageRegistrationMethod  Self;
  typedef ProcessObject  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(CongealingRegistrationMethod, ProcessObject);

  /**  Type of the Fixed image. */
  typedef          TImage                     ImageType;
  typedef typename ImageType::ConstPointer    ImageConstPointer;
  typedef        vector<ImageConstPointer>    ImageArrayPointer;



  /**  Type of the metric. */
  typedef MultiImageMetric< ImageType>       MetricType;
  typedef typename MetricType::Pointer                MetricPointer;
  typedef typename MetricType::FixedImageRegionType   FixedImageRegionType;

  /**  Type of the Transform . */
  typedef  typename MetricType::TransformType      TransformType;
  typedef  typename TransformType::Pointer         TransformPointer;
  typedef  vector<TransformPointer>                TransformPointerArray;

  /** Type for the output: Using Decorator pattern for enabling
   *  the Transform to be passed in the data pipeline */
  typedef  DataObjectDecorator< TransformType >    TransformOutputType;
  typedef typename TransformOutputType::Pointer    TransformOutputPointer;
  typedef typename TransformOutputType::ConstPointer    TransformOutputConstPointer;
  
  /**  Type of the Interpolator. */
  typedef  typename MetricType::InterpolatorType   InterpolatorType;
  typedef  typename InterpolatorType::Pointer      InterpolatorPointer;
  typedef  vector<InterpolatorPointer>             InterpolatorPointerArray;

  /**  Type of the optimizer. */
  typedef   SingleValuedNonLinearOptimizer         OptimizerType;

  /** Type of the Transformation parameters This is the same type used to
   *  represent the search space of the optimization algorithm */
  typedef  typename MetricType::TransformParametersType    ParametersType;

  /** Smart Pointer type to a DataObject. */
  typedef typename DataObject::Pointer DataObjectPointer;

  /** Method that initiates the registration. This will Initialize and ensure
   * that all inputs the registration needs are in place, via a call to 
   * Initialize() will then start the optimization process via a call to 
   * StartOptimization()  */
  void StartRegistration(void);

  /** Method that initiates the optimization process. */
  void StartOptimization(void);

  /** Set/Get the Fixed image. */
  void SetImageArrayPointer( const ImageType * fixedImage, int i );
  UserGetConstObjectMacro( ImageArrayPointer, ImageType );

  /** Set/Get the Optimizer. */
  itkSetObjectMacro( Optimizer,  OptimizerType );
  itkGetObjectMacro( Optimizer,  OptimizerType );

  /** Set/Get the Metric. */
  itkSetObjectMacro( Metric, MetricType );
  itkGetObjectMacro( Metric, MetricType );

  /** Set/Get the Transfrom. */
  UserSetObjectMacro( TransformArray, TransformType );
  UserGetObjectMacro( TransformArray, TransformType );

  /** Set/Get the Interpolator. */
  UserSetObjectMacro( InterpolatorArray, InterpolatorType );
  UserGetObjectMacro( InterpolatorArray, InterpolatorType );

  /** Set/Get the length of the parameters vector. The length of the parameters
  array should be set before assigning individual parameters */
  virtual void SetTransformParametersLength( int N );
  virtual int GetTransformParametersLength( int N );

  /** Set/Get the initial transformation parameters. */
  virtual void SetInitialTransformParameters( const ParametersType & param , int i );
  virtual void SetInitialTransformParameters( const ParametersType & param );
  itkGetConstReferenceMacro( InitialTransformParameters, ParametersType );

  /** Get the last transformation parameters visited by 
   * the optimizer. */
  itkGetConstReferenceMacro( LastTransformParameters, ParametersType );

  /** Set the region of the fixed image to be considered as region of
   interest during the registration. This region will be passed to 
   the ImageMetric in order to restrict the metric computation to 
   consider only this region. 
   \warning The same region can also be set directly into the metric.
   please avoid to set the region in both places since this can lead
   to inconsistent configurations.  */
  void SetFixedImageRegion( const  FixedImageRegionType & region );
  /** Get the region of the fixed image to be considered as region of
   interest during the registration. This region will be passed to 
   the ImageMetric in order to restrict the metric computation to 
   consider only this region.  */
  itkGetConstReferenceMacro( FixedImageRegion, FixedImageRegionType );
  /** True if a region has been defined for the fixed image to which
   the ImageMetric will limit its computation */
  itkGetMacro( FixedImageRegionDefined, bool );
  /** Turn on/off the use of a fixed image region to which
   the ImageMetric will limit its computation.
   \warning The region must have been previously defined using the
   SetFixedImageRegion member function */
  itkSetMacro( FixedImageRegionDefined, bool );

  /** Initialize by setting the interconnects between the components. */
  virtual void Initialize() throw (ExceptionObject);

  /** Returns the transform resulting from the registration process  */
  const TransformOutputType * GetOutput() const;

  /** Make a DataObject of the correct type to be used as the specified
   * output. */
  virtual DataObjectPointer MakeOutput(unsigned int idx);

  /** Method to return the latest modified time of this object or
   * any of its cached ivars */
  unsigned long GetMTime() const;  

  /** Set the number of images */
  void SetNumberOfImages(int N);
    
protected:
  MultiImageRegistrationMethod();
  virtual ~MultiImageRegistrationMethod() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Method invoked by the pipeline in order to trigger the computation of 
   * the registration. */
  void  GenerateData ();

  /** Provides derived classes with the ability to set this private var */
  itkSetMacro( LastTransformParameters, ParametersType );


private:
  MultiImageRegistrationMethod(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  MetricPointer                    m_Metric;
  OptimizerType::Pointer           m_Optimizer;

  ImageArrayPointer                m_ImageArrayPointer;

  TransformPointerArray            m_TransformArray;
  InterpolatorPointerArray         m_InterpolatorArray;

  ParametersType                   m_InitialTransformParameters;
  ParametersType                   m_LastTransformParameters;
  
  bool                             m_FixedImageRegionDefined;
  FixedImageRegionType             m_FixedImageRegion;

  int            m_NumberOfImages;

};


} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "MultiImageRegistrationMethod.cxx"
#endif

#endif



