//
// C++ Interface: sbiaSymRealSphericalHarmonicRep
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef __sbiaSymRealSphericalHarmonicRep_h
#define __sbiaSymRealSphericalHarmonicRep_h

// Undefine an eventual SymmetricSecondRankTensor macro
#ifdef SymRealSphericalHarmonicRep
#undef SymRealSphericalHarmonicRep
#endif

#include "itkIndent.h"
#include "itkFixedArray.h"
#include "itkMatrix.h"
#include "itkSymmetricEigenAnalysis.h"
#include "vnl/vnl_vector_fixed.h"
#include "itk_hash_map.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{

/** \class SymRealSphericalHarmonicRep
 * \brief Represent function as an expansion of the real spherical harmonics
 *
 * This class implements a ND symmetric tensor of second rank.
 *
 * Since RealSphericalHarmonicRep is a subclass of FixedArray,
 * you can access its components as:
 *
 * typedef itk::SymRealSphericalHarmonicRep< float >    PixelType;
 * PixelType odf;
 *
 *   odf[0] = 1.233;
 *   odf[1] = 1.456;
 *
 * \author Luke Bloy from School of Engineering at University of Pennsylvania
 *
 * \sa RealSymSphericalHarmonicRep
 *
 */

template < typename TComponent, unsigned int NOrder=4 >
class SymRealSphericalHarmonicRep: public
      FixedArray<TComponent,(NOrder+1)*(NOrder+2)/2>
{
public:

  /** Standard class typedefs. */
  typedef SymRealSphericalHarmonicRep  Self;
  typedef FixedArray<TComponent,(NOrder+1)*(NOrder+2)/2> Superclass;

  /** Dimension of the vector space. */
  itkStaticConstMacro(Order, unsigned int, NOrder);
  itkStaticConstMacro(InternalDimension, unsigned int, (NOrder+1)*(NOrder+2)/2);

  /** Convenience typedefs. */
  typedef FixedArray<TComponent,
  itkGetStaticConstMacro(InternalDimension)> BaseArray;

  /**  Define the component type. */
  typedef TComponent ComponentType;
  typedef typename Superclass::ValueType ValueType;
  typedef typename NumericTraits<ValueType>::RealType AccumulateValueType;
  typedef typename NumericTraits<ValueType>::RealType RealValueType;

  /** Runtime information support. */
  itkTypeMacro(SymRealSphericalHarmonicRep, FixedArray);

  /**  Define the Gradient Direction Type. */
  typedef vnl_vector_fixed< double, 3 >            GradientDirectionType;

  /** Default constructor. */
  SymRealSphericalHarmonicRep()
    {
    if ( (NOrder % 2) != 0)
      {
      itkExceptionMacro( << "Symetric real spherical harmonic representations are only of even order!");
      }

    this->Fill(0);
    }

  SymRealSphericalHarmonicRep (const ComponentType& r)
    {
    if ( (NOrder % 2) != 0)
      {
      itkExceptionMacro( << "Symetric real spherical harmonic representations are only of even order!");
      }
    this->Fill(r);
    }

  typedef ComponentType ComponentArrayType[ itkGetStaticConstMacro(InternalDimension) ];

  /** Pass-through constructor for the Array base class. */
  SymRealSphericalHarmonicRep(const Self& r): BaseArray(r)
    {
    if ( (NOrder % 2) != 0)
      {
      itkExceptionMacro( << "Symetric real spherical harmonic representations are only of even order!");
      }
    }

  SymRealSphericalHarmonicRep(const ComponentArrayType r): BaseArray(r)
    {
    if ( (NOrder % 2) != 0)
      {
      itkExceptionMacro( << "Symetric real spherical harmonic representations are only of even order!");
      }
    }

  /** Destructor */
  virtual ~SymRealSphericalHarmonicRep() 
   { }

  /** Pass-through assignment operator for the Array base class. */
  Self& operator= (const Self& r);
  Self& operator= (const ComponentType& r);
  Self& operator= (const ComponentArrayType r);

  /** assignment operator for the vnl_array class. */
  Self& operator= (const vnl_vector<ComponentType> r);


  /** Aritmetic operations between pixels. Return a new SymRealSphericalHarmonicRep. */
  Self operator+(const Self &vec) const;
  Self operator-(const Self &vec) const;
  const Self & operator+=(const Self &vec);
  const Self & operator-=(const Self &vec);

  /** Arithmetic operations between tensors and scalars */
  Self operator*(const RealValueType & scalar ) const;
  Self operator/(const RealValueType & scalar ) const;
  const Self & operator*=(const RealValueType & scalar );
  const Self & operator/=(const RealValueType & scalar );

  /** Return the number of components. */
  static unsigned int GetNumberOfComponents()
    {
    return itkGetStaticConstMacro(InternalDimension);
    }

  /** Return the number of components. */
  static unsigned int GetOrder()
    {
    return itkGetStaticConstMacro(Order);
    }

  /** Return the value for the Nth component. */
  ComponentType GetNthComponent(int c) const
      {
      return this->operator[](c);
      }

  /** Set the Nth component to v. */
  void SetNthComponent(int c, const ComponentType& v)
    {
    this->operator[](c) = v;
    }

  /// Evaluates the Associated Legendre Polynomial with parameters (l,m) at x.
  static RealValueType P( int l, int m, RealValueType x );

  ///Theta \in (0-PI)
  ///Phi \in (0,2 PI)
  /// Evaluates the real SH basis function with parameters (l,m) at (theta,phi).
  static RealValueType Y( int l, int m, RealValueType theta, RealValueType phi );

  static RealValueType Y( int j, RealValueType theta, RealValueType phi )
    {
    if ( j < 1 )
      {
      itkGenericExceptionMacro( << "J muct be >= 1:" << "supplied j = " << j);
      }

    LmVector lm = getLM(j);
    return Y(lm[0],lm[1],theta,phi);
    }

  RealValueType Evaluate(RealValueType theta, RealValueType phi);

  RealValueType Evaluate(GradientDirectionType Gradient);

protected:
  typedef vnl_vector_fixed<int,2>  LmVector;

  /// computes the l and m for a give index j
  static LmVector getLM(int j)
    {

    if ( j < 1 )
      {
      itkGenericExceptionMacro( << "J muct be >= 1:" << "supplied j = " << j);
      }

    LmVector newVect;
    int l = 0;
    int m = 0;

    //Find the right l... just search
    for(l = 0; (pow(l,2) + l + 2)/2.0 - l <= j; l=l+2);
    l = l-2;

    //Find the right m... just search
    for (m = -l; (pow(l,2) +  l + 2)/2.0 + m <= j ; m++);
    m=m-1;

    if ( j != (pow(l,2) +  l + 2)/2.0 + m)
      {
        itkGenericExceptionMacro( << "Error calculating l and m from j = " << j);
      }
    newVect[0] = l;
    newVect[1] = m;

    return newVect;
    }


  /// Returns the normalization constant for the SH basis function with parameters (l,m).
  static RealValueType K( int l, int m );

  /// Evaluates the real RSH basis function with parameters (l,m) at (theta,phi).
  static RealValueType RSH( int l, int m, RealValueType theta, RealValueType phi );

  /// Simple incremental factorial helper function.
  static int factorial( int n );

  /** This extra typedef is necessary for preventing an Internal Compiler Error in
   * Microsoft Visual C++ 6.0. This typedef is not needed for any other compiler. */
  typedef std::ostream               OutputStreamType;
  typedef std::istream               InputStreamType;
  /*
    template< typename TComponent, unsigned int NDimension  >
    ITK_EXPORT OutputStreamType& operator<<(OutputStreamType& os,
                                            const SymRealSphericalHarmonicRep<TComponent,NDimension> & c);
    template< typename TComponent, unsigned int NDimension  >
    ITK_EXPORT InputStreamType& operator>>(InputStreamType& is,
                                           SymRealSphericalHarmonicRep<TComponent,NDimension> & c);
  */
};

} // end namespace itk

//#include "itkNumericTraitsTensorPixel.h"
#include "sbiaSymRealSphericalHarmonicRep.txx"
#endif
