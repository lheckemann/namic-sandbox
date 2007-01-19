/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCongealingRegistrationMethod.txx,v $
  Language:  C++
  Date:      $Date: 2005/12/13 19:57:10 $
  Version:   $Revision: 1.24 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __MultiImageRegistrationMethod_cxx
#define __MultiImageRegistrationMethod_cxx

#include "MultiImageRegistrationMethod.h"


namespace itk
{

/*
 * Constructor
 */
template < typename ImageType >
MultiImageRegistrationMethod<ImageType>
::MultiImageRegistrationMethod()
{

  this->SetNumberOfRequiredOutputs( 1 );  // for the Transform

  m_ImageArrayPointer.resize(0); // Might be a bug
  m_TransformArray.resize(0);
  m_InterpolatorArray.resize(0);
  m_Metric       = 0; // has to be provided by the user.
  m_Optimizer    = 0; // has to be provided by the user.
  m_NumberOfImages = 0;


  m_InitialTransformParameters = ParametersType(1);
  m_LastTransformParameters = ParametersType(1);

  m_InitialTransformParameters.Fill( 0.0f );
  m_LastTransformParameters.Fill( 0.0f );

  m_FixedImageRegionDefined = false;


  TransformOutputPointer transformDecorator = 
                 static_cast< TransformOutputType * >( 
                                  this->MakeOutput(0).GetPointer() );

  this->ProcessObject::SetNthOutput( 0, transformDecorator.GetPointer() );

}


template < typename ImageType >
void MultiImageRegistrationMethod<ImageType>
::SetNumberOfImages(int N)
{

  m_TransformArray.resize(N);
  m_InterpolatorArray.resize(N);
  m_ImageArrayPointer.resize(N);

  for(int i=m_NumberOfImages; i<N; i++)
  {
    m_TransformArray[i]=0;
    m_InterpolatorArray[i]=0;
    m_ImageArrayPointer[i]=0;
  }

  m_NumberOfImages  = N;
  
}
/**
 *
 */
template < typename ImageType >
unsigned long
MultiImageRegistrationMethod<ImageType>
::GetMTime() const
{
  unsigned long mtime = Superclass::GetMTime();
  unsigned long m;


  // Some of the following should be removed once ivars are put in the
  // input and output lists
  

  if (m_Metric)
  {
    m = m_Metric->GetMTime();
    mtime = (m > mtime ? m : mtime);
  }

  if (m_Optimizer)
  {
    m = m_Optimizer->GetMTime();
    mtime = (m > mtime ? m : mtime);
  }


  for(int i=0; i<m_NumberOfImages; i++)
  {
    if (m_ImageArrayPointer[i])
    {
      m = m_ImageArrayPointer[i]->GetMTime();
      mtime = (m > mtime ? m : mtime);
    } 

    if (m_TransformArray[i])
    {
      m = m_TransformArray[i]->GetMTime();
      mtime = (m > mtime ? m : mtime);
    }

    if (m_InterpolatorArray[i])
    {
      m = m_InterpolatorArray[i]->GetMTime();
      mtime = (m > mtime ? m : mtime);
    }
  }

  return mtime;
  
}



/*
 * Set i'th initial transform parameters
 */
template < typename ImageType >
void
MultiImageRegistrationMethod<ImageType>
::SetInitialTransformParameters( const ParametersType & param, int i )
{
  for(int j=0; j < param.Size(); j++)
  {
      m_InitialTransformParameters[i*param.Size()+j] = param[j];
  }
  this->Modified();
}

/*
 * Set the initial transform parameters
 */
template < typename ImageType >
void
MultiImageRegistrationMethod<ImageType>
::SetInitialTransformParameters( const ParametersType & param )
{

  m_InitialTransformParameters = param;
  this->Modified();
}



/*
 * Set the region of the fixed image to be considered for registration
 */
template < typename ImageType >
void
MultiImageRegistrationMethod<ImageType>
::SetFixedImageRegion( const FixedImageRegionType & region )
{ 
  m_FixedImageRegion = region;
  m_FixedImageRegionDefined = true;
}


/*
 * Initialize by setting the interconnects between components. 
 */
template < typename ImageType >
void
MultiImageRegistrationMethod<ImageType>
::Initialize() throw (ExceptionObject)
{

  for(int i=0; i<m_NumberOfImages; i++)
  {
    if( !m_ImageArrayPointer[i] )
    {
      itkExceptionMacro(<<"FixedImage " << i << " is not present");
    }

    if( !m_TransformArray[i] )
    {
      itkExceptionMacro(<<"Transform " << i << " is not present");
    }

    if( !m_InterpolatorArray[i] )
    {
      itkExceptionMacro(<<"Interpolator " << i << " is not present");
    }
  }


  if ( !m_Metric )
  {
    itkExceptionMacro(<<"Metric is not present" );
  }

  if ( !m_Optimizer )
  {
    itkExceptionMacro(<<"Optimizer is not present" );
  }

  //
  // Connect the transform to the Decorator.
  //
//   TransformOutputType * transformOutput =  
//      static_cast< TransformOutputType * >( this->ProcessObject::GetOutput(0) );
// 
   /**Need to Change */
  /** What is this ? :) */
//   transformOutput->Set( m_TransformArray[0].GetPointer() );

  vector<TransformOutputPointer> transformOutput;
  transformOutput.resize(m_TransformArray.size());

  for(int i=0; i<m_TransformArray.size(); i++)
  {
    transformOutput[i] = static_cast< TransformOutputType * >( this->ProcessObject::GetOutput(0) );
    transformOutput[i]->Set( m_TransformArray[i].GetPointer() );
  }
  
  /* Setup the metric */
  m_Metric->SetNumberOfImages(m_NumberOfImages);
  for(int i=0; i<m_NumberOfImages; i++)
  {
    m_Metric->SetImageArray( m_ImageArrayPointer[i], i);
    m_Metric->SetInterpolatorArray( m_InterpolatorArray[i], i );
    m_Metric->SetTransformArray( m_TransformArray[i], i );
  }


  if( m_FixedImageRegionDefined )
  {
    m_Metric->SetFixedImageRegion( m_FixedImageRegion );
  }
  else
  {
    m_Metric->SetFixedImageRegion( m_ImageArrayPointer[0]->GetBufferedRegion() );
  }

  m_Metric->Initialize();

  // Setup the optimizer
  m_Optimizer->SetCostFunction( m_Metric );

  // Validate initial transform parameters
  if ( m_InitialTransformParameters.Size() !=
       m_TransformArray[0]->GetNumberOfParameters()*m_TransformArray.size() )
  {
    itkExceptionMacro(<<"Size mismatch between initial parameter and transform");
  }

  m_Optimizer->SetInitialPosition( m_InitialTransformParameters );

}


/*
 * Starts the Registration Process
 */
template < typename ImageType >
void
MultiImageRegistrationMethod<ImageType>
::StartRegistration( void )
{ 

  ParametersType empty(1);
  empty.Fill( 0.0 );
  try
  {
    // initialize the interconnects between components
    this->Initialize();
  }
  catch( ExceptionObject& err )
  {
    m_LastTransformParameters = empty;
    // pass exception to caller
    throw err;
  }
  
  this->StartOptimization();
}


/*
 * Starts the Optimization process
 */
template < typename ImageType >
void
MultiImageRegistrationMethod<ImageType>
::StartOptimization( void )
{ 
  try
  {
    // do the optimization
    m_Optimizer->StartOptimization();
  }
  catch( ExceptionObject& err )
  {
    // An error has occurred in the optimization.
    // Update the parameters
    m_LastTransformParameters = m_Optimizer->GetCurrentPosition();

    // Pass exception to caller
    throw err;
  }

  // get the results
  ParametersType current(m_TransformArray[0]->GetNumberOfParameters());
  m_LastTransformParameters = m_Optimizer->GetCurrentPosition();
  for(int i=0; i<m_NumberOfImages; i++)
  {
    for(int j=0; j<m_TransformArray[i]->GetNumberOfParameters();j++)
    {
      current[j] = m_LastTransformParameters[i*m_TransformArray[i]->GetNumberOfParameters()+j];
    }
    m_TransformArray[i]->SetParametersByValue( current );
  }
}


/*
 * PrintSelf
 */
template < typename ImageType >
void
MultiImageRegistrationMethod<ImageType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Metric: " << m_Metric.GetPointer() << std::endl;
  os << indent << "Optimizer: " << m_Optimizer.GetPointer() << std::endl;
  for( int i=0; i < m_NumberOfImages; i++ )
  {
    os << indent << "Transform: " << i << " " << m_TransformArray[i].GetPointer() << std::endl;
    os << indent << "Interpolator: " << i << " " << m_InterpolatorArray[i].GetPointer() << std::endl;
    os << indent << "Image: " << i << " " << m_ImageArrayPointer[i].GetPointer() << std::endl;
  }
  os << indent << "Fixed Image Region: " << m_FixedImageRegion << std::endl;
  os << indent << "Initial Transform Parameters: " << m_InitialTransformParameters << std::endl;
  os << indent << "Last    Transform Parameters: " << m_LastTransformParameters << std::endl;
}



