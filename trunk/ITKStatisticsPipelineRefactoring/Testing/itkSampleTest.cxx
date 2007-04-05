/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/11 13:52:36 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>
#include "itkSample.h"
#include "itkObjectFactory.h"

namespace itk {
namespace StatisticsNew {
namespace SampleTest {

template <class TMeasurementVector>
class MySample : public Sample< TMeasurementVector >
{
public:
  /** Standard class typedef. */
  typedef MySample  Self;
  typedef Sample< TMeasurementVector > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Standard macros */
  itkTypeMacro(MySample, Sample);

  /** Method for creation through the object factory. */
  itkNewMacro(Self) ;
 
  typedef typename Superclass::MeasurementVectorType MeasurementVectorType;
  typedef typename Superclass::TotalFrequencyType TotalFrequencyType;
  typedef typename Superclass::FrequencyType FrequencyType;
  typedef typename Superclass::InstanceIdentifier InstanceIdentifier;

  /** Get the size of the sample (number of measurements) */
  virtual unsigned int Size() const { m_Values.size(); }

  /** Get the measurement associated with a particular
   * InstanceIdentifier. */
  virtual const MeasurementVectorType & 
    GetMeasurementVector(const InstanceIdentifier &id) const
      { return m_Values[id]; }

  /** Get the frequency of a measurement specified by instance
   * identifier. */
  virtual FrequencyType GetFrequency(const InstanceIdentifier &id) const
      { return m_Frequencies[id]; }

  /** Get the total frequency of the sample. */
  virtual TotalFrequencyType GetTotalFrequency() const 
    { 
    TotalFrequencyType sum = NumericTraits< TotalFrequencyType >::Zero;
    typedef typename std::vector< FrequencyType >::const_iterator Iterator;
    Iterator itr = m_Frequencies.begin();
    while( itr != m_Frequencies.end() )
      {
      sum += *itr;
      ++itr;
      }
    return sum;
    }

private:

  std::vector< TMeasurementVector >  m_Values;
  std::vector< FrequencyType >       m_Frequencies;

};

}
}
}
int itkSampleTest(int, char* [] )
{

  const unsigned int MeasurementVectorSize = 17;

  typedef itk::FixedArray< 
    float, MeasurementVectorSize >  MeasurementVectorType;

  typedef itk::StatisticsNew::SampleTest::MySample< 
    MeasurementVectorType >   SampleType;

  SampleType::Pointer sample = SampleType::New();

  sample->SetMeasurementVectorSize( 27 ); // in this case the call should be ignored.

  if( sample->GetMeasurementVectorSize() != MeasurementVectorSize )
    {
    std::cerr << "GetMeasurementVectorSize() Failed !" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
