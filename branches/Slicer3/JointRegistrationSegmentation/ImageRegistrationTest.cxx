/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageRegistration9.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/31 14:14:16 $
  Version:   $Revision: 1.29 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// 
// This code attempts to register two multi-component images, each representing
// probabilistic memberships to some class. The multi-component images are
// represented by two itk::VectorImage (s). The VectorLength of the image is 
// equal to the number of classes. 
//
// The example is intended for registration of an atlas (with probabilistic 
// class memberships) to a segmentation (also possibly probabilistic).
//
// The example uses the usual components of the registration framework, namely
// an optimizer, transform, metric and interpolator. The metric used is the
// KullbackLeiblerDivergenceImageToImageMetric, which computes distances
// between two classes (represented by VectorImages). Note that the metric does
// not evalueate derivatives. The optimizer is an Amoeba optimizer, which uses 
// a Neadler-Mead simplex to converge on a solution. (Note that the amoeba does
// not need evaluation of derivatives to converge on a solution). The transform
// is a VersorRigid3DTransform (rotations + translations).
//
// The code requires as parameters two input VectorImages (fixed and moving).
// The pixel type of the images is assumed to be float and dimension assumed to
// be 3D.
// 
// A CenteredTransformGeometricInitializer is used to align the geometric centers
// of the images prior to registration.
// 
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageRegistrationMethod2.h"
#include "itkKullbackLeiblerDivergenceImageToImageMetric.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkAmoebaOptimizer.h"
#include "itkCenteredTransformGeometricInitializer.h"
#include "itkVectorImage.h"
#include "itkVersorRigid3DTransform.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//
//  The following piece of code implements an observer
//  that will monitor the evolution of the registration process.
//
#include "itkCommand.h"
class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::AmoebaOptimizer         OptimizerType;
  typedef   const OptimizerType   *    OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    OptimizerPointer optimizer = 
                      dynamic_cast< OptimizerPointer >( object );
    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }
      std::cout << optimizer->GetCachedValue() << "   ";
      std::cout << optimizer->GetCachedCurrentPosition() << std::endl;
  }
};


int main( int argc, char *argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << "   fixedImageFile  movingImageFile " << std::endl;
    std::cerr << "   [maxNumberOfIterations] "<< std::endl;
    return 1;
    }

  const    unsigned int    Dimension = 3;
  typedef  float           PixelType;

  typedef itk::VectorImage< PixelType, Dimension >  FixedImageType;
  typedef itk::VectorImage< PixelType, Dimension >  MovingImageType;
  typedef itk::VersorRigid3DTransform< double >     TransformType;
  typedef itk::AmoebaOptimizer                      OptimizerType;
  typedef itk::NearestNeighborInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;

  typedef itk::ImageRegistrationMethod2< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;

  typedef itk::KullbackLeiblerDivergenceImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );

  TransformType::Pointer  transform = TransformType::New();

  typedef itk::CenteredTransformGeometricInitializer< 
                                    TransformType, 
                                    FixedImageType, 
                                    MovingImageType 
                                                 >  TransformInitializerType;

  TransformInitializerType::Pointer initializer = 
                                          TransformInitializerType::New();

  typedef itk::ImageFileReader< FixedImageType > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;
  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName(  argv[1] );
  movingImageReader->SetFileName( argv[2] );
  
  registration->SetFixedImage(    fixedImageReader->GetOutput()    );
  registration->SetMovingImage(   movingImageReader->GetOutput()   );

  fixedImageReader->Update();

  registration->SetFixedImageRegion( 
     fixedImageReader->GetOutput()->GetBufferedRegion() );

  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedImageReader->GetOutput() );
  initializer->SetMovingImage( movingImageReader->GetOutput() );

  initializer->InitializeTransform();

  registration->SetTransform( transform );
  registration->SetInitialTransformParameters( 
                                 transform->GetParameters() );

  double translationScale = 1.0 / 10.0;

  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );

  optimizerScales[0] =  1.0;
  optimizerScales[1] =  1.0;
  optimizerScales[2] =  1.0;
  optimizerScales[3] =  translationScale;
  optimizerScales[4] =  translationScale;
  optimizerScales[5] =  translationScale;

  optimizer->SetScales( optimizerScales );
  optimizer->SetParametersConvergenceTolerance( 0.00001 ); // reasonable defaults
  optimizer->SetFunctionConvergenceTolerance( 0.0001 );

  double steplength = 0.1;

  unsigned int maxNumberOfIterations = 1500;

  if( argc > 3 )
    {
    maxNumberOfIterations = atoi( argv[3] );
    }

  optimizer->SetMaximumNumberOfIterations( maxNumberOfIterations );
  optimizer->MinimizeOn();

  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  try 
    { 
    std::cout << "Starting registration " << std::endl;
    registration->StartRegistration(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return -1;
    } 

  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();

  // set some output information,
  char results[1024];
  typedef TransformType::VersorType VersorType;
  VersorType versor = transform->GetVersor();
  TransformType::OffsetType offset = transform->GetOffset();
  typedef VersorType::VectorType   AxisType;
  AxisType axis = versor.GetAxis();

  sprintf(results,"Translation: %g %g %g\nRotation Axis %f %f %f %f\nOffset: %g %g %g", 
          finalParameters[3],
          finalParameters[4],
          finalParameters[5],
          axis[0],
          axis[1],
          axis[2],
          versor.GetAngle(),
          offset[0],
          offset[1],
          offset[2]
          );

  std::cout << results << std::endl;
  
  return EXIT_SUCCESS;
}

