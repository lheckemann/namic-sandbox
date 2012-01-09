/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkVectorFieldPCA_h
#define __itkVectorFieldPCA_h

#include "itkObject.h"
#include "itkPointSet.h"
#include "itkKernelFunctionBase.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"

namespace itk
{

/** \class VectorFieldPCA
 * \brief Produce the principle components of a vector valued function.
 * 
 * This calculator produces a set of basis functions composed of the
 * principal components of a set of vector valued functions.
 *
 * Specify an itk::KernelFunction for Kernel PCA.  The Kernel Function
 * can take as input an optional point set.
 *
 * This class is templated over the types of the vector valued functions,
 * the output point types, and optionally the point set type.
 *
 * \author Michael Bowers
 *
 * \ingroup Operators
 */

template< typename TRealValueType = double >
class ITK_EXPORT GaussianDistanceKernel : public KernelFunctionBase<TRealValueType>
{
public:
  /** Standard class typedefs. */
  typedef GaussianDistanceKernel      Self;
  typedef KernelFunctionBase<TRealValueType>      Superclass;
  typedef SmartPointer<Self>          Pointer;
  typedef SmartPointer<const Self>    ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(GaussianDistanceKernel, KernelFunction);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Set and get the Kernel sigma. */
  void SetKernelSigma( double s )
    { m_KernelSigma = s;
      m_OneOverMinusTwoSigmaSqr = -1.0 / (2.0 * s * s); }
  itkGetMacro( KernelSigma, double );

  /** Evaluate the function. Input is the squared distance */
  inline double Evaluate (const double& u) const
    { return ( vcl_exp( u * m_OneOverMinusTwoSigmaSqr )); }

protected:  
  GaussianDistanceKernel() {}
  ~GaussianDistanceKernel() {}
  void PrintSelf(std::ostream& os, Indent indent) const
    { Superclass::PrintSelf( os, indent ); }  

private:
  double                    m_KernelSigma;
  double                    m_OneOverMinusTwoSigmaSqr;

};

template <
    typename TVectorFieldElementType,
    typename TPCType,
    typename TPointSetPixelType = float,
    typename TPointSetCoordRepType = float,
    typename KernelFunctionType = KernelFunctionBase<TPointSetCoordRepType>,
    class    TPointSetType = PointSet<TPointSetPixelType, 3, DefaultStaticMeshTraits< TPointSetPixelType, 3, 3, TPointSetCoordRepType > >
  >
class ITK_EXPORT VectorFieldPCA : public Object 
{
public:
  /** Standard class typedefs. */
  typedef VectorFieldPCA                  Self;
  typedef Object                          Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VectorFieldPCA, Object);

  /** Type definitions for the PointSet. */
  typedef TPointSetType   InputPointSetType;

  /** Definitions for points of the PointSet. */
  typedef typename InputPointSetType::PointType     InputPointType;
  
  /** Pointer types for the PointSet. */
  typedef typename InputPointSetType::Pointer  InputPointSetPointer;
  
  /** Const Pointer type for the PointSet. */
  typedef typename InputPointSetType::ConstPointer InputPointSetConstPointer;

  /** Input PointSet dimension */
  itkStaticConstMacro(InputMeshDimension, unsigned int,
                      TPointSetType::PointDimension); 

  /** type for the vector fields. */
  typedef vnl_matrix< TVectorFieldElementType >               VectorFieldType;
  typedef VectorContainer< unsigned int, VectorFieldType >    VectorFieldSetType;

  typedef typename VectorFieldSetType::Pointer       VectorFieldSetTypePointer;
  typedef typename VectorFieldSetType::ConstPointer  VectorFieldSetTypeConstPointer;

  /** types for the output. */
  typedef vnl_matrix< TPCType > MatrixType;
  typedef vnl_vector< TPCType > VectorType;

  typedef VectorContainer< unsigned int, MatrixType > BasisSetType;
  typedef VectorContainer< unsigned int, VectorType > ResSetType;

  typedef typename BasisSetType::Pointer         BasisSetTypePointer;
  typedef typename KernelFunctionType::Pointer   KernelFunctionPointer;

  /** Set and get the input point set. */
  itkSetMacro(PointSet, InputPointSetPointer);
  itkGetMacro(PointSet, InputPointSetPointer);
  
  /** Set and get the vector fields for the analysis. */
  itkSetMacro(VectorFieldSet, VectorFieldSetTypePointer);
  itkGetMacro(VectorFieldSet, VectorFieldSetTypePointer);
  
  /** Set and get the PCA count. */
  itkSetMacro( ComponentCount, unsigned int );
  itkGetMacro( ComponentCount, unsigned int );

  /** Set pointer to the Kernel object. */
  itkSetMacro( KernelFunction, KernelFunctionPointer );

  /** Compute the PCA decomposition of the input point set.
      If a Kernel and a Kernel Sigma are set ,
      the calculator will perform Kernel PCA. */
  void Compute(void);

  /** Return the results. */
  itkGetConstReferenceMacro(AveVectorField, MatrixType);
  itkGetConstReferenceMacro(PCAEigenValues, VectorType);
  itkGetConstObjectMacro(BasisVectors, BasisSetType);

  /** Contains projection onto the vector field set from:
      1)  The vector field set used in Compute()
      2)  The vector field set from subsequent calls to Projection() */
  itkGetConstReferenceMacro(Projection, MatrixType);

  void Projection(const VectorFieldSetTypePointer );

protected:
  VectorFieldPCA();
  virtual ~VectorFieldPCA() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  void                      KernelPCA(void);
  void                      computeMomentumSCP(void);
  
private:

  VectorFieldPCA(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  VectorType                m_PCAEigenValues;

  BasisSetTypePointer       m_BasisVectors;
  VectorFieldSetTypePointer m_VectorFieldSet;
  InputPointSetPointer      m_PointSet;
  KernelFunctionPointer     m_KernelFunction;

  // problem dimensions
  unsigned int              m_ComponentCount;
  unsigned int              m_SetSize;
  unsigned int              m_VectorDimCount;
  unsigned int              m_VertexCount;
  unsigned int              m_PointDim;

  MatrixType                m_Projection;
  MatrixType                m_V0;
  MatrixType                m_AveVectorField;
  MatrixType                m_K;

  bool                      m_PCACalculated;

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVectorFieldPCA.txx"
#endif

#endif
