/* ==================================================
   Operations of cArray classes.

   I'm not sure how much including those operations into cArray
   objects would harm their speed, so I just separate them here.

   20081031 Yi Gao

   ================================================== */

#ifndef cArrayOp_h_
#define cArrayOp_h_

#include "itkImage.h"

#include <vnl/vnl_vector.h>

#include "boost/shared_ptr.hpp"

#include <cassert>

#include <csignal>


#include "cArray3D.h"


namespace newProstate
{
//  /* IO with images   */
//  template< typename T >
//  typename cArray3D< T >::PointerType readImageToArray3(const char *fileName);
//
//  template< typename T >
//  void saveAsImage3(typename cArray3D< T >::PointerType array3, const char *fileName);
//

  /* IO: image file <===> itk::Image  */
  template< typename T > typename itk::Image< T, 3 >::Pointer readImage3(const char *fileName);
  template< typename T, unsigned int dim > typename itk::Image< T, dim >::Pointer readImage(const char *fileName);

  template< typename T > void writeImage3(typename itk::Image< T, 3 >::Pointer img, const char *fileName);
  template< typename T, unsigned int dim > void writeImage(typename itk::Image< T, dim >::Pointer img, const char *fileName);


  /* IO: read series of images */
  template< typename TData > 
  std::vector< typename itk::Image< TData, 3 >::Pointer >
  readImageSeries3( const std::vector< std::string >& imageNameList );

  template< typename TData, unsigned int dim > 
  std::vector< typename itk::Image< TData, dim >::Pointer >
  readImageSeries( const std::vector< std::string >& imageNameList );


  /* IO: vnl_vector <===> itk::Image  */
  template< typename T, unsigned int dim > 
  vnl_vector< T > itkImageToVnlVector(typename itk::Image< T, dim >::Pointer img);

  template< typename T, unsigned int dim > 
  vnl_vector< T > itkImageToVnlVector(typename itk::Image< T, dim >::ConstPointer img);


  template< typename T, unsigned int dim > 
  typename itk::Image< T, dim >::Pointer vnlVectorToItkImage(const vnl_vector< T >& vct, long imSize[]);


  /*  IO: cArray <===> itk::Image  */
  template< typename T > 
  typename cArray3D< T >::PointerType itkImageToArray3(typename itk::Image< T, 3 >::Pointer image);

  template< typename T > 
  typename itk::Image< T, 3 >::Pointer cArray3ToItkImage(typename cArray3D< T >::PointerType array3);

}

#include "cArrayOp.txx"

#endif
/* definition in .txx*/
