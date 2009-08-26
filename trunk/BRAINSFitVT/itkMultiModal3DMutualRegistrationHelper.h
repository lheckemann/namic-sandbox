/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile$
Language:  C++

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMultiModal3DMutualRegistrationHelper_h
#define __itkMultiModal3DMutualRegistrationHelper_h
#if defined( _MSC_VER )
#pragma warning ( disable : 4786 )
#endif

#include "itkImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"

#include "itkDataObjectDecorator.h"

#include "itkCenteredTransformInitializer.h"

#include <fstream>
#include <string>

#include "itkMultiThreader.h"
#include "itkResampleImageFilter.h"

#include <stdio.h>

enum {
  Dimension = 3,
  MaxInputDimension = 4
  };

//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//
#include "itkCommand.h"
template <typename TOptimizer>
class CommandIterationUpdate : public itk::Command
  {
public:
  typedef  CommandIterationUpdate Self;
  typedef  itk::Command           Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate()
             {}
public:
  typedef          TOptimizer OptimizerType;
  typedef const OptimizerType *OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event );
  }

  void Execute(const itk::Object *object, const itk::EventObject & event)
  {
    OptimizerPointer optimizer = dynamic_cast<OptimizerPointer>( object );

    if ( optimizer == NULL )
      {
      std::cout << "ERROR::" << __FILE__ << " " << __LINE__ << std::endl;
      exit(-1);
      }
    if ( !itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }
    std::cout << optimizer->GetCurrentIteration() << "   ";
    std::cout << optimizer->GetValue() << "   ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
  }
  };

namespace itk
{
template <typename TTransformType, typename TOptimizer, typename TFixedImage,
  typename TMovingImage>
class ITK_EXPORT MultiModal3DMutualRegistrationHelper : public ProcessObject
  {
public:
  /** Standard class typedefs. */
  typedef MultiModal3DMutualRegistrationHelper Self;
  typedef ProcessObject                        Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>             ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MultiModal3DMutualRegistrationHelper, ProcessObject);

  typedef          TFixedImage                   FixedImageType;
  typedef typename FixedImageType::ConstPointer  FixedImageConstPointer;
  typedef typename FixedImageType::Pointer       FixedImagePointer;

  typedef          TMovingImage                  MovingImageType;
  typedef typename MovingImageType::ConstPointer MovingImageConstPointer;
  typedef typename MovingImageType::Pointer      MovingImagePointer;

  typedef          TTransformType                TransformType;
  typedef typename TransformType::Pointer        TransformPointer;

  /** Type for the output: Using Decorator pattern for enabling
   *  the Transform to be passed in the data pipeline */
  typedef DataObjectDecorator<TransformType>    TransformOutputType;
  typedef typename TransformOutputType::Pointer TransformOutputPointer;
  typedef typename TransformOutputType::ConstPointer
  TransformOutputConstPointer;

  typedef          TOptimizer OptimizerType;
  typedef const OptimizerType *OptimizerPointer;
  typedef typename OptimizerType::ScalesType OptimizerScalesType;

  typedef MattesMutualInformationImageToImageMetric<
    FixedImageType,
    MovingImageType>    MetricType;

  typedef typename MetricType::FixedImageMaskType  FixedBinaryVolumeType;
  typedef typename FixedBinaryVolumeType::Pointer  FixedBinaryVolumePointer;
  typedef typename MetricType::MovingImageMaskType MovingBinaryVolumeType;
  typedef typename MovingBinaryVolumeType::Pointer MovingBinaryVolumePointer;

  typedef LinearInterpolateImageFunction<
    MovingImageType,
    double>    InterpolatorType;

  typedef ImageRegistrationMethod<
    FixedImageType,
    MovingImageType>    RegistrationType;
  typedef typename RegistrationType::Pointer RegistrationPointer;

  typedef itk::CenteredTransformInitializer<
    TransformType,
    FixedImageType,
    MovingImageType>    TransformInitializerType;

  typedef itk::ResampleImageFilter<
    MovingImageType,
    FixedImageType>     ResampleFilterType;

  /** Initialize by setting the interconnects between the components. */
  virtual void Initialize(const int NumParam)
    throw ( ExceptionObject );

  /** Method that initiates the registration. */
  void StartRegistration(void);

  /** Set/Get the Fixed image. */
  void SetFixedImage( FixedImagePointer fixedImage );

  itkGetConstObjectMacro( FixedImage, FixedImageType );

  /** Set/Get the Moving image. */
  void SetMovingImage( MovingImagePointer movingImage );

