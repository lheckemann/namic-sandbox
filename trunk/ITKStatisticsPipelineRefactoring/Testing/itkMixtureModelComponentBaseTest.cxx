/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToListSampleAdaptorTest.cxx,v $
  Language:  C++
  Date:      $Date: 2007/01/15 18:38:35 $
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

#include "itkMixtureModelComponentBase.h"
#include "itkListSample.h"
#include "itkObjectFactory.h"

namespace itk {
namespace Statistics {

template <class TSample>
class MixtureModelComponentBaseTestHelper : public MixtureModelComponentBase<TSample>
{
public:
  typedef MixtureModelComponentBaseTestHelper   Self;
  typedef MixtureModelComponentBase<TSample>    Superclass;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;

  itkTypeMacro(MixtureModelComponentBaseTestHelper, MixtureModelComponentBase );
 
  itkNewMacro(Self);
 
  void RunTests()
    {
    std::cout << "Superclass Name " << this->Superclass::GetNameOfClass() << std::endl;
    std::cout << "This class Name " << this->GetNameOfClass() << std::endl;

    this->Print( std::cout );

    std::cout << "Full Parameters = " << this->GetFullParameters() << std::endl;
    std::cout << "Minimal change  = " << this->GetMinimalParametersChange() << std::endl;
    }

};

}
}

int itkMixtureModelComponentBaseTest( int , char* [] ) 
{

  typedef itk::Array< double > MeasurementVectorType ;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType ;

  typedef itk::Statistics::MixtureModelComponentBase<SampleType>  ComponentBaseType;

  ComponentBaseType::Pointer component1 = ComponentBaseType::New();

  component1->Print( std::cout );

  std::cout << "GetNameOfClass() = " << component1->GetNameOfClass() << std::endl;

  std::cout << "Full Parameters = " << component1->GetFullParameters() << std::endl;
  std::cout << "Minimal change  = " << component1->GetMinimalParametersChange() << std::endl;

  typedef itk::Statistics::MixtureModelComponentBaseTestHelper<SampleType>  ComponentType;

  ComponentType::Pointer component2 = ComponentType::New();

  component2->RunTests();

  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}
