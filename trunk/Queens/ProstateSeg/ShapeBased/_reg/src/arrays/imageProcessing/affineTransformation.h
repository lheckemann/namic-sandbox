#ifndef affineTransformation_h_
#define affineTransformation_h_


#include "itkImage.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"


/*************************************************************
   affine transform */
template< typename TPixel, unsigned int dim > 
typename itk::Image< TPixel, dim >::Pointer 
affineTransform(typename itk::Image< TPixel, dim >::Pointer image,      \
                vnl_matrix< double > A,                                 \
                vnl_vector< double > translation,                       \
                TPixel fillValue,                                       \
                char interpType,                                        \
                typename itk::Image< TPixel, dim >::SizeType newSize);




#include "affineTransformation.txx"

#endif
