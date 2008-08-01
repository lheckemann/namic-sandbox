/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSampleClassifierFilter.txx,v $
  Language:  C++
  Date:      $Date: 2003/12/15 01:00:46 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSampleClassifierFilter_txx
#define __itkSampleClassifierFilter_txx

#include "itkSampleClassifierFilter.h"

namespace itk {
namespace Statistics {

template< class TSample >
SampleClassifierFilter< TSample >
::SampleClassifierFilter()
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
SampleClassifierFilter< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

template< class TSample >
void
SampleClassifierFilter< TSample >
::SetInput( const TSample* sample )
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0,
                                   const_cast< SampleType * >( sample ) );
}

template< class TSample >
const TSample *
SampleClassifierFilter< TSample >
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
SampleClassifierFilter< TSample >
::SetClassLabels( const ClassLabelVectorObjectType * classLabels )
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(1,
                                   const_cast< ClassLabelVectorObjectType * >( classLabels ) );
}

template< class TSample >
void
SampleClassifierFilter< TSample >
::SetMembershipFunctions( const MembershipFunctionVectorObjectType * membershipFunctions )
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(2,
                                   const_cast< MembershipFunctionVectorObjectType * >( membershipFunctions ) );
}

template< class TSample >
typename SampleClassifierFilter< TSample >::DataObjectPointer
SampleClassifierFilter< TSample >
::MakeOutput(unsigned int)
{
  return static_cast<DataObject*>( MembershipSampleType::New().GetPointer() );
}

template< class TSample >
void
SampleClassifierFilter< TSample >
::GenerateData()
{
  const ClassLabelVectorObjectType * classLabelsDecorated = 
    static_cast< const ClassLabelVectorObjectType * >( this->ProcessObject::GetInput( 1 ) );

  const MembershipFunctionVectorObjectType * membershipFunctionsDecorated = 
    static_cast< const MembershipFunctionVectorObjectType * >( this->ProcessObject::GetInput( 2 ) );

  if( classLabelsDecorated == NULL )
    {
    itkExceptionMacro("Input of class labels decorated is NULL");
    }

  if( membershipFunctionsDecorated == NULL )
    {
    itkExceptionMacro("Input of membership samples decorated is NULL");
    }

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

  const SampleType * sample =
    static_cast< const SampleType * >( this->ProcessObject::GetInput( 0 ) );

  std::vector< double > discriminantScores;
  discriminantScores.resize( this->m_NumberOfClasses );

  MembershipSampleType * output = dynamic_cast< MembershipSampleType * >(
                      this->ProcessObject::GetOutput(0)); 

  output->SetSample( this->GetInput() );
  output->SetNumberOfClasses( this->m_NumberOfClasses ); 

  typename TSample::ConstIterator iter = sample->Begin();
  typename TSample::ConstIterator end  = sample->End();

  while (iter != end)
    {
    typename TSample::MeasurementVectorType measurements;
    measurements = iter.GetMeasurementVector();
    for (unsigned int i = 0; i < this->m_NumberOfClasses; i++)
      {
      discriminantScores[i] =
        membershipFunctions[i]->Evaluate(measurements);
      }

    unsigned int classIndex;
    classIndex = m_DecisionRule->Evaluate(discriminantScores);
  
    output->AddInstance(classLabels[classIndex], iter.GetInstanceIdentifier());
    ++iter;
    }
}

template< class TSample >
const typename SampleClassifierFilter< TSample >::MembershipSampleType *
SampleClassifierFilter< TSample >
::GetOutput() const
{
  return static_cast< const MembershipSampleType * >(this->ProcessObject::GetOutput(0));
}

} // end of namespace Statistics
} // end of namespace itk

#endif
