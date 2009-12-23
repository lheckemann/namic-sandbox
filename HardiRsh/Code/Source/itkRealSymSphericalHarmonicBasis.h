
#ifndef __itkRealSymSphericalHarmonicBasis_h
#define __itkRealSymSphericalHarmonicBasis_h

#include "itkMatrix.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkVectorContainer.h"
#include "itkReplaceSpecialFunctions.h"

namespace itk
{

template < unsigned int TOrder = 4 > 
class RealSymSphericalHarmonicBasis
{
public:

  static RealSymSphericalHarmonicBasis& Instance()
  {
    static RealSymSphericalHarmonicBasis singleton;
    return singleton;
  }

  static const int MaxOrder       = TOrder;
  static const int Dimensions     = (MaxOrder+1)*(MaxOrder+2)/2;
  static const int NumberOfOrders = 1+(MaxOrder/2);

  //Typedefs for function Return Types
  typedef Matrix<double, Dimensions, Dimensions>      RshRotationMatixType;
  typedef vnl_vector_fixed<int,2>                     LmVector;

  //We should remove these maybe
  /** Holds each magnetic field gradient used to acquire one DWImage */
  typedef vnl_vector_fixed< double, 3 >               GradientDirectionType;
  //end possible remove

  /** Compute the j index from l and m*/
  static const unsigned int GetJ(int,int);
  static const LmVector GetLM(int);

  /*** Evaluates the real SH basis function with parameters (l,m) at (theta,phi).
   * We use the following converntion
   * 
   * theta \in (0-PI) is measures from the postize z axis.
   * phi \in (0,2 PI) from the positive z axis.
   *
   * Grad directions relation to theta phi
   * x = sin(theta) * cos(phi)
   * y = sin(theta) * sin(phi)
   * z = cos(theta)
   */
  static double Y( int j, double theta, double phi );
  static double Y( int l, int m, double theta, double phi )
  {
    return Y(GetJ(l,m),theta,phi);
  }
  
  /** Computes the full rotation matrix in this RSH basis.
   *  Arguments are takes euler angles in the z,y,z convention and computes the
   * rotation operator in the RSH space corresponding to that rotation.
   * 
   * Ie R(alpha,beta,gamma) = Rz(alpha) Ry(beta) Rz(gamma)
   * 
   * computation of Rz is not complex. however computation of Ry is.
   * A speed up is achieved by decomposing Ry(beta)
   * 
   * Ry(beta) = Rz(-pi/2) Ry(-pi/2) Rz(beta) Ry(pi/2) Rz(pi/2)
   * 
   * allowing Ry(pi/2) Rz(pi/2) and  Rz(-pi/2) Ry(-pi/2) to be precomputed.
   *
   * It is not forseen that changing the basis should change this method
   * instead Subclasses sould overload the methods
   * Initialize and ComputeZRotationMatrix 
  */
  template<typename TMatrixValueType>
  const RshRotationMatixType ComputeRotationMatrix(const Matrix<TMatrixValueType, 3u, 3u> & m);

protected:
  
  /** Set/get for DMatPiover2 and DMatMinusPiOver2
   *  Provided for subclasses
   **/
//  void SetDMatPiOver2(RshRotationMatixType);
//  void SetDMatMinusPiOver2(RshRotationMatixType);

  const RshRotationMatixType GetDMatPiOver2() const;
  const RshRotationMatixType GetDMatMinusPiOver2() const;
  
private:
  RealSymSphericalHarmonicBasis() { Initialize(); };
  ~RealSymSphericalHarmonicBasis() {};
  RealSymSphericalHarmonicBasis(const RealSymSphericalHarmonicBasis&);                 // Prevent copy-construction
  RealSymSphericalHarmonicBasis& operator=(const RealSymSphericalHarmonicBasis&);      // Prevent assignment

  /// Returns the normalization constant for the SH basis function with parameters (l,m).
  static double K( int l, int m );

  /**Initialize the m_DMatPi2 and m_DMatMinusPi2
   * is a seperate method to provide cleaner subclassing
   */
  virtual void Initialize();

  static const RshRotationMatixType ComputeZRotationMatrix(double);

  /**
   * Computes the wigner litte d matrix element(in sp harms) for pi over 2
   */
  static const double computeWignerLittleDPlus(int, int, int);

  /**Member variables*/
  RshRotationMatixType m_DMatPiOver2;      // Rz(pi/2)  Ry(pi/2)   
  RshRotationMatixType m_DMatMinusPiOver2; //  Ry(-pi/2) Rz(-pi/2) 

};
}

#include "itkRealSymSphericalHarmonicBasis.txx"

#endif
