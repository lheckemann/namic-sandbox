/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkScalarImageToTextureFeaturesFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/05/10 05:33:48 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkScalarImageToTextureFeaturesFilter_txx
#define __itkScalarImageToTextureFeaturesFilter_txx

#include "itkScalarImageToTextureFeaturesFilter.h"
#include "itkNeighborhood.h"
#include "vnl/vnl_math.h"

namespace itk {
namespace Statistics {

template< class TImage, class THistogramFrequencyContainer >
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::
ScalarImageToTextureFeaturesFilter()
{
  this->SetNumberOfRequiredInputs(1);
  this->SetNumberOfRequiredOutputs(1);

  for (int i=0; i < 2; ++i)
    {
    typename FeatureValueVectorDataObjectType::Pointer output
      = static_cast<FeatureValueVectorDataObjectType*>(this->MakeOutput().GetPointer());
    this->ProcessObject::SetNthOutput(i, output.GetPointer());
    }

  m_GLCMGenerator = CooccurrenceMatrixFilterType::New();
  m_FeatureMeans = FeatureValueVector::New();
  m_FeatureStandardDeviations = FeatureValueVector::New();
      
  // Set the requested features to the default value:
  // {Energy, Entropy, InverseDifferenceMoment, Inertia, ClusterShade, ClusterProminence}
  FeatureNameVectorPointer requestedFeatures = FeatureNameVector::New(); 
  // can't directly set m_RequestedFeatures since it is const!
  requestedFeatures->push_back(TextureFeaturesFilterType::Energy);
  requestedFeatures->push_back(TextureFeaturesFilterType::Entropy);
  requestedFeatures->push_back(TextureFeaturesFilterType::InverseDifferenceMoment);
  requestedFeatures->push_back(TextureFeaturesFilterType::Inertia);
  requestedFeatures->push_back(TextureFeaturesFilterType::ClusterShade);
  requestedFeatures->push_back(TextureFeaturesFilterType::ClusterProminence);
  this->SetRequestedFeatures(requestedFeatures);
  
  // Set the offset directions to their defaults: half of all the possible
  // directions 1 pixel away. (The other half is included by symmetry.)
  // We use a neighborhood iterator to calculate the appropriate offsets.
  typedef Neighborhood<ITK_TYPENAME ImageType::PixelType, ::itk::GetImageDimension< 
    ImageType >::ImageDimension > NeighborhoodType;
  NeighborhoodType hood;
  hood.SetRadius(1);
      
  // select all "previous" neighbors that are face+edge+vertex
  // connected to the current pixel. do not include the center pixel.
  unsigned int centerIndex = hood.GetCenterNeighborhoodIndex();
  OffsetType offset;
  OffsetVectorPointer offsets = OffsetVector::New();
  for (unsigned int d=0; d < centerIndex; d++)
    {
    offset = hood.GetOffset(d);
    offsets->push_back(offset);
    }
  this->SetOffsets(offsets);
  m_FastCalculations = false;
}

template< class TImage, class THistogramFrequencyContainer >
typename
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer > ::DataObjectPointer
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >
::MakeOutput( void )
{
  return static_cast<DataObject*>(FeatureValueVectorDataObjectType::New().GetPointer());
}


template< class TImage, class THistogramFrequencyContainer >
void
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::
GenerateData(void)
{
  const ImageType *input = this->GetInput();

  if( input == NULL )
    {
    itkExceptionMacro("Input image has not been set yet");
    }

  if (m_FastCalculations) 
    {
    this->FastCompute();
    }
  else 
    {
    this->FullCompute();
    }
}  

template< class TImage, class THistogramFrequencyContainer >
void
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::
FullCompute(void)
{
  int numOffsets = m_Offsets->size();
  int numFeatures = m_RequestedFeatures->size();
  double **features;

  features = new double *[numOffsets];
  for (int i = 0; i < numOffsets; i++)
    {
    features[i] = new double [numFeatures];
    }
      
  // For each offset, calculate each feature
  typename OffsetVector::ConstIterator offsetIt;
  int offsetNum, featureNum;
      
  for(offsetIt = m_Offsets->Begin(), offsetNum = 0;
      offsetIt != m_Offsets->End(); offsetIt++, offsetNum++)
    {
    m_GLCMGenerator->SetOffset(offsetIt.Value());
    m_GLCMGenerator->Update();
    typename TextureFeaturesFilterType::Pointer glcmCalc = TextureFeaturesFilterType::New();
    glcmCalc->SetInput(m_GLCMGenerator->GetOutput());
    glcmCalc->Update();
        
    typename FeatureNameVector::ConstIterator fnameIt;
    for(fnameIt = m_RequestedFeatures->Begin(), featureNum = 0; 
        fnameIt != m_RequestedFeatures->End(); fnameIt++, featureNum++)
      {
      features[offsetNum][featureNum] = glcmCalc->GetFeature(fnameIt.Value());
      }
    }
      
  // Now get the mean and deviaton of each feature across the offsets.
  m_FeatureMeans->clear();
  m_FeatureStandardDeviations->clear();
  double *tempFeatureMeans = new double [numFeatures];
  double *tempFeatureDevs = new double [numFeatures];
      
  /*Compute incremental mean and SD, a la Knuth, "The  Art of Computer 
    Programming, Volume 2: Seminumerical Algorithms",  section 4.2.2. 
    Compute mean and standard deviation using the recurrence relation:
    M(1) = x(1), M(k) = M(k-1) + (x(k) - M(k-1) ) / k
    S(1) = 0, S(k) = S(k-1) + (x(k) - M(k-1)) * (x(k) - M(k))
    for 2 <= k <= n, then
    sigma = vcl_sqrt(S(n) / n) (or divide by n-1 for sample SD instead of
    population SD).
  */
      
  // Set up the initial conditions (k = 1)
  for (featureNum = 0; featureNum < numFeatures; featureNum++)
    {
    tempFeatureMeans[featureNum] = features[0][featureNum];
    tempFeatureDevs[featureNum] = 0;
    }
  // Run through the recurrence (k = 2 ... N)
  for (offsetNum = 1; offsetNum < numOffsets; offsetNum++)
    {
    int k = offsetNum + 1;
    for (featureNum = 0; featureNum < numFeatures; featureNum++)
      {
      double M_k_minus_1 = tempFeatureMeans[featureNum];
      double S_k_minus_1 = tempFeatureDevs[featureNum];
      double x_k = features[offsetNum][featureNum];
          
      double M_k = M_k_minus_1 + (x_k - M_k_minus_1) / k;
      double S_k = S_k_minus_1 + (x_k - M_k_minus_1) * (x_k - M_k);
          
      tempFeatureMeans[featureNum] = M_k;
      tempFeatureDevs[featureNum] = S_k;
      }
    }
  for (featureNum = 0; featureNum < numFeatures; featureNum++)
    {
    tempFeatureDevs[featureNum] = vcl_sqrt(tempFeatureDevs[featureNum] / numOffsets);
        
    m_FeatureMeans->push_back(tempFeatureMeans[featureNum]);
    m_FeatureStandardDeviations->push_back(tempFeatureDevs[featureNum]);
    }

  FeatureValueVectorDataObjectType* meanOutputObject=
                   static_cast<FeatureValueVectorDataObjectType*>(this->ProcessObject::GetOutput(0));
  meanOutputObject->Set( m_FeatureMeans );

  FeatureValueVectorDataObjectType* standardDeviationOutputObject=
                   static_cast<FeatureValueVectorDataObjectType*>(this->ProcessObject::GetOutput(1));
  standardDeviationOutputObject->Set( m_FeatureStandardDeviations );

  delete [] tempFeatureMeans;
  delete [] tempFeatureDevs;
  for(int i=0; i < numOffsets; i++)
    {
    delete [] features[i];
    }
  delete[] features;
}
    
template< class TImage, class THistogramFrequencyContainer >
void
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::
FastCompute(void)
{
  // For each offset, calculate each feature
  typename OffsetVector::ConstIterator offsetIt;
  for(offsetIt = m_Offsets->Begin(); offsetIt != m_Offsets->End(); offsetIt++)
    {
    m_GLCMGenerator->SetOffset(offsetIt.Value());
    }
  
  m_GLCMGenerator->Update();
  typename TextureFeaturesFilterType::Pointer glcmCalc = TextureFeaturesFilterType::New();
  glcmCalc->SetInput(m_GLCMGenerator->GetOutput());
  glcmCalc->Update();
  
  m_FeatureMeans->clear();
  m_FeatureStandardDeviations->clear();
  typename FeatureNameVector::ConstIterator fnameIt;
  for(fnameIt = m_RequestedFeatures->Begin(); 
      fnameIt != m_RequestedFeatures->End(); fnameIt++)
    {
    m_FeatureMeans->push_back(glcmCalc->GetFeature(fnameIt.Value()));
    m_FeatureStandardDeviations->push_back(0.0);
    }

  FeatureValueVectorDataObjectType* meanOutputObject=
                   static_cast<FeatureValueVectorDataObjectType*>(this->ProcessObject::GetOutput(0));
  meanOutputObject->Set( m_FeatureMeans );

  FeatureValueVectorDataObjectType* standardDeviationOutputObject=
                   static_cast<FeatureValueVectorDataObjectType*>(this->ProcessObject::GetOutput(1));
  standardDeviationOutputObject->Set( m_FeatureStandardDeviations );

}


template< class TImage, class THistogramFrequencyContainer >
void
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::
SetInput( const ImageType * image )
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< ImageType* >( image ) );

