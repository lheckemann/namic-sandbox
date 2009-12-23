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
#ifndef __itkSymRealSphericalHarmonicRep_h
#define __itkSymRealSphericalHarmonicRep_h

// Undefine an eventual SymmetricSecondRankTensor macro
#ifdef SymRealSphericalHarmonicRep
#undef SymRealSphericalHarmonicRep
#endif

#include "itkIndent.h"
#include "itkFixedArray.h"
#include "itkMatrix.h"
#include "itkArray2D.h"
#include "itkSymmetricEigenAnalysis.h"
#include "itk_hash_map.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_vector_fixed.h"

#include "itkRealSymSphericalHarmonicBasis.h"

namespace itk
{

template
< typename TComponent,
  unsigned int TMaxOrder=4,
  typename TBasisType= itk::RealSymSphericalHarmonicBasis< TMaxOrder >
>
class SymRealSphericalHarmonicRep: public
      FixedArray<TComponent,(TMaxOrder+1)*(TMaxOrder+2)/2>
{
public:

  /** Standard class typedefs. */
  typedef SymRealSphericalHarmonicRep  Self;
  typedef FixedArray<TComponent,(TMaxOrder+1)*(TMaxOrder+2)/2> Superclass;

  /** Dimension of the vector space. */
  itkStaticConstMacro(MaxOrder, unsigned int, TMaxOrder);
  itkStaticConstMacro(Dimension, unsigned int, (TMaxOrder+1)*(TMaxOrder+2)/2);

  /**Dimension of unique Orders since Max Order must be even. */
  itkStaticConstMacro(NumberOfOrders, unsigned int, TMaxOrder / 2 + 1);

  /** Basis Typedef. */
  typedef TBasisType                                                BasisType;
  typedef typename BasisType::RshRotationMatixType                  RshRotationMatixType;

  /** Convenience typedefs. */
  typedef FixedArray<TComponent, itkGetStaticConstMacro(Dimension)> BaseArray;

  /**  Define the component type. */
  typedef TComponent ComponentType;
  typedef typename Superclass::ValueType ValueType;
  typedef typename NumericTraits<ValueType>::RealType AccumulateValueType;
  typedef typename NumericTraits<ValueType>::RealType RealValueType;

  /**  Define the Gradient Direction Type. */
  typedef vnl_vector_fixed< double, 3 >               GradientDirectionType;

  /** Container to hold gradient directions of the 'n' DW measurements */
  typedef VectorContainer< unsigned int,
          GradientDirectionType >                     GradientDirectionContainerType;

  typedef vnl_matrix<double>                          RshBasisMatrixType;

  /** Default constructor. */
  SymRealSphericalHarmonicRep()
    {
    if ( (MaxOrder % 2) != 0 )
      {
      itkGenericExceptionMacro( << "Symetric real spherical harmonic representations are only of even order");
      }

    this->Fill(0);
    }

  SymRealSphericalHarmonicRep (const ComponentType& r)
    {
    if ( (MaxOrder % 2) != 0 )
      {
      itkGenericExceptionMacro( << "Symetric real spherical harmonic representations are only of even order!");
      }
    this->Fill(r);
    }

  typedef ComponentType ComponentArrayType[ itkGetStaticConstMacro(Dimension) ];

  /** Pass-through constructor for the Array base class. */
  SymRealSphericalHarmonicRep(const Self& r): BaseArray(r)
    {
    if ( (MaxOrder % 2) != 0 )
      {
      itkGenericExceptionMacro( << "Symetric real spherical harmonic representations are only of even order!");
      }
    }

  SymRealSphericalHarmonicRep(const ComponentArrayType r): BaseArray(r)
    {
    if ( (MaxOrder % 2) != 0 )
      {
      itkGenericExceptionMacro( << "Symetric real spherical harmonic representations are only of even order!");
      }
    }

  /** Templated constructor */
  template < typename TCoordRepB >
  SymRealSphericalHarmonicRep( const SymRealSphericalHarmonicRep<TCoordRepB,TMaxOrder> & pa ):
    BaseArray(pa) { };


  /** Pass-through assignment operator for the Array base class. */
  Self& operator= (const Self& r);
  Self& operator= (const ComponentType& r);
  Self& operator= (const ComponentArrayType r);

  /**
   * Assigment from a vnl_vector
   */
  template <typename VectorComponentType>
  Self& operator= (const vnl_vector<VectorComponentType> r )
  {
    ///check the size...
    if (r.size() != Dimension)
    {
      itkGenericExceptionMacro( << "Assignment must assign every element of array:"
          << "Expecting " << Dimension << "elements. But only received " << r.size()
                     );
    }
    for (unsigned int i=0;i<r.size();i++){
      (*this)[i] = static_cast<ComponentType>(r.get(i));
    }
    return *this;
  }


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
    return itkGetStaticConstMacro(Dimension);
    }

  /** Return the number of Orders. */
  static unsigned int GetNumberOfOrder()
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

  /** Return the value for the l and mth component. */
  ComponentType GetLthMthComponent(int l,int m) const
  {
    if ( (l % 2) != 0 || l > 20 )
    {
      itkGenericExceptionMacro( << "Attempting to extract a componant with an illegal order (l)");
    }
    int c = BasisType::GetJ(l,m);
    return this->operator[](c);
  }

  /** Set the Nth component to v. */
  void SetLthMthComponent(int l,int m, const ComponentType& v)
  {
    if ( (l % 2) != 0 || l > 20 )
    {
      itkGenericExceptionMacro( << "Attempting to Set a componant with an illegal order (l)");
    }
    int c = BasisType::GetJ(l,m);
    this->operator[](c) = v;
  }
 
  static const RshBasisMatrixType ComputeRshBasis( const GradientDirectionContainerType* );

  BaseArray GetFixedArray() { return (*this); }

  /** Rotate by the provided matix. */
  template<typename TMatrixValueType>
  Self Rotate( const Matrix<TMatrixValueType, 3u, 3u> & m);

  template<typename TMatrixValueType>
  Self Rotate( const vnl_matrix_fixed<TMatrixValueType, 3u, 3u> & m)
  {
    return this->Rotate( static_cast<Matrix<TMatrixValueType, 3u, 3u> >(m) );
  }

  template<typename TMatrixValueType>
  Self Rotate( const vnl_matrix<TMatrixValueType> & m)
  {
    return this->Rotate( static_cast<Matrix<TMatrixValueType> >(m) );
  }

  RealValueType Evaluate(RealValueType theta, RealValueType phi);
  RealValueType Evaluate(GradientDirectionType Gradient);

protected:

};

} // end namespace itk

#include "itkNumericTraitsSymRshPixel.h"
#include "itkSymRealSphericalHarmonicRep.txx"
#endif