  itkGetConstObjectMacro( MovingImage, MovingImageType );

  void SetFixedBinaryVolume( FixedBinaryVolumePointer fixedImageMask )
  {
    m_FixedBinaryVolume = fixedImageMask;
  }

  itkGetConstObjectMacro( FixedBinaryVolume, FixedBinaryVolumeType );

  void SetMovingBinaryVolume( MovingBinaryVolumePointer movingImageMask )
  {
    m_MovingBinaryVolume = movingImageMask;
  }

  itkGetConstObjectMacro( MovingBinaryVolume, MovingBinaryVolumeType );

  /** Set/Get the InitialTransfrom. */
  void SetInitialTransform( typename TransformType::Pointer initialTransform );
  itkGetConstObjectMacro( InitialTransform, TransformType );

  /** Set/Get the Transfrom. */
  itkSetObjectMacro( Transform, TransformType );
  typename TransformType::Pointer GetTransform(void);

  typedef enum {
    LINEAR_INTERP = 0,
    WINDOWSINC_INTERP = 1,
    } InterpolationType;
  typename FixedImageType::Pointer GetResampledImage(
    InterpolationType newInterp) const;

  //itkSetMacro( PermissionToVaryGuide, std::vector<int>      );

  itkSetMacro( NumberOfSpatialSamples,        unsigned int );
  itkSetMacro( NumberOfHistogramBins,         unsigned int );
  itkSetMacro( NumberOfIterations,            unsigned int );
  itkSetMacro( RelaxationFactor,              double        );
  itkSetMacro( MaximumStepLength,             double        );
  itkSetMacro( MinimumStepLength,             double        );
  itkSetMacro( TranslationScale,              double        );
  itkSetMacro( ReproportionScale,             double        );
  itkSetMacro( SkewScale,                     double        );
  itkSetMacro( InitialTransformPassThruFlag,  bool           );
  itkSetMacro( UseExplicitPDFDerivativesFlag, bool           );
  itkSetMacro( BackgroundFillValue,           double        );

  itkGetMacro( ActualNumberOfIterations,      unsigned int  );

  /** Returns the transform resulting from the registration process  */
  const TransformOutputType * GetOutput() const;

  /** Make a DataObject of the correct type to be used as the specified
   * output. */
  virtual DataObjectPointer MakeOutput(unsigned int idx);

  /** Method to return the latest modified time of this object or
   * any of its cached ivars */
  unsigned long GetMTime() const;

  /** Method to set the Permission to vary by level  */
  void SetPermissionToVaryGuide(std::vector<int> perms)
  {
    m_PermissionToVaryGuide.resize(perms.size());
    for (unsigned int i=0;i<perms.size();i++) 
    {
      m_PermissionToVaryGuide[i] = perms[i];
    }
  }

protected:
  MultiModal3DMutualRegistrationHelper();
  virtual ~MultiModal3DMutualRegistrationHelper()
               {};
  void PrintSelf(std::ostream & os, Indent indent) const;

  /** Method invoked by the pipeline in order to trigger the computation of
   * the registration. */
  void  GenerateData ();

private:
  MultiModal3DMutualRegistrationHelper(const Self &);  // purposely not
                                                       // implemented
  void operator=(const Self &);                        // purposely not

  // implemented

  // FixedImageConstPointer           m_FixedImage;
  // MovingImageConstPointer          m_MovingImage;

  FixedImagePointer  m_FixedImage;
  MovingImagePointer m_MovingImage;

  FixedBinaryVolumePointer  m_FixedBinaryVolume;
  MovingBinaryVolumePointer m_MovingBinaryVolume;
  TransformPointer          m_InitialTransform;
  TransformPointer          m_Transform;

  RegistrationPointer m_Registration;

  std::vector<int> m_PermissionToVaryGuide;

  unsigned int m_NumberOfSpatialSamples;
  unsigned int m_NumberOfHistogramBins;
  unsigned int m_NumberOfIterations;
  double       m_RelaxationFactor;
  double       m_MaximumStepLength;
  double       m_MinimumStepLength;
  double       m_TranslationScale;
  double       m_ReproportionScale;
  double       m_SkewScale;
  bool          m_InitialTransformPassThruFlag;
  bool          m_UseExplicitPDFDerivativesFlag;
  double       m_BackgroundFillValue;
  unsigned int m_NumberOfOptimizationParameters;
  unsigned int m_ActualNumberOfIterations;
  };
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiModal3DMutualRegistrationHelper.txx"
#endif

#endif