/*
 * Generate Data
 */
template < typename ImageType >
void
MultiImageRegistrationMethod<ImageType>
::GenerateData()
{
  this->StartRegistration();
}


/*
 *  Get Output
 */
template < typename ImageType >
const typename MultiImageRegistrationMethod<ImageType>::TransformOutputType *
MultiImageRegistrationMethod<ImageType>
::GetOutput() const
{
  return static_cast< const TransformOutputType * >( this->ProcessObject::GetOutput(0) );
}



template < typename ImageType >
DataObject::Pointer
MultiImageRegistrationMethod<ImageType>
::MakeOutput(unsigned int output)
{
  switch (output)
    {
    case 0:
  
  //TransformOutputType a;
   //a.New;
  //a.Set(m_TransformArray[0]);
        return static_cast<DataObject*>(TransformOutputType::New().GetPointer());
  //return static_cast<DataObject*>(TransformOutputType::Register(m_TransformArray[0]));
      break;
    default:
      itkExceptionMacro("MakeOutput request for an output number larger than the expected number of outputs");
      return 0;
    }
}


template < typename ImageType >
void 
MultiImageRegistrationMethod<ImageType>
::SetImage( const ImageType * fixedImage, int i )
{
  itkDebugMacro("setting Fixed Image to " << fixedImage ); 

  if (this->m_ImageArrayPointer[i].GetPointer() != fixedImage ) 
  {
    this->m_ImageArrayPointer[i] = fixedImage;

    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(0, const_cast< ImageType *>( fixedImage ) );
    
    this->Modified(); 
  }
}

/*
 * Set the length of the parameters vector
 */
template < typename ImageType >
void
MultiImageRegistrationMethod<ImageType>
::SetTransformParametersLength( int N )
{

  m_InitialTransformParameters.SetSize(N);
  m_LastTransformParameters.SetSize(N);

  this->Modified();
}

/*
 * Get the length of the parameters vector
 */
template < typename ImageType >
int
MultiImageRegistrationMethod<ImageType>
::GetTransformParametersLength( int N )
{

  return m_InitialTransformParameters.GetSize();
  
}




} // end namespace itk


#endif
