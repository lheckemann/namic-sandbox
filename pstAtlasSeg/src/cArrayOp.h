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

namespace douher
{
  /* IO: image file <===> itk::Image  */
  template< typename T > typename itk::Image< T, 2 >::Pointer readImage2(const char *fileName);
  template< typename T > typename itk::Image< T, 3 >::Pointer readImage3(const char *fileName);
  template< typename T, unsigned int dim > typename itk::Image< T, dim >::Pointer readImage(const char *fileName);

  template< typename T > void writeImage2(typename itk::Image< T, 2 >::Pointer img, const char *fileName);
  template< typename T > void writeImage3(typename itk::Image< T, 3 >::Pointer img, const char *fileName);
  template< typename T, unsigned int dim > void writeImage(typename itk::Image< T, dim >::Pointer img, const char *fileName);
}

#include "cArrayOp.txx"

#endif
/* definition in .txx*/
