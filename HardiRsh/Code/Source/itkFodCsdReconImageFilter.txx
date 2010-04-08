
#ifndef __itkFodCsdReconImageFilter_txx
#define __itkFodCsdReconImageFilter_txx

#include "itkFodCsdReconImageFilter.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_math.h"
#include "vnl/algo/vnl_qr.h"

#include <cmath>

namespace itk {

template< class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
        >
FodCsdReconImageFilter< TGradientImagePixelType, TOutputPixelType, TImageDimension, TResidualPercisionType >
::FodCsdReconImageFilter()
{

  m_NumIters = 50;
  m_NumConstraintGrads = 300;
  m_Threshold = 0.1;
  m_Regulizer = 1.0;
  this->SetBeltramiLambda(0.0);
  m_UseCSD = true;
  m_Normalize = false;
  
  this->AllowSuperResolutionOn();
  
  //Default is [1 1 1 0 0 0 0] For use with CSD
  m_RecipricalFilter = VectorType( OutputPixelType::NumberOfOrders, 0.0 );
  switch (OutputPixelType::NumberOfOrders)
  {
    default:
    case 2:
      m_RecipricalFilter[2] = 1.0;
    case 1:
      m_RecipricalFilter[1] = 1.0;
    case 0:
      m_RecipricalFilter[0] = 1.0;
      break;
  }
}

template< class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
          >
void
FodCsdReconImageFilter< TGradientImagePixelType, TOutputPixelType, TImageDimension,TResidualPercisionType >
::ComputeRshBasis()
{
  
  if ( this->GetResponseRSH().empty() )
  {
    itkExceptionMacro( << "fiber impulse response function Not set" );
  }

  //First call the super class method to compute the BasisMatrix
  Superclass::ComputeRshBasis();
  VectorType respRsh = this->GetResponseRSH();
  
  int respRsh_size = respRsh.size();

  /* multiple the rows of m_RshBasis by the correct part of the FIR Resp*/
  for (unsigned int i=0; i<NumberOfCoefficients; ++i)
  {
    LmVector lm = OutputPixelType::GetLM(i+1);
    if (lm[0]/2 < respRsh_size)
    {
      this->m_RshBasis.scale_column(
                    i,respRsh[lm[0]/2] / vcl_sqrt((2 * lm[0]+1) / (4 * vnl_math::pi) ) );
    }
    else
    {
      this->m_RshBasis.scale_column( i, 0.0 );      
    }
    
  }

  //compute the psuedo inverse! Ignoring m_lambda
  vnl_svd<double> solver(this->m_RshBasis);
  this->m_RshBasisPseudoInverse = solver.inverse();

  /* Apply the inverse filter!*/
  for (unsigned int i=0; i<NumberOfCoefficients; ++i)
  {
    LmVector lm = OutputPixelType::GetLM(i+1);
    this->m_RshBasisPseudoInverse.scale_row(i,m_RecipricalFilter[lm[0]/2]);
  }
    
  if (this->GetUseCSD())
  {
    //Compute the hiResRshBasis matrix
    /**
     * uses the formula in [saff:dmp:1997] to generate equidistributed points on the sphere.
     * Basically a squared spiral
     * 
     * REFERENCE
     * @Article{saff:dmp:1997,
     * author =          {Saff, E. B. and Kuijlaars, A. B. J.},
     * title =          {Distributing Many Points on a Sphere},
     * journal =          {Math. Intell.},
     * year =          {1997},
     * volume =          {19},
     * number =          {1},
     * pages =          {5--11},
     **/
    
    this->m_HiResRshBasis.set_size( m_NumConstraintGrads, NumberOfCoefficients );

    double theta, phi, lastPhi=0.0, h;

    for (unsigned int m = 0; m < m_NumConstraintGrads; m++)
    {
      /*** Grad directions relation to theta phi
      * x = sin(theta) * cos(phi)
      * y = sin(theta) * sin(phi)
      * z = cos(theta)
      */

      h = -1 + 2*( m )/ static_cast<double>( m_NumConstraintGrads -1 );
      theta = vcl_acos(h);
      
      if (m == 0 or m == m_NumConstraintGrads -1)
      {
        phi = 0.0;
      }
      else
      {
        phi = vcl_fmod( lastPhi + 3.6 / vcl_sqrt( m_NumConstraintGrads * (1- vcl_pow(h,2) ) ) 
                                          , ( 2 * vnl_math::pi ) );
      }

      for (unsigned int c = 0; c < NumberOfCoefficients; c++)
        this->m_HiResRshBasis[m][c]  = OutputPixelType::Y(c+1,theta,phi);

      lastPhi = phi;
    }
  }
}

template< class TGradientImagePixelType,
          class TOutputPixelType,
          unsigned int TImageDimension,
          class TResidualPercisionType
        >
vnl_vector< double >
FodCsdReconImageFilter< TGradientImagePixelType, TOutputPixelType, TImageDimension, TResidualPercisionType >
::ComputeCoeffsFromSignal( vnl_vector< double > signal, ResidualPixelType& residual )
{
  
  vnl_vector< double > coeffs =  this->m_RshBasisPseudoInverse * signal;

  if (this->GetUseCSD())
  {

    VectorType testSig = this->m_HiResRshBasis * coeffs;
    vnl_vector<double> lastCoeffs = coeffs;

    lastCoeffs.fill(0);
    
    double threshold = this->GetThreshold()*testSig.mean();

    RshBasisMatrixType fullMat;
    VectorType expandedSignal;
    
    //Make fullMat as big as it might need to be!
    fullMat.set_size(this->m_HiResRshBasis.rows() + this->m_RshBasis.rows(), NumberOfCoefficients);
    
    for (unsigned int i = 0; i < this->m_RshBasis.rows() ; ++i)
    {
      fullMat.set_row(i,this->m_RshBasis.get_row(i));
    }
    
    expandedSignal.set_size(this->m_HiResRshBasis.rows() + this->m_RshBasis.rows());
    expandedSignal.fill(0.0);
    expandedSignal.update(signal,0);
 
    std::vector<unsigned int> badInds = std::vector<unsigned int>();
    unsigned int iterations = 0;
    unsigned int numBad;
    while( iterations <= m_NumIters )
    {
      //reload the badInds vector!
      badInds.clear();
      testSig = this->m_HiResRshBasis * coeffs;

      for (unsigned int i = 0; i < m_NumConstraintGrads ; ++i)
      {
        if ( testSig[i] < threshold )
        {
          badInds.push_back(i);
        }
      }
      numBad = badInds.size();

      ///TODO check for underdetermined system!
      
      for (unsigned int i = 0; i < this->m_HiResRshBasis.rows() ;++i )
      {
        if ( i < numBad )
        {
          fullMat.set_row( this->m_RshBasis.rows()+i,this->m_HiResRshBasis.get_row(badInds[i]) );
        }
        else
        {
          fullMat.set_row( this->m_RshBasis.rows()+i, 0.0 );
        }
      }
      if ( this->m_RshBasis.rows()+numBad < NumberOfCoefficients)
      {
        std::cerr << "n Constraints        : " << this->m_RshBasis.rows()+numBad << std::endl;
        std::cerr << "NumberOfCoefficients : " << NumberOfCoefficients << std::endl;
        
        itkExceptionMacro( << "System is underdetermined\nNot enought constaints to solve system!" );
      }

      //Should try to check the condition number of the matrix!
      vnl_qr<double> solver( fullMat.extract( this->m_RshBasis.rows()+numBad, NumberOfCoefficients, 0, 0) );
      coeffs = solver.solve(expandedSignal.extract( this->m_RshBasis.rows()+numBad, 0) );

      //Stop when coeffs stop changing
      if ( lastCoeffs == coeffs )
      {
        break;
      }
      else
      {
        lastCoeffs = coeffs;
      }
      ++iterations;
    }

    if ( iterations == m_NumIters)
    {
      std::cerr << "CSD Failed to Converge" << std::endl;
      coeffs.fill(0.0);
    }
    
    itkDebugMacro( << "CSD converged in " << iterations << " iterations");
  }
  
  
  if (this->GetNormalize())
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
