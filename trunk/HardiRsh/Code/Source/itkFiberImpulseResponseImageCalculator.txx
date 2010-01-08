
#ifndef __itkFiberImpulseResponseImageCalculator_txx
#define __itkFiberImpulseResponseImageCalculator_txx

#include "itkFiberImpulseResponseImageCalculator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkNumericTraits.h"

namespace itk
{ 
    
/**
 * Constructor
 */
template<class TGradientImagePixelType, typename TBasisType>
FiberImpulseResponseImageCalculator<TGradientImagePixelType,TBasisType>
::FiberImpulseResponseImageCalculator()
{
  m_BValue = 0;
  
}

template<class TGradientImagePixelType, typename TBasisType>
void
FiberImpulseResponseImageCalculator<TGradientImagePixelType,TBasisType>
::SetGradientImage( GradientDirectionContainerType *gradientDirection,
                        const GradientImagesType *gradientImage )
{

  this->m_GradientDirectionContainer = gradientDirection;
  this->m_GradientImage = gradientImage;

  //Find the baseline and the gradient indicies

  for(GradientDirectionContainerType::ConstIterator gdcit = this->m_GradientDirectionContainer->Begin();
      gdcit != this->m_GradientDirectionContainer->End(); ++gdcit)
  {

    if(gdcit.Value().one_norm() <= 0.0)
    {
      this->m_Baselineind.push_back( gdcit.Index() ); 
    }

    else if(gdcit.Value().one_norm() > 0.0)
    {
      this->m_Gradientind.push_back(gdcit.Index());
    }
  }

  this->m_NumGradients = this->m_Gradientind.size();
  this->m_NumBaselines = this->m_Baselineind.size();

  if ( this->m_NumBaselines == 0 )
  {
    itkExceptionMacro( << "No Baseline Images found!!!" );
  }

  
  // ensure that the gradient image we received has as many components as
  // the number of gradient directions
  if( gradientImage->GetVectorLength() != this->m_NumBaselines + this->m_NumGradients )
    {
    itkExceptionMacro( << this->m_NumGradients << " gradients + " << this->m_NumBaselines
                       << "baselines = " << this->m_NumGradients + this->m_NumBaselines
                       << " directions specified but image has " << gradientImage->GetVectorLength()
      << " components.");
    }

}


template<class TGradientImagePixelType, typename TBasisType>
typename FiberImpulseResponseImageCalculator<TGradientImagePixelType,TBasisType>::GradientDirectionContainerPointer
FiberImpulseResponseImageCalculator<TGradientImagePixelType,TBasisType>
::RotateGradientContainer(MatrixType rotMatrix) const
{
  //We want a full copy of the container at m_GradientDirectionContainer
  GradientDirectionContainerType::Pointer gradContainer = GradientDirectionContainerType::New();
  GradientDirectionType dir;

  for (unsigned int g = 0; g < m_GradientDirectionContainer->size(); ++g)
  {
    dir[0] = m_GradientDirectionContainer->GetElement(g)[0];
    dir[1] = m_GradientDirectionContainer->GetElement(g)[1];
    dir[2] = m_GradientDirectionContainer->GetElement(g)[2];
    
    dir = rotMatrix * dir;
//    dir = rotMatrix.transpose() * dir;

    if (dir.two_norm() > 0) 
      gradContainer->InsertElement(g,dir/dir.two_norm());
    else
      gradContainer->InsertElement(g,dir);
  }
  
  return static_cast<GradientDirectionContainerPointer>(gradContainer);
}


template<class TGradientImagePixelType, typename TBasisType>
typename FiberImpulseResponseImageCalculator<TGradientImagePixelType,TBasisType>::MatrixType
FiberImpulseResponseImageCalculator<TGradientImagePixelType,TBasisType>
::InitDtiSolver( GradientDirectionContainerPointer gradContainer )
{
  MatrixType dtiSolver;
  
  if( this->m_NumGradients < 6 )
  {
    itkExceptionMacro( << "Not enough gradient directions supplied. Need to supply at least 6" );
  }

  vnl_matrix< double > BMatrix;
  
  BMatrix.set_size( this->m_NumGradients, 6 );
  for (unsigned int m = 0; m < this->m_NumGradients; m++)
  {
    BMatrix[m][0] =     m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[0] * m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[0];
    BMatrix[m][1] = 2 * m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[0] * m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[1];
    BMatrix[m][2] = 2 * m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[0] * m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[2];
    BMatrix[m][3] =     m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[1] * m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[1];
    BMatrix[m][4] = 2 * m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[1] * m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[2];
    BMatrix[m][5] =     m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[2] * m_GradientDirectionContainer->ElementAt(m_Gradientind[m])[2];
  }

  vnl_svd<double> solver(BMatrix);
  dtiSolver = solver.inverse();
  return dtiSolver;  
}


template<class TGradientImagePixelType,typename TBasisType>
typename FiberImpulseResponseImageCalculator<TGradientImagePixelType,TBasisType>::MatrixType
FiberImpulseResponseImageCalculator<TGradientImagePixelType,TBasisType>
::InitRshSolver( GradientDirectionContainerPointer gradContainer )
{
  MatrixType rshSolver;
  
  unsigned int numberOfOrders = RshPixelType::NumberOfOrders;

  vnl_matrix< double >  rshBasis;
  rshBasis.set_size( this->m_NumGradients, numberOfOrders );
  
  GradientDirectionType dir;
  
  for (unsigned int m = 0; m < this->m_NumGradients; m++)
  {
    /*** Grad directions relation to theta phi
    * x = sin(theta) * cos(phi)
    * y = sin(theta) * sin(phi)
    * z = cos(theta)
    */
    dir = gradContainer->ElementAt(m_Gradientind[m]);
    
    double theta, phi;
    if ( dir[2] == 1) // z = 1
    {
      theta =0.0;
      phi   =0.0;
    }
    else
    {
      theta = vcl_acos (dir[2]);
      phi   = vcl_atan2(dir[1],dir[0]); // atan2(y,x) = atan(y/x);
    }

    for (unsigned int c = 0; c < numberOfOrders; c++)
    {
      rshBasis[m][c]  = RshPixelType::Y(2 * c, 0 ,theta,phi);
    }
  }

  vnl_svd<double> solver(rshBasis);
  rshSolver = solver.inverse();
  return rshSolver;
    
}


template<class TGradientImagePixelType,typename TBasisType>
void
FiberImpulseResponseImageCalculator<TGradientImagePixelType,TBasisType>
::Compute(void)
{
  std::cerr << "start FOD computatoin" << std::endl;
  
  //Make sure the GradientContainer is set!
  if (! m_GradientDirectionContainer || m_GradientDirectionContainer->size() == 0 )
  {
    itkExceptionMacro( << "Please Specify a Gradient Contatiner!" );
  }
  
  //Make sure the mask image was specified
  if ( ! m_ImageMask )
  {
   itkExceptionMacro( << "You must specifiy a mask Image!");
  }
  
  //Make sure the BValue
  if ( m_BValue == 0 )
  {
   itkExceptionMacro( << "You must specifiy a B Value!");
  }
  
  
  //Loop Through everything!
  ImageRegionConstIteratorWithIndex< GradientImagesType >
                  it( m_GradientImage, m_GradientImage->GetLargestPossibleRegion() );

  typename GradientImagesType::IndexType index;
  typename GradientImagesType::PointType inputPoint;
  
  DtiType tensor;

  MatrixType rshSolver;
  MatrixType dtiSolver = InitDtiSolver(m_GradientDirectionContainer);
  
  m_RespRSH.set_size(0);
  unsigned int counter = 0;
  while( !it.IsAtEnd() )
  {
    index = it.GetIndex();
    m_GradientImage->TransformIndexToPhysicalPoint( index, inputPoint );

    if( ! this->m_ImageMask->IsInside( inputPoint ) )
    {
      ++it;
      continue;
    }
    
    //Vecotor to hold S(g) and then to hold S(g) / S0
    vnl_vector<double> sig(m_NumGradients);
    vnl_vector<double> negLogSig(m_NumGradients);
    vnl_vector<double> D(6);

    
    //grab the whole vector, at this pixel, out of the GradientImage.
    GradientVectorType b = it.Get();
    
    double b0 = 0.0;
    // Average the baseline image pixels
    for(unsigned int i = 0; i < m_NumBaselines; ++i)
    {
      b0 += static_cast<double>(b[m_Baselineind[i]]);
    }
    b0 /=  m_NumBaselines;

    //Compute S/S0 and -log(s/s0) / bvalue
    for(unsigned int i = 0; i < m_NumGradients; ++i)
    {
      sig[i] =static_cast<double>(b[m_Gradientind[i]]) / b0;
      negLogSig[i] = -vcl_log( sig[i] ) / this->m_BValue;
    }

    //Compute Tensor
    D = dtiSolver * negLogSig;
    int skipVox = 0;
    for (unsigned int i=0;i<6;++i)
    {
      if ( vnl_math_isnan(D[i]) || vnl_math_isinf(D[i]) )
      {
        //I'm not sure why this happens? quite confusing
        ///TODO investigate this
        
        //~ std::cerr << dtiSolver << std::endl << std::endl;
        //~ std::cerr << negLogSig << std::endl << std::endl;
        //~ std::cerr << D << std::endl;
        //~ itkExceptionMacro( << "Error in Pixel " << index << ", caught nans or infs");
        skipVox = 1;
      }
    }
    
    if (skipVox)
    {
      ++it;
      continue;
    }
    
    tensor[0] = D[0];
    tensor[1] = D[1];
    tensor[2] = D[2];
    tensor[3] = D[3];
    tensor[4] = D[4];
    tensor[5] = D[5];

    typename DtiType::EigenValuesArrayType     lambda;
    typename DtiType::EigenVectorsMatrixType   vMat;
    
    tensor.ComputeEigenAnalysis(lambda, vMat);

    std::cerr << "Fa : " << tensor.GetFractionalAnisotropy() << std::endl;
    //Compute FA (This is the slow way to do this)
    if ( tensor.GetFractionalAnisotropy() < this->GetFAThreshold() )
    {
      //SKIP!
      ++it;
      continue;
    }

    GradientDirectionContainerPointer
            gradContainer = RotateGradientContainer(vMat.GetVnlMatrix());
  
    rshSolver = InitRshSolver(gradContainer);

    if ( m_RespRSH.size() == 0)
    {
      m_RespRSH  = rshSolver * sig;
    }
    else
    {
      m_RespRSH += rshSolver * sig;
    }
    counter++;
    ++it;
  }
  
  if (counter == 0)
  {
    std::cerr << "No Voxels made the list!!" << std::endl;
    itkExceptionMacro( << "No Voxels made the list!!");
    std::cerr << "Post Exception!!" << std::endl;
  }

  m_RespRSH /= counter;

  std::cerr << "end FOD computatoin" << std::endl;
  
}
 
 
template<class TGradientImagePixelType,typename TBasisType>
void
FiberImpulseResponseImageCalculator<TGradientImagePixelType,TBasisType>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
}

} // end namespace itk

#endif
