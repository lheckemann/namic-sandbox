/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGaussianDerivativeImageFunction.h,v $
  Language:  C++
  Date:      $Date: 2006/02/06 22:01:55 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkShapeOperatorImageFunction_h
#define __itkShapeOperatorImageFunction_h

#include "itkNeighborhoodOperatorImageFunction.h"
#include "itkImageFunction.h"
#include "itkGaussianDerivativeSpatialFunction.h"
#include "itkGaussianSpatialFunction.h"
#include "itkSymmetricSecondRankTensor.h"
#include "itkMatrix.h"

namespace itk
{

/**
 * \class ShapeOperatorImageFunction
 * \brief Compute the shape operator of an the image
 *        at a specific location in space, i.e. point, index or continuous
 *        index.
 * This class is templated over the input image type.
 * \sa NeighborhoodOperator
 * \sa ImageFunction
 */
template <class TInputImage, class TOutput = double>
class ITK_EXPORT ShapeOperatorImageFunction :
  public ImageFunction< TInputImage, 
    //Vector<TOutput,::itk::GetImageDimension<TInputImage>::ImageDimension>,
  //SymmetricSecondRankTensor<TOutput,::itk::GetImageDimension<TInputImage>::ImageDimension>,
  itk::Matrix<TOutput, ::itk::GetImageDimension<TInputImage>::ImageDimension,::itk::GetImageDimension<TInputImage>::ImageDimension>,
  TOutput>
{
public:

  /**Standard "Self" typedef */
  typedef ShapeOperatorImageFunction Self;

  /** Standard "Superclass" typedef*/
  /*
  typedef ImageFunction<TInputImage,
    Vector<TOutput,::itk::GetImageDimension<TInputImage>::ImageDimension>,
    TOutput > Superclass;
 */
  typedef ImageFunction<TInputImage,
    //SymmetricSecondRankTensor<TOutput,::itk::GetImageDimension<TInputImage>::ImageDimension>,
  itk::Matrix<TOutput, ::itk::GetImageDimension<TInputImage>::ImageDimension,::itk::GetImageDimension<TInputImage>::ImageDimension>,
    TOutput > Superclass;

  /** Smart pointer typedef support. */
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory.*/
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( ShapeOperatorImageFunction, ImageFunction );

  /** InputImageType typedef support.*/
  typedef TInputImage                                 InputImageType;
  typedef typename InputImageType::PixelType          InputPixelType;
  typedef typename InputImageType::IndexType IndexType;

  /** Dimension of the underlying image. */
  itkStaticConstMacro(ImageDimension2, unsigned int,
                      InputImageType::ImageDimension);
  
  typedef ContinuousIndex<TOutput,itkGetStaticConstMacro(ImageDimension2)>
          ContinuousIndexType;


  typedef Neighborhood<InputPixelType, itkGetStaticConstMacro(ImageDimension2)> NeighborhoodType;
  typedef Neighborhood<TOutput, itkGetStaticConstMacro(ImageDimension2)> OperatorNeighborhoodType;

  typedef Vector<TOutput,itkGetStaticConstMacro(ImageDimension2)>  VectorType;
  typedef SymmetricSecondRankTensor<double,itkGetStaticConstMacro(ImageDimension2)> TensorType;
  //typedef typename Superclass::OutputType  OutputType;
    //typedef SymmetricSecondRankTensor<TOutput,::itk::GetImageDimension<TInputImage>::ImageDimension> OutputType;
  typedef itk::Matrix<TOutput,::itk::GetImageDimension<TInputImage>::ImageDimension,::itk::GetImageDimension<TInputImage>::ImageDimension> OutputType;

  typedef FixedArray<OperatorNeighborhoodType,2*itkGetStaticConstMacro(ImageDimension2)> OperatorArrayType;
  typedef NeighborhoodOperatorImageFunction<InputImageType,
                                             TOutput> OperatorImageFunctionType;
  typedef typename OperatorImageFunctionType::Pointer OperatorImageFunctionPointer;

  typedef GaussianDerivativeSpatialFunction<TOutput,1>  GaussianDerivativeFunctionType;
  typedef typename GaussianDerivativeFunctionType::Pointer GaussianDerivativeFunctionPointer;

  typedef GaussianSpatialFunction<TOutput,1>  GaussianFunctionType;
  typedef typename GaussianFunctionType::Pointer GaussianFunctionPointer;

  /** Point typedef support. */
  typedef Point<TOutput,itkGetStaticConstMacro(ImageDimension2)> PointType;
  
  /** Evalutate the  in the given dimension at specified point */
  virtual OutputType Evaluate(const PointType& point) const;


  /** Evaluate the function at specified Index position*/
  virtual OutputType EvaluateAtIndex( const IndexType & index ) const;
  //virtual TensorType EvaluateAtIndex( const IndexType & index ) const;

  /** Evaluate the function at specified ContinousIndex position.*/
  virtual OutputType EvaluateAtContinuousIndex( 
    const ContinuousIndexType & index ) const;

  /** Set the input image.
   * \warning this method caches BufferedRegion information.
   * If the BufferedRegion has changed, user must call
   * SetInputImage again to update cached values. */
  virtual void SetInputImage( const InputImageType * ptr );

protected:
  ShapeOperatorImageFunction();
  ShapeOperatorImageFunction( const Self& ){};

  ~ShapeOperatorImageFunction(){};

  void operator=( const Self& ){};
  void PrintSelf(std::ostream& os, Indent indent) const;

private:

  double                        m_Sigma[ImageDimension2];

  /** Array of 1D operators. Contains a derivative kernel and a gaussian kernel for
   *  each dimension */
  mutable OperatorArrayType     m_OperatorArray;
  mutable OperatorArrayType     m_ContinuousOperatorArray;

  /** OperatorImageFunction */
  OperatorImageFunctionPointer  m_OperatorImageFunction;
  double m_Extent[ImageDimension2];

  /** Flag to indicate whether to use image spacing */
  bool m_UseImageSpacing;

  /** Neighborhood Image Function */
  GaussianDerivativeFunctionPointer m_GaussianDerivativeFunction;
  GaussianFunctionPointer           m_GaussianFunction;

  };

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkShapeOperatorImageFunction.txx"
#endif

#endif

