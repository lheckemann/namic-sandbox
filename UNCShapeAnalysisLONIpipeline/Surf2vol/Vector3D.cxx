/*---------------------------------------------------------------------------
Member functions for class Vector3D

Author:      Tom Fletcher
Version:    1.0
Last Modified:  6/07/99
---------------------------------------------------------------------------*/
#include <math.h>

#include "Vector3D.h"

using namespace std;

Vector3D::Vector3D()
{
  x = 0;
  y = 0;
  z = 0;
}

Vector3D::Vector3D(double X, double Y, double Z)
{
  x = X;
  y = Y;
  z = Z;
}

Vector3D::Vector3D(double vect[3])
{
  x = vect[0];
  y = vect[1];
  z = vect[2];
}

void Vector3D::print(ostream & out) const
{
    out << '<' << x << ", " << y << ", " << z << ">\n";
}

void Vector3D::multByMatrix4(float * mat)
{
  double newX, newY, newZ;

  newX = *(mat    ) * x + *(mat + 4) * y + *(mat + 8) * z + *(mat + 12);
  newY = *(mat + 1) * x + *(mat + 5) * y + *(mat + 9) * z + *(mat + 13);
  newZ = *(mat + 2) * x + *(mat + 6) * y + *(mat + 10) * z + *(mat + 14);

  x = newX;
  y = newY;
  z = newZ;
}

// Computes the cross product with a vector v
Vector3D Vector3D::cross(const Vector3D &v) const
{
  Vector3D vect;

  vect.x = y * v.z - z * v.y;
  vect.y = z * v.x - x * v.z;
  vect.z = x * v.y - y * v.x;

  return vect;
}

// Computes the norm (square of the dot product of the vector w/ itself)
double Vector3D::norm() const
{
  return sqrt((*this) * (*this));
}

// Sets vector to a unit length, and returns old norm
double Vector3D::normalize()
{
  double length = norm();

  if(length == 0.0)
    return 0.0;

  x /= length;
  y /= length;
  z /= length;

  return length;
}

// Vector add
Vector3D operator + (const Vector3D &v1, const Vector3D &v2)
{
  Vector3D vect;

  vect.x = v1.x + v2.x;
  vect.y = v1.y + v2.y;
  vect.z = v1.z + v2.z;

  return vect;
}

Vector3D operator + (const Vector3D &v, const double scalar)
{
  Vector3D vect;

  vect.x = v.x + scalar;
  vect.y = v.y + scalar;
  vect.z = v.z + scalar;

  return vect;
}

// Vector subtract
Vector3D operator - (const Vector3D &v1, const Vector3D &v2)
{
  Vector3D vect;

  vect.x = v1.x - v2.x;
  vect.y = v1.y - v2.y;
  vect.z = v1.z - v2.z;

  return vect;
}

// Vector negate
Vector3D operator - (const Vector3D &v)
{
  Vector3D vect;

  vect.x = -(v.x);
  vect.y = -(v.y);
  vect.z = -(v.z);

  return vect;
}

// Vector scale
Vector3D operator * (const Vector3D &v, const double scalar)
{
  Vector3D vect;

  vect.x = scalar * v.x;
  vect.y = scalar * v.y;
  vect.z = scalar * v.z;

  return vect;
}

// Another vector scale
Vector3D operator * (const double scalar, const Vector3D &v)
{
  Vector3D vect;

  vect.x = scalar * v.x;
  vect.y = scalar * v.y;
  vect.z = scalar * v.z;

  return vect;
}

// Dot product
double operator * (const Vector3D &v1, const Vector3D &v2)
{
  return ((v1.x * v2.x) +  (v1.y * v2.y) + (v1.z * v2.z));
}

Vector3D operator ^ (const Vector3D &v1, const Vector3D &v2)
{
  Vector3D vect;

  vect.x = v1.x*v2.x;
  vect.y = v1.y*v2.y;
  vect.z = v1.z*v2.z;

  return vect;
}

Vector3D operator / (const Vector3D &v, const double scalar)
{
  Vector3D vect;

  vect.x = v.x/scalar;
  vect.y = v.y/scalar;
  vect.z = v.z/scalar;

  return vect;
}

Vector3D operator / (const Vector3D &v1, const Vector3D &v2)
{
  Vector3D vect;

  vect.x = v1.x/v2.x;
  vect.y = v1.y/v2.y;
  vect.z = v1.z/v2.z;

  return vect;
}

Vector3D operator / (const double scalar, const Vector3D &v)
{
  Vector3D vect;

  vect.x = scalar/v.x;
  vect.y = scalar/v.y;
  vect.z = scalar/v.z;

  return vect;
}

bool operator == (const Vector3D &v1, const Vector3D &v2)
{
  if(v1.x != v2.x)
    return false;
  if(v1.y != v2.y)
    return false;
  if(v1.z != v2.z)
    return false;

  return true;
}

bool operator != (const Vector3D &v1, const Vector3D &v2)
{
  return !(v1 == v2);
}
