#ifndef affineTransformation_txx_
#define affineTransformation_txx_


#include "affineTransformation.h"


#include "itkImage.h"

#include "itkResampleImageFilter.h"
#include "itkAffineTransform.h"

#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"



/*************************************************************
   affine transform */
template< typename TPixel, unsigned int dim > 
typename itk::Image< TPixel, dim >::Pointer 
affineTransform(typename itk::Image< TPixel, dim >::Pointer image,      \
                vnl_matrix< double > A,                                 \
                vnl_vector< double > translation,                       \
                TPixel fillValue,                                       \
                char interpType,                                        \
                typename itk::Image< TPixel, dim >::SizeType newSize)
{
  if (dim != A.rows() || dim != A.columns() || dim != translation.size())
    {
      std::cerr<<"Error: dim != A.rows() || dim != A.columns() || dim != translation.size()\n";
      raise(SIGABRT);
    }

  typedef itk::Image<TPixel, dim> TImage;
  typedef typename TImage::Pointer TImagePointer;


  typedef itk::AffineTransform< double, dim> TAffineTransform;
  typename TAffineTransform::Pointer aff = TAffineTransform::New();
  typename TAffineTransform::MatrixType AA(A);
  typename TAffineTransform::OutputVectorType AT;
  AT.SetVnlVector(translation);

  std::cout<<"AA = "<<AA<<std::endl;
  std::cout<<"AT = "<<AT<<std::endl;


  aff->SetMatrix(AA);
  aff->SetTranslation(AT);
  
  typename itk::InterpolateImageFunction<TImage, double>::Pointer interp;

  if (interpType == 0)
    {
      // NN interpolation
      typedef itk::NearestNeighborInterpolateImageFunction<TImage, double> InterpolatorType;
      interp = InterpolatorType::New();
    }
  else if(interpType == 1)
    {
      // linear 
      typedef itk::LinearInterpolateImageFunction<TImage, double> InterpolatorType;
      interp = InterpolatorType::New();
    }
  else if(interpType == 2)
    {
      // bspline
      typedef itk::BSplineInterpolateImageFunction<TImage, double, double> InterpolatorType;
      interp = InterpolatorType::New();
    }
  interp->SetInputImage(image);


  typedef itk::ResampleImageFilter< TImage, TImage > TResampleFilter;
  typename TResampleFilter::Pointer resampler = TResampleFilter::New();
  resampler->SetInput( image );
  resampler->SetSize(newSize);
  resampler->SetTransform(aff);
  resampler->SetInterpolator(interp);

  resampler->SetOutputOrigin(  image->GetOrigin() );
  resampler->SetOutputSpacing( image->GetSpacing() );
  resampler->SetOutputDirection( image->GetDirection() );
  resampler->SetDefaultPixelValue( fillValue );

  resampler->Update();


  return resampler->GetOutput();
}

#endif
