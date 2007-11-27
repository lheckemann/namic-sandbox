/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ModelToImageRegistration1.cxx,v $
  Language:  C++
  Date:      $Date: 2007/09/07 14:17:42 $
  Version:   $Revision: 1.41 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkEllipseSpatialObject.h"
#include "itkImageToSpatialObjectMetric.h"
#include "itkImageToSpatialObjectRegistrationMethod.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkTranslationTransform.h"
#include "itkAmoebaOptimizer.h"

#include "itkImageFileReader.h"

#include "itkCommand.h"


template < class TOptimizer >
class IterationCallback : public itk::Command 
{
public:
  typedef IterationCallback   Self;
  typedef itk::Command  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;
  
  itkTypeMacro( IterationCallback, Superclass );
  itkNewMacro( Self );

  /** Type defining the optimizer. */
  typedef    TOptimizer     OptimizerType;

  /** Method to specify the optimizer. */
  void SetOptimizer( OptimizerType * optimizer )
    { 
      m_Optimizer = optimizer;
      m_Optimizer->AddObserver( itk::IterationEvent(), this );
    }

  /** Execute method will print data at each iteration */
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object *, const itk::EventObject & event)
    {
      if( typeid( event ) == typeid( itk::StartEvent ) )
        {
        std::cout << std::endl << "Position              Value";
        std::cout << std::endl << std::endl;
        }    
      else if( typeid( event ) == typeid( itk::IterationEvent ) )
        {
        std::cout << m_Optimizer->GetCachedValue() << "   ";
        std::cout << m_Optimizer->GetCachedCurrentPosition() << std::endl;
        }
      else if( typeid( event ) == typeid( itk::EndEvent ) )
        {
        std::cout << std::endl << std::endl;
        std::cout << "Solution is    = ";
        std::cout << m_Optimizer->GetCachedCurrentPosition() << std::endl;
        std::cout << std::endl;
        }
    }

protected:
  IterationCallback() {};
  itk::WeakPointer<OptimizerType>   m_Optimizer;
};




template <typename TFixedImage, typename TMovingSpatialObject>
class SimpleImageToSpatialObjectMetric : 
  public itk::ImageToSpatialObjectMetric<TFixedImage,TMovingSpatialObject>
{
public:
  /** Standard class typedefs. */
  typedef SimpleImageToSpatialObjectMetric  Self;
  typedef itk::ImageToSpatialObjectMetric<TFixedImage,TMovingSpatialObject>  
  Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  typedef itk::Point<double,3>   PointType;
  typedef std::list<PointType> PointListType;
  typedef TMovingSpatialObject MovingSpatialObjectType;
  typedef typename Superclass::ParametersType ParametersType;
  typedef typename Superclass::DerivativeType DerivativeType;
  typedef typename Superclass::MeasureType    MeasureType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(SimpleImageToSpatialObjectMetric, ImageToSpatialObjectMetric);

  itkStaticConstMacro( ParametricSpaceDimension, unsigned int, 3 );

  /** Specify the moving spatial object. */
  void SetMovingSpatialObject( const MovingSpatialObjectType * object)
    {
      if(!this->m_FixedImage)
        {
        std::cerr << "Please set the image before the moving spatial object" << std::endl;
        return;
        }
      this->m_MovingSpatialObject = object;
      m_PointList.clear();
      typedef itk::ImageRegionConstIteratorWithIndex<TFixedImage> myIteratorType;

      myIteratorType it(this->m_FixedImage,this->m_FixedImage->GetBufferedRegion());

      typename TMovingSpatialObject::PointType point;

      while( !it.IsAtEnd() )
        {
        this->m_FixedImage->TransformIndexToPhysicalPoint( it.GetIndex(), point );

        if( this->m_MovingSpatialObject->IsInside( point ) )
          { 
          m_PointList.push_back( point );
          }    
        ++it;
        }

      std::cout << "Number of points in the metric = " << static_cast<unsigned long>( m_PointList.size() ) << std::endl;
    }

  unsigned int GetNumberOfParameters(void) const  {return ParametricSpaceDimension;};

  /** Get the Derivatives of the Match Measure */
  void GetDerivative( const ParametersType &, DerivativeType & ) const
    {
      return;
    }


  /** Get the value for SingleValue optimizers. */
  MeasureType    GetValue( const ParametersType & parameters ) const
    {   
    std::cout << "GetValue(" << parameters << " ) = ";
      double value;
      this->m_Transform->SetParameters( parameters );
    
      PointListType::const_iterator it = m_PointList.begin();
    
      value = 0;
      while( it != m_PointList.end() )
        {
        PointType transformedPoint = this->m_Transform->TransformPoint(*it);
        if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) )
          {
          value += this->m_Interpolator->Evaluate( transformedPoint );
          }
        it++;
        }
      std::cout << value << std::endl;
      return value;
    }

  /** Get Value and Derivatives for MultipleValuedOptimizers */
  void GetValueAndDerivative( const ParametersType & parameters,
       MeasureType & Value, DerivativeType  & Derivative ) const
    {
      Value = this->GetValue(parameters);
      this->GetDerivative(parameters,Derivative);
    }

