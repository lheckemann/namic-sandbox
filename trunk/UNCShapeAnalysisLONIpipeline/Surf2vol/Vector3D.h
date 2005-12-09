/*---------------------------------------------------------------------------
Class definitions for Vector3D

Author:      Tom Fletcher
Version:    1.0
Last Modified:  6/06/99
---------------------------------------------------------------------------*/

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <iostream>
//#include "Mathdefs.h"

class Vector3D
{
public:
  // Constructor to create a zero vector
  Vector3D();

  // Constructor takes three coordinates
  Vector3D(double X, double Y, double Z);

  // Constructor takes a double vector
  Vector3D(double vect[3]);

  void print(std::ostream & out = std::cout) const;

  // Get and set functions
  void set(double X, double Y, double Z)
  {
    x = X;
    y = Y;
    z = Z;
  }

  void set(double v[3])
  {
    x = v[0];
    y = v[1];
    z = v[2];
  }

  void setX(double X) {x = X;}
  void setY(double Y) {y = Y;}
  void setZ(double Z) {z = Z;}

  double getX() const {return x;}
  double getY() const {return y;}
  double getZ() const {return z;}
  double getComp(int i)
  {
    double ret;
    if (i == 0) ret = x;
    else if (i == 1) ret = y;
    else if (i == 2) ret = z;
    else ret = 1.0e100;

    return ret;
  }

  // Arithmetic operations
  Vector3D& operator = (const Vector3D &v)
  {
    x = v.x;
    y = v.y;
    z = v.z;

    return (*this);
  }

  void operator += (const Vector3D &v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
  }

  void operator -= (const Vector3D &v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
  }

  void operator *= (const double k)
  {
    x *= k;
    y *= k;
    z *= k;
  }

  void operator /= (const double k)
  {
    x /= k;
    y /= k;
    z /= k;
  }

  void operator /= (const Vector3D &v)
  {
    x /= v.x;
    y /= v.y;
    z /= v.z;
  }

  // Multiply by a 4x4 matrix
  void multByMatrix4(float * mat);

  // Compute the cross product with another vector
  Vector3D cross(const Vector3D &v) const;

  // Compute the vector product with another vector
  Vector3D vprod(const Vector3D &v) const
  {
    Vector3D ret;

    ret.x = x * v.x;
    ret.y = y * v.y;
    ret.z = z * v.z;

    return ret;
  }

  // Compute the norm of the vector
  double norm() const;

  // Normalize the vector, returns old norm
  double normalize();

  // Declare our operations to be friends (so we can access private data)
  friend Vector3D operator + (const Vector3D &v1, const Vector3D &v2);
  friend Vector3D operator - (const Vector3D &v1, const Vector3D &v2);
  friend Vector3D operator - (const Vector3D &v);
  friend Vector3D operator * (const Vector3D &v, const double scalar);
  friend Vector3D operator * (const double scalar, const Vector3D &v);
  friend double operator * (const Vector3D &v1, const Vector3D &v2);
  friend bool operator == (const Vector3D &v1, const Vector3D &v2);
  friend bool operator != (const Vector3D &v1, const Vector3D &v2);

//private:
  double x, y, z;
};

// Vector operations
Vector3D operator + (const Vector3D &v1, const Vector3D &v2);
Vector3D operator - (const Vector3D &v1, const Vector3D &v2);
Vector3D operator - (const Vector3D &v);
Vector3D operator * (const Vector3D &v, const double scalar);
Vector3D operator * (const double scalar, const Vector3D &v);
Vector3D operator / (const Vector3D &v, const double scalar);
Vector3D operator / (const double scalar, const Vector3D &v);
Vector3D operator / (const Vector3D &v1, const Vector3D &v2);
Vector3D operator + (const Vector3D &v, const double scalar);
Vector3D operator ^ (const Vector3D &v1, const Vector3D &v2);

// Dot product
double operator * (const Vector3D &v1, const Vector3D &v2);

bool operator == (const Vector3D &v1, const Vector3D &v2);
bool operator != (const Vector3D &v1, const Vector3D &v2);


#endif

