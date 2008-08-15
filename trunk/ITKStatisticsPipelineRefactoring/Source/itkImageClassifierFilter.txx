/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageClassifierFilter.txx,v $
  Language:  C++
  Date:      $Date: 2003/12/15 01:00:46 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageClassifierFilter_txx
#define __itkImageClassifierFilter_txx

#include "itkImageClassifierFilter.h"

namespace itk {
namespace Statistics {

template< class TSample >
ImageClassifierFilter< TSample >
::ImageClassifierFilter()
{
  this->SetNumberOfRequiredInputs( 3 );
  this->SetNumberOfRequiredOutputs( 1 );

  // Create the output. We use static_cast<> here because we know the default
  // output must be of type MembershipSampleType
  MembershipSampleObjectPointer membershipSample
    = dynamic_cast<MembershipSampleType*>(this->MakeOutput(0).GetPointer()); 

  this->ProcessObject::SetNthOutput(0, membershipSample.GetPointer());

  /** Set sample in the output */
  membershipSample->SetSample( this->GetInput() ); 

  /** Initialize decision rule */
  m_DecisionRule = NULL;
}

template< class TSample >
void
ImageClassifierFilter< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

template< class TSample >
void
ImageClassifierFilter< TSample >
::SetInput( const TSample* sample )
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0,
                                   const_cast< SampleType * >( sample ) );
}

template< class TSample >
const TSample *
ImageClassifierFilter< TSample >
::GetInput( ) const
{
  if (this->GetNumberOfInputs() < 1)
    {
    return 0;
    }

  return static_cast<const SampleType * >
  (this->ProcessObject::GetInput(0) );
}


template< class TSample >
void
ImageClassifierFilter< TSample >
::SetClassLabels( const ClassLabelVectorObjectType * classLabels )
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(1,
                                   const_cast< ClassLabelVectorObjectType * >( classLabels ) );
}

template< class TSample >
void
ImageClassifierFilter< TSample >
::SetMembershipFunctions( const MembershipFunctionVectorObjectType * membershipFunctions )
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(2,
                                   const_cast< MembershipFunctionVectorObjectType * >( membershipFunctions ) );
}

template< class TSample >
void
ImageClassifierFilter< TSample >
::SetMembershipFunctionsWeightsArray( const
MembershipFunctionsWeightsArrayObjectType * weightsArray )
{
 // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(3,
                                   const_cast<
  MembershipFunctionsWeightsArrayObjectType * >( weightsArray ) );
}


template< class TSample >
typename ImageClassifierFilter< TSample >::DataObjectPointer
ImageClassifierFilter< TSample >
::MakeOutput(unsigned int)
{
  return static_cast<DataObject*>( MembershipSampleType::New().GetPointer() );
}

template< class TSample >
void
ImageClassifierFilter< TSample >
::GenerateData()
{
  const ClassLabelVectorObjectType * classLabelsDecorated = 
    static_cast< const ClassLabelVectorObjectType * >( this->ProcessObject::GetInput( 1 ) );

  const MembershipFunctionVectorObjectType * membershipFunctionsDecorated = 
    static_cast< const MembershipFunctionVectorObjectType * >( this->ProcessObject::GetInput( 2 ) );

  const MembershipFunctionsWeightsArrayObjectType *
                membershipFunctionsWeightsArrayDecorated = 
    static_cast< const MembershipFunctionsWeightsArrayObjectType * >( this->ProcessObject::GetInput( 3 ) );

  const ClassLabelVectorType & classLabels = classLabelsDecorated->Get();

  const MembershipFunctionVectorType & membershipFunctions = membershipFunctionsDecorated->Get();

  // Check number of Labels and MembershipSamples against the number of classes */
  if( membershipFunctions.size() != this->m_NumberOfClasses )
    {
    itkExceptionMacro("Number of Membership functions does not match the number of classes");
    }

  if( classLabels.size() != this->m_NumberOfClasses )
    {
    itkExceptionMacro("Number of class labels does not match the number of classes");
    }

  if( m_DecisionRule.IsNull())
    {
    itkExceptionMacro("Decision rule is not set");
    }

  MembershipFunctionsWeightsArrayType membershipFunctionsWeightsArray;
  if( membershipFunctionsWeightsArrayDecorated == NULL )
    {
    // no weights array is set and hence all membership functions will have equal
    // weight
    membershipFunctionsWeightsArray.SetSize( this->m_NumberOfClasses );
    membershipFunctionsWeightsArray.Fill(1.0);
    }
  else
    {
    membershipFunctionsWeightsArray = membershipFunctionsWeightsArrayDecorated->Get();
    }

  if ( membershipFunctionsWeightsArray.Size() != this->m_NumberOfClasses
)
    {
    itkExceptionMacro("Membership functions weight array size does not match the\
                      number of classes "); 
    }
  
  const ImageType * inputImage =
    static_cast< const ImageType * >( this->ProcessObject::GetInput( 0 ) );

  std::vector< double > discriminantScores;
  discriminantScores.resize( this->m_NumberOfClasses );

  OutputImageType * outputImage = dynamic_cast< OutputImageType * >(
                      this->ProcessObject::GetOutput(0)); 

  outputImage->CopyInformation( inputImage );
  outputImage->SetRegions( inputImage->GetBufferedRegion() );
  outputImage->Allocate();

  ImageRegionConstIterator< InputImageType >   inpItr( inputImage, inputImage->GetBufferedRegion() );
  ImageRegionIterator< OutputImageType >       outItr( outputImage, outputImage->GetBufferedRegion() );

  inpItr.GoToBegin();
  outItr.GoToBegin();

  while( !inpItr.IsNotEnd() )
    {
    typename MeasurementVectorType measurements;

    MeasurementVectorTraits::Assign( measurements, inpItr.Get() ); 

    for (unsigned int i = 0; i < this->m_NumberOfClasses; i++)
      {
      discriminantScores[i] = membershipFunctionsWeightsArray[i] * 
        membershipFunctions[i]->Evaluate(measurements);
      }

    unsigned int classIndex;
    classIndex = m_DecisionRule->Evaluate(discriminantScores);
  
    outItr.Set(classLabels[classIndex] );

    ++inpItr;
    ++outItr;
    }
}

template< class TSample >
const typename ImageClassifierFilter< TSample >::MembershipSampleType *
ImageClassifierFilter< TSample >
::GetOutput() const
{
  return static_cast< const MembershipSampleType * >(this->ProcessObject::GetOutput(0));
}

} // end of namespace Statistics
} // end of namespace itk

#endif
