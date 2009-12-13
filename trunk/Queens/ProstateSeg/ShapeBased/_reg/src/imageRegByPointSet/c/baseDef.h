#ifndef baseDef_h_
#define baseDef_h_

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

#include <vector>

#include "itkImage.h"

#include "boost/shared_ptr.hpp"


////////////////////////////////////////////////////////////
typedef vnl_vector<double> VnlDoubleVectorType;
typedef boost::shared_ptr< VnlDoubleVectorType > VnlDoubleVectorPointerType;

typedef vnl_matrix<double> VnlDoubleMatrixType;
typedef boost::shared_ptr< VnlDoubleMatrixType > VnlDoubleMatrixPointerType;


////////////////////////////////////////////////////////////
typedef vnl_vector_fixed< double, 2> VnlDoubleVector2DType;
typedef vnl_matrix_fixed< double, 2, 2> VnlDoubleMatrix2x2Type;


////////////////////////////////////////////////////////////
typedef vnl_vector_fixed< double, 3> VnlDoubleVector3DType;
typedef boost::shared_ptr< VnlDoubleVector3DType > VnlDoubleVector3DPointerType;

typedef vnl_matrix_fixed< double, 3, 3> VnlDoubleMatrix3x3Type;
typedef boost::shared_ptr< VnlDoubleMatrix3x3Type > VnlDoubleMatrix3x3PointerType;

////////////////////////////////////////////////////////////
typedef itk::Image< double, 2 > DoubleImage2DType;
typedef DoubleImage2DType::Pointer DoubleImage2DPointerType;


////////////////////////////////////////////////////////////
typedef itk::Image< double, 3 > DoubleImage3DType;
typedef DoubleImage3DType::Pointer DoubleImage3DPointerType;

////////////////////////////////////////////////////////////
typedef itk::Image< unsigned char, 3 > UCHARImage3DType;
typedef UCHARImage3DType::Pointer UCHARImage3DPointerType;

////////////////////////////////////////////////////////////
typedef itk::Image< unsigned char, 2 > UCHARImage2DType;
typedef UCHARImage2DType::Pointer UCHARImage2DPointerType;

#endif