  m_GLCMGenerator->SetInput(image);
}
    
template< class TImage, class THistogramFrequencyContainer >
void
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::
SetNumberOfBinsPerAxis( unsigned int numberOfBins )
{
  itkDebugMacro("setting NumberOfBinsPerAxis to " << numberOfBins);
  m_GLCMGenerator->SetNumberOfBinsPerAxis(numberOfBins);
  this->Modified();
}
    
template< class TImage, class THistogramFrequencyContainer >
void
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::
SetPixelValueMinMax( PixelType min, PixelType max )
{
  itkDebugMacro("setting Min to " << min << "and Max to " << max);
  m_GLCMGenerator->SetPixelValueMinMax(min, max);
  this->Modified();
}
    
template< class TImage, class THistogramFrequencyContainer >
void
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::
SetMaskImage( const ImageType* image)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(1, 
                                   const_cast< ImageType* >( image ) );

  m_GLCMGenerator->SetImageMask(image);
}

template< class TImage, class THistogramFrequencyContainer >
const TImage *
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >
::GetInput() const
{
  if (this->GetNumberOfInputs() < 1)
    {
    return 0;
    }
  
  return static_cast<const ImageType * >
    (this->ProcessObject::GetInput(0) );
}  

template< class TImage, class THistogramFrequencyContainer >
const typename  
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::FeatureValueVectorDataObjectType*
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::GetFeatureMeansOutput() const
{
  return static_cast< const FeatureValueVectorDataObjectType*>(this->ProcessObject::GetOutput(0));
}

template< class TImage, class THistogramFrequencyContainer >
const typename  
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::FeatureValueVectorDataObjectType*
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >
::GetFeatureStandardDeviationsOutput() const
{
  return static_cast< const FeatureValueVectorDataObjectType*>(this->ProcessObject::GetOutput(1));
}

template< class TImage, class THistogramFrequencyContainer >
const TImage *
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >
::GetMaskImage() const
{
  if (this->GetNumberOfInputs() < 2)
    {
    return 0;
    }
  
  return static_cast<const ImageType * >
    (this->ProcessObject::GetInput(1) );
}


   
template< class TImage, class THistogramFrequencyContainer >
void
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::
SetInsidePixelValue(PixelType insidePixelValue)
{
  itkDebugMacro("setting InsidePixelValue to " << insidePixelValue);
  m_GLCMGenerator->SetInsidePixelValue(insidePixelValue);
  this->Modified();
}
    
template< class TImage, class THistogramFrequencyContainer >
void
ScalarImageToTextureFeaturesFilter< TImage, THistogramFrequencyContainer >::
PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}
    
} // end of namespace Statistics 

} // end of namespace itk 


#endif
