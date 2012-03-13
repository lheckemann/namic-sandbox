/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSymmetricEigenAnalysisImageFilterWithMatrix.h,v $
  Language:  C++
  Date:      $Date: 2008-10-07 17:31:02 $
  Version:   $Revision: 1.25 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSymmetricEigenAnalysisImageFilterWithMatrix_h
#define __itkSymmetricEigenAnalysisImageFilterWithMatrix_h

#include "itkInPlaceImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkSymmetricEigenAnalysis.h"

namespace itk
{
  
/** \class SymmetricEigenAnalysisImageFilterWithMatrix
 * \brief Implements pixel-wise generic operation on one image.
 *
 * SymmetricEigenAnalysisImageFilterWithMatrix allows the output dimension of the filter
 * to be larger than the input dimension. Thus subclasses of the
 * SymmetricEigenAnalysisImageFilterWithMatrix (like the CastImageFilter) can be used
 * to promote a 2D image to a 3D image, etc.
 * 
 * \ingroup   IntensityImageFilters     Multithreaded
 */
template <class TInputImage, class TOutputImage >
class ITK_EXPORT SymmetricEigenAnalysisImageFilterWithMatrix : public InPlaceImageFilter<TInputImage,TOutputImage> 
{
public:
  /** Standard class typedefs. */
  typedef SymmetricEigenAnalysisImageFilterWithMatrix                       Self;
  typedef InPlaceImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(SymmetricEigenAnalysisImageFilterWithMatrix, InPlaceImageFilter);

  /** Some typedefs. */
  typedef TInputImage                              InputImageType;
  typedef typename    InputImageType::ConstPointer InputImagePointer;
  typedef typename    InputImageType::RegionType   InputImageRegionType; 
  typedef typename    InputImageType::PixelType    InputImagePixelType; 

  typedef TOutputImage                             OutputImageType;
  typedef typename     OutputImageType::Pointer    OutputImagePointer;
  typedef typename     OutputImageType::RegionType OutputImageRegionType;
  typedef typename     OutputImageType::PixelType  OutputImagePixelType;

  typedef SymmetricEigenAnalysis< InputImagePixelType, OutputImagePixelType > AnalysisType;

  /** Method to explicitly set the dimension of the matrix */
  void SetDimension( unsigned int n )
    {
    m_Analysis.SetDimension(n);
    }

  /** Typdedefs to order eigen values. 
   * OrderByValue:      lambda_1 < lambda_2 < ....
   * OrderByMagnitude:  |lambda_1| < |lambda_2| < .....
   * DoNotOrder:        Default order of eigen values obtained after QL method
   */
  typedef enum {
    OrderByValue=1,
    OrderByMagnitude,
    DoNotOrder
  } EigenValueOrderType;
 
  /** Order eigen values. Default is to OrderByValue:  lambda_1 <
   * lambda_2 < .... */
  void OrderEigenValuesBy( EigenValueOrderType order )
    {
    if( order == OrderByMagnitude )
      {
      m_Analysis.SetOrderEigenMagnitudes( true );
      }
    else if( order == DoNotOrder )
      {
      m_Analysis.SetOrderEigenValues( false );
      }
    }

protected:
  SymmetricEigenAnalysisImageFilterWithMatrix();
  virtual ~SymmetricEigenAnalysisImageFilterWithMatrix() {};

  /** SymmetricEigenAnalysisImageFilterWithMatrix can produce an image which is a different
   * resolution than its input image.  As such, SymmetricEigenAnalysisImageFilterWithMatrix
   * needs to provide an implementation for
   * GenerateOutputInformation() in order to inform the pipeline
   * execution model.  The original documentation of this method is
   * below.
   *
   * \sa ProcessObject::GenerateOutputInformaton()  */
  virtual void GenerateOutputInformation();

  /** SymmetricEigenAnalysisImageFilterWithMatrix can be implemented as a multithreaded filter.
   * Therefore, this implementation provides a ThreadedGenerateData() routine
   * which is called for each processing thread. The output image data is
   * allocated automatically by the superclass prior to calling
   * ThreadedGenerateData().  ThreadedGenerateData can only write to the
   * portion of the output image specified by the parameter
   * "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData()  */
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            int threadId );

private:
  SymmetricEigenAnalysisImageFilterWithMatrix(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  AnalysisType  m_Analysis;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSymmetricEigenAnalysisImageFilterWithMatrix.txx"
#endif

#endif
