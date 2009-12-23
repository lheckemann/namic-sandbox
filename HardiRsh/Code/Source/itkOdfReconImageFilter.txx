
#ifndef __itkOdfReconImageFilter_txx
#define __itkOdfReconImageFilter_txx

#include "itkOdfReconImageFilter.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_math.h"
#include "itkReplaceSpecialFunctions.h"

namespace itk {

template< 
          class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
        >
OdfReconImageFilter< TGradientImagePixelType, TOutputPixelType, TImageDimension, TResidualPercisionType >
::OdfReconImageFilter()
{
  m_Normalize = true;
  this->AllowSuperResolutionOff();
}

template< 
          class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
        >
vnl_vector< double >
OdfReconImageFilter<  TGradientImagePixelType, TOutputPixelType, TImageDimension, TResidualPercisionType >
::ComputeCoeffsFromSignal( vnl_vector< double > signal, ResidualPixelType& residual )
{

  //call the superclass method to compute the RSH coeffs of the signal!
  vnl_vector< double > coeffs = Superclass::ComputeCoeffsFromSignal(signal, residual);

  //convert the rshcoeffs to odf coeffs
  for( unsigned int i=0; i<OutputPixelType::Dimension; i++){
    int l = (OutputPixelType::BasisType::GetLM(i+1))[0];
    coeffs[i] = static_cast<double>( 2.0 * vnl_math::pi * LegendreP( l , 0, 0 ) * coeffs[i] );
  }

  if (m_Normalize)
  {
    //only normalize if 0th order is non zero...
    if ( coeffs[0] != 0)
    {
      double nFactor = 1.0 / ( 2 * vcl_sqrt( vnl_math::pi ) * coeffs[0]);
      for( unsigned int i=0; i< OutputPixelType::Dimension; i++)
        coeffs[i] = nFactor * coeffs[i];
    }
  }

  return coeffs;
}

}

#endif
