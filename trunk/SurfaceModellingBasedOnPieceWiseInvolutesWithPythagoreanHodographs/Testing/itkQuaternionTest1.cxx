/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuaternionTest.cxx,v $
  Language:  C++
  Date:      $Date: 2007/12/20 19:19:18 $
  Version:   $Revision: 1.19 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
/**
 *  
 *  This program illustrates the use of Quaternions
 *  
 *  Quaternions are Unit Quaternions used to represent
 *  rotations. 
 *
 */


#include "itkQuaternion.h"
#include <iostream>



//-------------------------
//
//   Main code
//
//-------------------------
int main(int, char* [] ) 
{

  typedef   double          ValueType;

  const ValueType epsilon = 1e-12;


  //  Quaternion type
  typedef    itk::Quaternion< ValueType >    QuaternionType;


  //  Vector type
  typedef    QuaternionType::VectorType      VectorType;


  //  Point type
  typedef    QuaternionType::PointType      PointType;


  //  Covariant Vector type
  typedef    QuaternionType::CovariantVectorType      CovariantVectorType;


  //  VnlVector type
  typedef    QuaternionType::VnlVectorType       VnlVectorType;


  //  VnlQuaternion type
  typedef    QuaternionType::VnlQuaternionType   VnlQuaternionType;


  //  Matrix type
  typedef    QuaternionType::MatrixType          MatrixType;




  {
    std::cout << "Test default constructor... ";
    QuaternionType qa;
    if( fabs(qa.GetX()) > epsilon ) 
      {
      std::cout << "Error ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(qa.GetY()) > epsilon ) 
      {
      std::cout << "Error ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(qa.GetZ()) > epsilon ) 
      {
      std::cout << "Error ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(qa.GetW()-1.0) > epsilon ) 
      {
      std::cout << "Error ! " << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;
  }


  {
    std::cout << "Test initialization and GetMatrix()... ";
    QuaternionType qa;
    qa.SetIdentity();
    MatrixType ma = qa.GetMatrix();
    std::cout << "Matrix = " << std::endl;
    std::cout <<    ma       << std::endl;
  }



  {
    std::cout << "Test for setting Axis and Angle...";
    QuaternionType qa;
    VectorType xa;
    xa[0] = 2.5;
    xa[1] = 1.5;
    xa[2] = 0.5;
    ValueType angle = atan(1.0)/3.0; // 15 degrees in radians
    
    qa.Set( xa, angle );
        
    xa.Normalize();

    ValueType cosangle = cos( angle / 2.0 );
    ValueType sinangle = sin( angle / 2.0 );

    VectorType xb;

    xb =  xa * sinangle;

    if( fabs(qa.GetX()-xb[0]) > epsilon ) 
      {
      std::cout << "Error in X ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(qa.GetY()-xb[1]) > epsilon ) 
      {
      std::cout << "Error in Y ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(qa.GetZ()-xb[2]) > epsilon ) 
      {
      std::cout << "Error in Z ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(qa.GetW()-cosangle) > epsilon ) 
      {
      std::cout << "Error in W ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(qa.GetAngle()-angle) > epsilon ) 
      {
      std::cout << "Error in Angle ! " << std::endl;
      return EXIT_FAILURE;
      } 

    std::cout << " PASSED !" << std::endl;
  }

  {
    std::cout << "Test for setting Right part...";
    QuaternionType qa;
    VectorType xa;
    
    ValueType angle = atan(1.0)*30.0/45.0;
    ValueType sin2a = sin( angle/2.0 );
    
    xa[0] = 0.7;
    xa[1] = 0.3;
    xa[2] = 0.1;
    
    xa.Normalize();

    xa *= sin2a;

    qa.Set( xa, angle );
        
    ValueType cos2a = cos( angle/2.0 );

    if( fabs(qa.GetW()-cos2a) > epsilon ) 
      {
      std::cout << "Error in W ! " << std::endl;
      std::cout << "W= " << qa.GetW();
      std::cout << " it should be " << cos2a << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(qa.GetAngle()-angle) > epsilon ) 
      {
      std::cout << "Error in Angle ! " << std::endl;
      return EXIT_FAILURE;
      } 

    std::cout << " PASSED !" << std::endl;
  }

 {
    std::cout << "Test for Square Root...";
    QuaternionType qa;
    VectorType xa;
    
    ValueType angle = atan(1.0)*30.0/45.0;
    ValueType sin2a = sin( angle/2.0 );
    
    xa[0] = 0.7;
    xa[1] = 0.3;
    xa[2] = 0.1;
    
    xa.Normalize();

    xa *= sin2a;

    qa.Set( xa, angle );
        
    QuaternionType qb;
 
    qb = qa.SquareRoot();

    if( fabs( qa.GetAngle() - 2.0 * qb.GetAngle() ) > epsilon ) 
      {
      std::cout << "Error in Square Root ! " << std::endl;
      std::cout << "Angle = " << qb.GetAngle();
      std::cout << " it should be " << qa.GetAngle() / 2.0 << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;
  }

  {
    std::cout << "Test for Transforming a vector...";
    QuaternionType qa;
    VectorType xa;
    xa[0] = 2.5;
    xa[1] = 2.5;
    xa[2] = 2.5;
    ValueType angle = 8.0*atan(1.0)/3.0; // 120 degrees in radians
    
    qa.Set( xa, angle );
        
    VectorType::ValueType xbInit[3] = {3.0, 7.0, 9.0};
    VectorType xb = xbInit;

    VectorType xc;

    xc = qa.Transform( xb );

    // This rotation will just permute the axis

    if( fabs(xc[1]-xb[0]) > epsilon ) 
      {
      std::cout << "Error in X ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(xc[2]-xb[1]) > epsilon ) 
      {
      std::cout << "Error in Y ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(xc[0]-xb[2]) > epsilon ) 
      {
      std::cout << "Error in Z ! " << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;
  }

  {
    std::cout << "Test for Transforming a point...";
    QuaternionType qa;
    VectorType xa;
    xa[0] = 2.5;
    xa[1] = 2.5;
    xa[2] = 2.5;
    ValueType angle = 8.0*atan(1.0)/3.0; // 120 degrees in radians
    
    qa.Set( xa, angle );
        
    PointType::ValueType xbInit[3] = {3.0, 7.0, 9.0};
    PointType xb = xbInit;

    PointType xc;

    xc = qa.Transform( xb );

    // This rotation will just permute the axis

    if( fabs(xc[1]-xb[0]) > epsilon ) 
      {
      std::cout << "Error in X ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(xc[2]-xb[1]) > epsilon ) 
      {
      std::cout << "Error in Y ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(xc[0]-xb[2]) > epsilon ) 
      {
      std::cout << "Error in Z ! " << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;
  }


  {
    std::cout << "Test for Transforming a covariantvector...";
    QuaternionType qa;
    VectorType xa;
    xa[0] = 2.5;
    xa[1] = 2.5;
    xa[2] = 2.5;
    ValueType angle = 8.0*atan(1.0)/3.0; // 120 degrees in radians
    
    qa.Set( xa, angle );
        
    CovariantVectorType::ValueType xbInit[3] = {3.0, 7.0, 9.0};
    CovariantVectorType xb = xbInit;

    CovariantVectorType xc;

    xc = qa.Transform( xb );

    // This rotation will just permute the axis

    if( fabs(xc[1]-xb[0]) > epsilon ) 
      {
      std::cout << "Error in X ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(xc[2]-xb[1]) > epsilon ) 
      {
      std::cout << "Error in Y ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(xc[0]-xb[2]) > epsilon ) 
      {
      std::cout << "Error in Z ! " << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;
  }

  {
    std::cout << "Test for Transforming a vnl_vector...";
    QuaternionType qa;
    VectorType xa;
    xa[0] = 2.5;
    xa[1] = 2.5;
    xa[2] = 2.5;
    ValueType angle = 8.0*atan(1.0)/3.0; // 120 degrees in radians
    
    qa.Set( xa, angle );
        
    VnlVectorType xb;
    xb[0] = 3.0;
    xb[1] = 7.0;
    xb[2] = 9.0;

    VnlVectorType xc;

    xc = qa.Transform( xb );

    // This rotation will just permute the axis

    if( fabs(xc[1]-xb[0]) > epsilon ) 
      {
      std::cout << "Error in X ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(xc[2]-xb[1]) > epsilon ) 
      {
      std::cout << "Error in Y ! " << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(xc[0]-xb[2]) > epsilon ) 
      {
      std::cout << "Error in Z ! " << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;
  }
  


  {

    std::cout << "Test for Set components operations ...";

    // First, create a known versor
    QuaternionType v1;

    VectorType::ValueType x1Init[3] = {2.5f, 1.5f, 3.5f};
    VectorType x1 = x1Init;

    ValueType angle1 = atan(1.0)/3.0; // 15 degrees in radians
    
    v1.Set( x1, angle1 );
 
    // Get the components and scale them
    ValueType scale = 5.5;
    ValueType x = v1.GetX() * scale;
    ValueType y = v1.GetY() * scale;
    ValueType z = v1.GetZ() * scale;
    ValueType w = v1.GetW() * scale;
 
    QuaternionType v2;
    v2.Set( x, y, z, w );

    // Compare both versors
    if( fabs( v1.GetX() * scale - v2.GetX() ) > epsilon ||
        fabs( v1.GetY() * scale - v2.GetY() ) > epsilon ||
        fabs( v1.GetZ() * scale - v2.GetZ() ) > epsilon ||
        fabs( v1.GetW() * scale - v2.GetW() ) > epsilon )
      {
      std::cout << "Error in Quaternion Set(x,y,z,w) ! " << std::endl;
      std::cout << "v1  = " << v1 << std::endl;
      std::cout << "v2  = " << v2 << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;

    std::cout << "Test for Tensor... ";
    if( fabs( v2.GetTensor() - scale ) > epsilon )
      {
      std::cout << "Failed !" << std::endl;
      std::cout << "Expected Tensor = " << scale << std::endl;
      std::cout << "Computed Tensor = " << v2.GetTensor() << std::endl;
      return EXIT_FAILURE;
      }
    std::cout << " PASSED !" << std::endl;

    std::cout << "Test for Set quaternion ...";
    // Get a vnl_quaternion
    VnlQuaternionType vnlq = v1.GetVnlQuaternion();
   
    vnlq *= scale;

    v2.Set( vnlq );

    // Compare both versors
    if( fabs(v1.GetX() * scale - v2.GetX() ) > epsilon ||
        fabs(v1.GetY() * scale - v2.GetY() ) > epsilon ||
        fabs(v1.GetZ() * scale - v2.GetZ() ) > epsilon ||
        fabs(v1.GetW() * scale - v2.GetW() ) > epsilon )
      {
      std::cout << "Error in Quaternion Set( vnl_quaternion ) ! " << std::endl;
      std::cout << "v1  = " << v1 << std::endl;
      std::cout << "v2  = " << v2 << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;


    std::cout << "Test for Set(x,y,z,w) with negative W.";
    // Check that a negative W results in negating 
    // all the versor components.
    x = - v1.GetX();
    y = - v1.GetY();
    z = - v1.GetZ();
    w = - v1.GetW();
 
    QuaternionType v3;
    v3.Set( x, y, z, w );

     // Compare both versors 
    if( fabs( v1.GetX() - v3.GetX() ) > epsilon ||
        fabs( v1.GetY() - v3.GetY() ) > epsilon ||
        fabs( v1.GetZ() - v3.GetZ() ) > epsilon ||
        fabs( v1.GetW() - v3.GetW() ) > epsilon )
      {
      std::cout << "Error in Quaternion Set() with negative W ! " << std::endl;
      std::cout << "v1  = " << v1 << std::endl;
      std::cout << "v3  = " << v3 << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;

  }



  {

    std::cout << "Test for Reciprocal and Conjugate Operations...";

    QuaternionType  v1;
    QuaternionType  v2;

    VectorType::ValueType x1Init[3] = {2.5f, 1.5f, 0.5f};
    VectorType x1 = x1Init;

    ValueType angle1 = atan(1.0)/3.0; // 15 degrees in radians
    
    VectorType::ValueType x2Init[3] = {1.5f, 0.5f, 0.5f};
    VectorType x2 = x2Init;

    ValueType angle2 = atan(1.0)/1.0; // 45 degrees in radians
    
    v1.Set( x1, angle1 );
    v2.Set( x2, angle2 );
        

    QuaternionType unit;
    QuaternionType v2r;

    v2r  = v2.GetReciprocal();
    unit = v2 * v2r;

    if( fabs( unit.GetX() ) > epsilon ||
        fabs( unit.GetY() ) > epsilon ||
        fabs( unit.GetZ() ) > epsilon ||
        fabs( unit.GetW() - 1.0 ) > epsilon )
      {
      std::cout << "Error in Reciprocal ! " << std::endl;
      std::cout << "Quaternion     = " << v2    << std::endl;
      std::cout << "Reciprocal = " << v2r   << std::endl;
      std::cout << "Product    = " << unit  << std::endl;

      return EXIT_FAILURE;
      }  


    unit = v2 / v2;

    if( fabs( unit.GetX() ) > epsilon ||
        fabs( unit.GetY() ) > epsilon ||
        fabs( unit.GetZ() ) > epsilon ||
        fabs( unit.GetW() - 1.0 ) > epsilon )
      {
      std::cout << "Error in Division ! " << std::endl;
      std::cout << "Quaternion          = " << v2    << std::endl;
      std::cout << "Self Division   = " << unit  << std::endl;

      return EXIT_FAILURE;
      }  

    unit =  v2;
    unit /= v2;

    if( fabs( unit.GetX() ) > epsilon ||
        fabs( unit.GetY() ) > epsilon ||
        fabs( unit.GetZ() ) > epsilon ||
        fabs( unit.GetW() - 1.0 ) > epsilon )
      {
      std::cout << "Error in Division operator/= ! " << std::endl;
      std::cout << "Quaternion          = " << v2    << std::endl;
      std::cout << "Self Division   = " << unit  << std::endl;

      return EXIT_FAILURE;
      }  





    x1.Normalize();
    x2.Normalize();

   
    QuaternionType  v3;

    v3 = v1 * v2;

    QuaternionType v4;

    v4 = v3 * v2r;


    if( fabs(v1.GetX() - v4.GetX() ) > epsilon ||
        fabs(v1.GetY() - v4.GetY() ) > epsilon ||
        fabs(v1.GetZ() - v4.GetZ() ) > epsilon ||
        fabs(v1.GetW() - v4.GetW() ) > epsilon )
      {
      std::cout << "Error in Quaternion division ! " << std::endl;
      std::cout << "v1  = " << v1 << std::endl;
      std::cout << "v1' = " << v4 << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;


    std::cout << "Testing Sqrt() and Exponential()...";
    QuaternionType v5 = v4.SquareRoot();
    QuaternionType v6 = v4.Exponential( 0.5 );
    QuaternionType v7 = v5.Exponential( 2.0 );
    
    if( fabs(v6.GetX() - v5.GetX() ) > epsilon ||
        fabs(v6.GetY() - v5.GetY() ) > epsilon ||
        fabs(v6.GetZ() - v5.GetZ() ) > epsilon ||
        fabs(v6.GetW() - v5.GetW() ) > epsilon )
      {
      std::cout << "Error in SquareRoot() and/or Exponential ! " << std::endl;
      std::cout << "v5 = " << v5 << std::endl;
      std::cout << "v6 = " << v6 << std::endl;
      return EXIT_FAILURE;
      } 
    if( fabs(v7.GetX() - v4.GetX() ) > epsilon ||
        fabs(v7.GetY() - v4.GetY() ) > epsilon ||
        fabs(v7.GetZ() - v4.GetZ() ) > epsilon ||
        fabs(v7.GetW() - v4.GetW() ) > epsilon )
      {
      std::cout << "Error in SquareRoot() and/or Exponential ! " << std::endl;
      std::cout << "v4 = " << v4 << std::endl;
      std::cout << "v7 = " << v7 << std::endl;
      return EXIT_FAILURE;
      } 

    std::cout << " PASSED !" << std::endl;


  }


  { // Test for the Set() matrix method
    
    std::cout << "Test for Set( MatrixType ) method ...";

    QuaternionType vv;
    MatrixType mm;

    // Setting the matrix of a 90 degrees rotation around Z
    mm[0][0] =  0.0;
    mm[0][1] =  1.0;
    mm[0][2] =  0.0;

    mm[1][0] =  0.0;
    mm[1][1] = -1.0;
    mm[1][2] =  0.0;

    mm[2][0] =  0.0;
    mm[2][1] =  0.0;
    mm[2][2] =  1.0;

    vv.Set( mm );
    vv.Normalize();

    const double halfSqrtOfTwo = vcl_sqrt( 2.0 ) / 2.0;

    if( fabs(vv.GetX() -             0.0  ) > epsilon ||
        fabs(vv.GetY() -             0.0  ) > epsilon ||
        fabs(vv.GetZ() - (-halfSqrtOfTwo) ) > epsilon ||
        fabs(vv.GetW() -   halfSqrtOfTwo  ) > epsilon )
      {
      std::cout << "Error in Quaternion Set(Matrix) method ! " << std::endl;
      std::cout << "vv  = " << vv << std::endl;
      return EXIT_FAILURE;
      } 
    std::cout << " PASSED !" << std::endl;



  }


  return EXIT_SUCCESS;

}



