/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkSampleClassifierWithMaskTest.cxx,v $
Language:  C++
Date:      $Date: 2005-08-24 15:16:19 $
Version:   $Revision: 1.7 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include "itkWin32Header.h"

#include <fstream>

#include "itkVector.h"
#include "itkPoint.h"
#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "vnl/vnl_matrix.h"

#include "itkPointSetToListAdaptor.h"
#include "itkSubsample.h"
#include "itkEuclideanDistance.h"
#include "itkMinimumDecisionRule.h"
#include "itkListSample.h"
#include "itkSampleClassifierWithMask.h"

int itkSampleClassifierWithMaskTest(int argc, char* argv[] )
{
  namespace stat = itk::Statistics ;
 
  if (argc < 2)
    {
    std::cout << "ERROR: data file name argument missing." 
              << std::endl ;
    return EXIT_FAILURE;
    }

  unsigned int i, j ;
  char* dataFileName = argv[1] ;
  int dataSize = 2000 ;

  unsigned int numberOfClasses = 2 ;

  /* Loading point data */
  typedef itk::PointSet< double, 2 > PointSetType ;
  PointSetType::Pointer pointSet = PointSetType::New() ;
  PointSetType::PointsContainerPointer pointsContainer = 
    PointSetType::PointsContainer::New() ;
  pointsContainer->Reserve(dataSize) ;
  pointSet->SetPoints(pointsContainer.GetPointer()) ;

  PointSetType::PointsContainerIterator p_iter = pointsContainer->Begin() ;
  PointSetType::PointType point ;
  double temp ;
  std::ifstream dataStream(dataFileName) ;
  while (p_iter != pointsContainer->End())
    {
    for ( i = 0 ; i < PointSetType::PointDimension ; i++)
      {
      dataStream >> temp ;
      point[i] = temp ;
      }
    p_iter.Value() = point ;
    ++p_iter ;
    }

  dataStream.close() ;
  
  /* Importing the point set to the sample */
  typedef stat::PointSetToListAdaptor< PointSetType >
    DataSampleType;

  DataSampleType::Pointer sample =
    DataSampleType::New() ;  

  sample->SetPointSet(pointSet);

  /** preparing classifier and decision rule object */
  typedef itk::FixedArray< unsigned int , 1 > ClassMaskVectorType ;
  typedef itk::Statistics::ListSample< ClassMaskVectorType > 
    ClassMaskSampleType ;
  ClassMaskSampleType::Pointer mask = ClassMaskSampleType::New() ;

  ClassMaskVectorType m ;
  m[0] = 0 ;
  for ( i = 0 ; i < 200 ; ++i )
    {
    mask->PushBack( m ) ;
    }
  
  m[0] = 1 ;
  for ( i = 200 ; i < 1000 ; ++i )
    {
    mask->PushBack( m ) ;
    }

  m[0] = 2 ;
  for ( i = 1000 ; i < 2000 ; ++i )
    {
    mask->PushBack( m ) ;
    }

  typedef itk::Statistics::SampleClassifierWithMask< DataSampleType, 
    ClassMaskSampleType > ClassifierType ;
  ClassifierType::Pointer classifier = ClassifierType::New() ;

  typedef itk::MinimumDecisionRule DecisionRuleType ;
  typedef itk::Statistics::EuclideanDistance< 
    DataSampleType::MeasurementVectorType > MembershipFunctionType ;
  DecisionRuleType::Pointer decisionRule = DecisionRuleType::New() ;
  classifier->SetDecisionRule((itk::DecisionRuleBase::Pointer) decisionRule) ;
  classifier->SetNumberOfClasses(numberOfClasses) ;
  classifier->SetSample(sample.GetPointer()) ;

  typedef MembershipFunctionType::OriginType MeanType;
  std::vector< MeanType > trueMeans;
  MeanType m1( 2 );
  MeanType m2( 2 );
  trueMeans.push_back( m1 );
  trueMeans.push_back( m2 );
  trueMeans[0][0] = 99.261 ;
  trueMeans[0][1] = 100.078 ;
  trueMeans[1][0] = 200.1 ;
  trueMeans[1][1] = 201.3 ;



  ClassifierType::ClassLabelVectorType selectedClassLabels ;
  selectedClassLabels.push_back( 1 ) ;
  selectedClassLabels.push_back( 2 ) ;
  classifier->SetSelectedClassLabels( selectedClassLabels ) ;
  classifier->SetOtherClassLabel( 0 ) ;
  classifier->SetMask( mask ) ;
  
  std::vector< MembershipFunctionType::Pointer > membershipFunctions ;
  std::vector< unsigned int > classLabels ;
  for ( i = 0 ; i < numberOfClasses ; i++ ) 
    {
    membershipFunctions.push_back(MembershipFunctionType::New()) ;
    classLabels.push_back(i + 1) ;
    for ( j = 0 ; j < DataSampleType::MeasurementVectorSize ; j++ )
      {
      membershipFunctions[i]->SetOrigin(trueMeans[i]) ;
      }
    classifier->AddMembershipFunction(membershipFunctions[i].GetPointer()) ;
    }
  classifier->SetMembershipFunctionClassLabels(classLabels) ;

  /* start classification process */
  classifier->Update() ;

  /* evaluate the classification result */
  ClassifierType::OutputType* membershipSample =
    classifier->GetOutput() ;
  ClassifierType::OutputType::ConstIterator m_iter =
    membershipSample->Begin() ;
  ClassifierType::OutputType::ConstIterator m_last =
    membershipSample->End() ;

  unsigned int index = 0 ;
  unsigned int error1 = 0 ;
  unsigned int error2 = 0 ;
  while ( m_iter != m_last )
    {
    if ( index < 200 )
      {
      if ( m_iter.GetClassLabel() != 0 )
        {
        ++error1 ;
        }
      ++index ;
      ++m_iter ;
      continue ;
      }
    
    if ( index < 1000 )
      {
      if ( m_iter.GetClassLabel() != 1 )
        {
        ++error1 ;
        }
      ++index ;
      ++m_iter ;
      continue ;
      }

    if ( m_iter.GetClassLabel() != 2 )
      {
      ++error2 ;
      }
    ++index ;
    ++m_iter ;
    }

  std::cout << "Among 2000 measurement vectors, " << error1 + error2
            << " vectors are misclassified." << std::endl ;
  if( double(error1 / 10) > 2 || double(error2 / 10) > 2)
    {
    std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}