private:
  PointListType m_PointList;
};




int main( int argc, char *argv[] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing parameters " << std::endl;
    std::cerr << "Usage: " << argv[0] <<  " inputImage initialDelta" << std::endl;
    }

  typedef itk::Image< float, 3 >      ImageType;

  typedef itk::ImageFileReader< ImageType > ReaderType;

  typedef itk::EllipseSpatialObject< 3 >   EllipseType;

  EllipseType::Pointer ellipse = EllipseType::New();

  ellipse->SetRadius( 50.0 );


  EllipseType::TransformType::OffsetType offset;
  offset[ 0 ] =  70.0;
  offset[ 1 ] =  50.0;
  offset[ 2 ] =  50.0;

  ellipse->GetObjectToParentTransform()->SetOffset(offset);
  ellipse->ComputeObjectToWorldTransform();

  typedef itk::ImageToSpatialObjectRegistrationMethod< ImageType, EllipseType >
    RegistrationType;
  RegistrationType::Pointer registration = RegistrationType::New();


  typedef SimpleImageToSpatialObjectMetric< ImageType, EllipseType > MetricType;
  MetricType::Pointer metric = MetricType::New();


  typedef itk::LinearInterpolateImageFunction< ImageType, double >  
    InterpolatorType;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();


  typedef itk::AmoebaOptimizer  OptimizerType;
  OptimizerType::Pointer optimizer  = OptimizerType::New();


  typedef itk::TranslationTransform<> TransformType;
  TransformType::Pointer transform = TransformType::New();


  OptimizerType::ParametersType simplexDelta( transform->GetNumberOfParameters() );
  simplexDelta.Fill( atof( argv[2] )  );

  optimizer->AutomaticInitialSimplexOff();
  optimizer->SetInitialSimplexDelta( simplexDelta );
  optimizer->SetParametersConvergenceTolerance( 0.25 ); // quarter pixel
  optimizer->SetFunctionConvergenceTolerance(0.001); // 0.1%
  optimizer->SetMaximumNumberOfIterations( 200 );


  TransformType::ParametersType parametersScale( transform->GetNumberOfParameters() );
  parametersScale.Fill(1.0);

  optimizer->SetScales( parametersScale );


  typedef IterationCallback< OptimizerType >   IterationCallbackType;
  IterationCallbackType::Pointer callback = IterationCallbackType::New();
  callback->SetOptimizer( optimizer );


  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  try 
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & exp ) 
    {
    std::cerr << "Exception caught ! " << std::endl;
    std::cerr << exp << std::endl;
    }


  registration->SetFixedImage( reader->GetOutput() );
  registration->SetMovingSpatialObject( ellipse );
  registration->SetTransform( transform );
  registration->SetInterpolator( interpolator );
  registration->SetOptimizer( optimizer );
  registration->SetMetric( metric );


  TransformType::ParametersType initialParameters( transform->GetNumberOfParameters() );
  
  initialParameters[0] = 0.0;     // Offset x
  initialParameters[1] = 0.0;     // Offset y
  initialParameters[2] = 0.0;     // Offset z 

  registration->SetInitialTransformParameters(initialParameters);

  std::cout << "Initial Parameters  : " << initialParameters << std::endl;

  optimizer->MaximizeOn();


  try 
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & exp ) 
    {
    std::cerr << "Exception caught ! " << std::endl;
    std::cerr << exp << std::endl;
    }

  RegistrationType::ParametersType finalParameters 
    = registration->GetLastTransformParameters();

  std::cout << "Final Solution is : " << finalParameters << std::endl;


  return EXIT_SUCCESS;
}
