/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogramToTextureFeaturesFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/25 11:54:23 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
// Insight classes
#include "itkHistogram.h"
#include "vnl/vnl_math.h"

#include "itkHistogramToTextureFeaturesFilter.h"

int itkHistogramToTextureFeaturesFilterTest(int, char* [] )
{
  //Data definitions 
  const unsigned int  HISTOGRAM_AXIS_LEN =  25;


  //------------------------------------------------------
  // Create a simple test histogram. The histogram must be
  // symmetric and normalized.
  //------------------------------------------------------
  typedef float MeasurementType;
  typedef itk::Statistics::Histogram< MeasurementType, 2 > HistogramType ;
  HistogramType::Pointer histogram = HistogramType::New() ;
  HistogramType::SizeType size ;
  size.Fill(HISTOGRAM_AXIS_LEN) ;
  HistogramType::MeasurementVectorType lowerBound ;
  HistogramType::MeasurementVectorType upperBound ;
  lowerBound[0] = 0 ;
  lowerBound[1] = 0 ;
  upperBound[0] = HISTOGRAM_AXIS_LEN + 1 ;
  upperBound[1] = HISTOGRAM_AXIS_LEN + 1 ;
  histogram->Initialize(size, lowerBound, upperBound ) ; 

  HistogramType::IndexType                  index ;
  HistogramType::AbsoluteFrequencyType              frequency;
  HistogramType::InstanceIdentifier         identifier;
  
  index[0] = 0 ;
  index[1] = 0 ;
  frequency = 10; 
  identifier = histogram->GetInstanceIdentifier ( index );
  histogram->SetFrequency(identifier, frequency);

  index[0] = 3 ;
  index[1] = 3 ;
  frequency = 50; 
  identifier = histogram->GetInstanceIdentifier ( index );
  histogram->SetFrequency(identifier, frequency);

  index[0] = 2 ;
  index[1] = 1 ;
  frequency= 5;
  identifier = histogram->GetInstanceIdentifier ( index );
  histogram->SetFrequency(identifier, frequency);

  index[0] = 1 ;
  index[1] = 2 ;
  frequency = 5;
  identifier = histogram->GetInstanceIdentifier ( index );
  histogram->SetFrequency(identifier, frequency);

  index[0] = 7 ;
  index[1] = 6 ;
  frequency = 10;
  identifier = histogram->GetInstanceIdentifier ( index );
  histogram->SetFrequency(identifier, frequency);

  index[0] = 6 ;
  index[1] = 7 ;
  frequency = 10;
  identifier = histogram->GetInstanceIdentifier ( index );
  histogram->SetFrequency(identifier, frequency);

  index[0] = 10 ;
  index[1] = 10 ;
  frequency = 10;
  identifier = histogram->GetInstanceIdentifier ( index );
  histogram->SetFrequency(identifier, frequency);

  typedef itk::Statistics::HistogramToTextureFeaturesFilter<
    HistogramType > HistogramToTextureFeaturesFilterType;

  HistogramToTextureFeaturesFilterType::Pointer filter = 
                      HistogramToTextureFeaturesFilterType::New();

  std::cout << filter->GetNameOfClass() << std::endl;
  filter->Print(std::cout);

  bool passed = true;
  //Invoke update before adding an input. An exception should be 
  //thrown.
  try
    {
    filter->Update();
    passed = false;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    }    

  if ( filter->GetInput() != NULL )
    {
    passed = false;
    }

  filter->ResetPipeline();
 
  filter->SetInput( histogram );
  try
    {
    filter->Update();
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    return EXIT_FAILURE;
    }   
  
  double trueEnergy = 0.295;
  double trueEntropy = 2.26096;
  double trueCorrelation = 0.12819;
  double trueInverseDifferenceMoment = 0.85;
  double trueInertia = 0.3;
  double trueClusterShade = 139.1879;
  double trueClusterProminence = 2732.557;
  double trueHaralickCorrelation = 2264.549;
  
  double energy = filter->GetEnergy();
  double entropy = filter->GetEntropy();
  double correlation = filter->GetCorrelation();
  double inverseDifferenceMoment = filter->GetInverseDifferenceMoment();
  double inertia = filter->GetInertia();
  double clusterShade = filter->GetClusterShade();
  double clusterProminence = filter->GetClusterProminence();
  double haralickCorrelation = filter->GetHaralickCorrelation();
  
  
  if( vnl_math_abs(energy - trueEnergy) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "Energy calculated wrong. Expected: " << trueEnergy << ", got: " 
      << energy << std::endl;
    passed = false;
    }
   
  if( vnl_math_abs(entropy - trueEntropy) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "Entropy calculated wrong. Expected: " << trueEntropy << ", got: "  
      << entropy << std::endl;
    passed = false;
    }
  
  if( vnl_math_abs(correlation - trueCorrelation) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "Correlation calculated wrong. Expected: " << trueCorrelation << 
      ", got: "  << correlation << std::endl;
    passed = false;
    }
  
  if( vnl_math_abs(inverseDifferenceMoment - trueInverseDifferenceMoment) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "InverseDifferenceMoment calculated wrong. Expected: " << 
      trueInverseDifferenceMoment <<  ", got: "  << inverseDifferenceMoment << std::endl;
    passed = false;
    }
  
  if( vnl_math_abs(inertia - trueInertia) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "Inertia calculated wrong. Expected: " << trueInertia << ", got: " 
      << inertia << std::endl;
    passed = false;
    }
  
  if( vnl_math_abs(clusterShade - trueClusterShade) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "ClusterShade calculated wrong. Expected: " << trueClusterShade << 
      ", got: "  << clusterShade << std::endl;
    passed = false;
    }

  if( vnl_math_abs(clusterProminence - trueClusterProminence) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "ClusterProminence calculated wrong. Expected: " 
      << trueClusterProminence << ", got: "  << clusterProminence << std::endl;
    passed = false;
    }
  
  if( vnl_math_abs(haralickCorrelation - trueHaralickCorrelation) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "Haralick's Correlation calculated wrong. Expected: "
      << trueHaralickCorrelation << ", got: "  << haralickCorrelation << std::endl;
    passed = false;
    }
 
  //Get the texture features using GetFeature() method  
  double energy2 = filter->GetFeature(
                      HistogramToTextureFeaturesFilterType::Energy);

  double entropy2 = filter->GetFeature(
                      HistogramToTextureFeaturesFilterType::Entropy);

  double correlation2 = filter->GetFeature(
                      HistogramToTextureFeaturesFilterType::Correlation);

  double inverseDifferenceMoment2 = filter->GetFeature(
                      HistogramToTextureFeaturesFilterType::InverseDifferenceMoment);

  double inertia2 = filter->GetFeature(
                      HistogramToTextureFeaturesFilterType::Inertia);

  double clusterShade2 = filter->GetFeature(
                      HistogramToTextureFeaturesFilterType::ClusterShade);

  double clusterProminence2 = filter->GetFeature(
                      HistogramToTextureFeaturesFilterType::ClusterProminence);

  double haralickCorrelation2 = filter->GetFeature(
                      HistogramToTextureFeaturesFilterType::HaralickCorrelation);
  
  if( vnl_math_abs(energy2 - trueEnergy) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "Energy calculated wrong. Expected: " << trueEnergy << ", got: " 
      << energy2 << std::endl;
    passed = false;
    }
   
  if( vnl_math_abs(entropy2 - trueEntropy) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "Entropy calculated wrong. Expected: " << trueEntropy << ", got: "  
      << entropy2 << std::endl;
    passed = false;
    }
  
  if( vnl_math_abs(correlation2 - trueCorrelation) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "Correlation calculated wrong. Expected: " << trueCorrelation << 
      ", got: "  << correlation2 << std::endl;
    passed = false;
    }
  
  if( vnl_math_abs(inverseDifferenceMoment2 - trueInverseDifferenceMoment) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "InverseDifferenceMoment calculated wrong. Expected: " << 
      trueInverseDifferenceMoment <<  ", got: "  << inverseDifferenceMoment2 << std::endl;
    passed = false;
    }
  
  if( vnl_math_abs(inertia2 - trueInertia) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "Inertia calculated wrong. Expected: " << trueInertia << ", got: " 
      << inertia2 << std::endl;
    passed = false;
    }
  
  if( vnl_math_abs(clusterShade2 - trueClusterShade) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "ClusterShade calculated wrong. Expected: " << trueClusterShade << 
      ", got: "  << clusterShade2 << std::endl;
    passed = false;
    }

  if( vnl_math_abs(clusterProminence2 - trueClusterProminence) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "ClusterProminence calculated wrong. Expected: " 
      << trueClusterProminence << ", got: "  << clusterProminence2 << std::endl;
    passed = false;
    }
  
  if( vnl_math_abs(haralickCorrelation2 - trueHaralickCorrelation) > 0.001 )
    {
    std::cerr << "Error:" << std::endl;
    std::cerr << "Haralick's Correlation calculated wrong. Expected: "
      << trueHaralickCorrelation << ", got: "  << haralickCorrelation2 << std::endl;
    passed = false;
    }

  //Test inquiry for invalid feature

  if( filter->GetFeature( HistogramToTextureFeaturesFilterType::InvalidFeatureName ) ) 
    {
    std::cerr << "Error: " << std::endl;
    std::cerr << "GetFeature() is returing non-zero feature value: "
              << "for invalid feature request" << std::endl;
    passed = false;
    }
    
  
  if( !passed )
    {
    std::cerr << "Test failed" << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cerr << "Test succeeded" << std::endl;
    return EXIT_SUCCESS;
    }
}

