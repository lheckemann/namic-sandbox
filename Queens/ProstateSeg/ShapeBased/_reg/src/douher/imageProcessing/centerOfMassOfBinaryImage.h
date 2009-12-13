#ifndef centerOfMassOfBinaryImage_h_
#define centerOfMassOfBinaryImage_h_

#include "../cArrayOp.h"

namespace douher
{
  /* Compute the center of mass of a binary image where 0 is
     background and 1 is the foreground. The binary constraint is NOT
     checked. */

  /*--------------------------------------------------
    centerOfMass  */
  template< typename T, unsigned int dim > 
  vnl_vector<double> computeCenterOfMass(typename itk::Image< T, dim >::Pointer image, double& totalMass = 0, bool relative = true);


  /*--------------------------------------------------
    centerOfMass2  */
  template< typename T > 
  vnl_vector<double> computeCenterOfMass2(typename itk::Image< T, 2 >::Pointer image, double& totalMass = 0, bool relative = true);


  /*--------------------------------------------------
    centerOfMass3  */
  template< typename T > 
  vnl_vector<double> computeCenterOfMass3(typename itk::Image< T, 3 >::Pointer image, double& totalMass = 0, bool relative = true);


} //douher

#include "centerOfMassOfBinaryImage.txx"


#endif
