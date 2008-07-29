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
  typename MembershipSampleType::Pointer output
    = static_cast<MembershipSampleType*>(this->MakeOutput(0).GetPointer()); 

  output->SetSample( this->GetInput() );

  this->ProcessObject::SetNthOutput(0, output.GetPointer());
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
const TSample*
SampleClassifierFilter< TSample >
::GetInput() const
{
  const SampleType * input =
    static_cast< const SampleType * >( this->ProcessObject::GetInput( 0 ) );
  return input;
}

template< class TSample >
typename SampleClassifierFilter< TSample >::DataObjectPointer
SampleClassifierFilter< TSample >
::MakeOutput(unsigned int)
{
  return static_cast<DataObject*>( OutputType::New().GetPointer() );
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

  /*
  unsigned int i;
  typename TSample::ConstIterator iter = this->GetSample()->Begin();
  typename TSample::ConstIterator end  = this->GetSample()->End();
  typename TSample::MeasurementVectorType measurements;

  m_Output->Resize( this->GetSample()->Size() );
  std::vector< double > discriminantScores;
  unsigned int numberOfClasses = this->GetNumberOfClasses();
  discriminantScores.resize(numberOfClasses);
  unsigned int classLabel;
  m_Output->SetNumberOfClasses(numberOfClasses);
  typename Superclass::DecisionRuleType::Pointer rule =
    this->GetDecisionRule();

  if ( m_ClassLabels.size() != this->GetNumberOfMembershipFunctions() )
    {
    while (iter != end)
      {
      measurements = iter.GetMeasurementVector();
      for (i = 0; i < numberOfClasses; i++)
        {
        discriminantScores[i] =
          (this->GetMembershipFunction(i))->Evaluate(measurements);
        }
      classLabel = rule->Evaluate(discriminantScores);
      m_Output->AddInstance(classLabel, iter.GetInstanceIdentifier());
      ++iter;
      }
    }
  else
    {
    while (iter != end)
      {
      measurements = iter.GetMeasurementVector();
      for (i = 0; i < numberOfClasses; i++)
        {
        discriminantScores[i] =
          (this->GetMembershipFunction(i))->Evaluate(measurements);
        }
      classLabel = rule->Evaluate(discriminantScores);
      m_Output->AddInstance(m_ClassLabels[classLabel],
                            iter.GetInstanceIdentifier());
      ++iter;
      }
    }
    */
}

template< class TSample >
const typename SampleClassifierFilter< TSample >::OutputType *
SampleClassifierFilter< TSample >
::GetOutput() const
{
  const OutputType * output =
    static_cast< const OutputType * >(this->ProcessObject::GetOutput(0));

  return output;
}

} // end of namespace Statistics
} // end of namespace itk

#endif
