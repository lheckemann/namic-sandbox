/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageClassifierFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:55:04 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageClassifierFilter_h
#define __itkImageClassifierFilter_h

#include <vector>

#include "itkMembershipSample.h"
#include "itkMembershipFunctionBase.h"
#include "itkDecisionRule.h"
#include "itkImageToImageFilter.h"
#include "itkSimpleDataObjectDecorator.h"

namespace itk {
namespace Statistics {

/** \class ImageClassifierFilter 
 *
 *  \brief This filter takes as input image, membership functions
 *  and produces as output image with each pixel 
 *  labeled according to the classification result.
 *
 */

template< class TSample, class TInputImage, class TOutputImage >
class ITK_EXPORT ImageClassifierFilter : 
      public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedef */
  typedef ImageClassifierFilter                           Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>    Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer<const Self>                        ConstPointer;

  /** Standard macros */
  itkTypeMacro(ImageClassifierFilter, ProcessObject);
  itkNewMacro(Self);

  /** Image pixel value typedef. */
  typedef           TInputImage             InputImageType;
  typedef           TOutputImage            OutputImageType;
  typedef typename TInputImage::PixelType   InputPixelType;
  typedef typename TOutputImage::PixelType  OutputPixelType;
  
  /** Image related typedefs. */
  typedef typename TInputImage::Pointer   InputImagePointer;
  typedef typename TOutputImage::Pointer  OutputImagePointer;

  typedef typename TInputImage::SizeType    InputSizeType;
  typedef typename TInputImage::IndexType   InputIndexType;
  typedef typename TInputImage::RegionType  InputImageRegionType;
  typedef typename TOutputImage::SizeType   OutputSizeType;
  typedef typename TOutputImage::IndexType  OutputIndexType;
  typedef typename TOutputImage::RegionType OutputImageRegionType;

  /** Image related typedefs. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension );
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension );
 
  /** Type of the input Sample */
  typedef TSample                        SampleType;

  /** typedefs Output type */
  typedef MembershipSample< SampleType >                     MembershipSampleType;
  typedef typename MembershipSampleType::Pointer             MembershipSampleObjectPointer; 

  /** typedefs from SampleType object */
  typedef typename SampleType::MeasurementType            MeasurementType;
  typedef typename SampleType::MeasurementVectorType      MeasurementVectorType;

  /** typedef for the MembershipFunction */
  typedef MembershipFunctionBase< MeasurementVectorType > MembershipFunctionType;
  typedef typename MembershipFunctionType::ConstPointer   MembershipFunctionPointer;
  typedef std::vector< MembershipFunctionPointer >        MembershipFunctionVectorType;
  typedef SimpleDataObjectDecorator<
    MembershipFunctionVectorType >                        MembershipFunctionVectorObjectType;
  typedef typename 
    MembershipFunctionVectorObjectType::Pointer           MembershipFunctionVectorObjectPointer;

  /** typedef for membership functions weight proprtion */
  typedef Array< double >                                MembershipFunctionsWeightsArrayType;

  typedef SimpleDataObjectDecorator<
   MembershipFunctionsWeightsArrayType >                 MembershipFunctionsWeightsArrayObjectType;
  typedef typename 
    MembershipFunctionsWeightsArrayObjectType::Pointer   MembershipFunctionsWeightsArrayPointer;

  /** Types required for the pipeline infrastructure */
  typedef typename DataObject::Pointer                DataObjectPointer;
  
  typedef unsigned long                               ClassLabelType;
  typedef std::vector< ClassLabelType >               ClassLabelVectorType;
  typedef SimpleDataObjectDecorator<
    ClassLabelVectorType >                            ClassLabelVectorObjectType;
  typedef ClassLabelVectorObjectType::Pointer         ClassLabelVectorObjectPointer;


  /** type of the decision rule */
  typedef DecisionRule                                DecisionRuleType;
  typedef DecisionRuleType::ConstPointer              DecisionRulePointer;

  /** Sets the input sample that will be classified by this filter. */
  void SetInput(const SampleType * sample);
  const SampleType *  GetInput() const;

  /** Returns the classification result */
  const MembershipSampleType * GetOutput() const;

  /** Number of classes. This must match the number of labels and membership
   * functions provided by the user, otherwise an exception will be thrown at
   */
  itkSetMacro( NumberOfClasses, unsigned int );
  itkGetMacro( NumberOfClasses, unsigned int );

  /** Set/Get the decision rule. */
  itkSetConstObjectMacro( DecisionRule, DecisionRuleType );
  itkGetConstObjectMacro( DecisionRule, DecisionRuleType );

  /** Sets input vector of class labels. The length of this vector must match
   * the number of classes, otherwise an exception will be thrown at run time.
   * */
  void SetClassLabels(const ClassLabelVectorObjectType * classLabels );

  /** Sets input vector of membership functions. The length of this vector must match
   * the number of classes, otherwise an exception will be thrown at run time.
   * */
  void SetMembershipFunctions(const MembershipFunctionVectorObjectType * membershipFunctions );

  /** Sets array of weights for the membership functions */
  void SetMembershipFunctionsWeightsArray(const MembershipFunctionsWeightsArrayObjectType * weightsArray );


protected:
  ImageClassifierFilter();
  virtual ~ImageClassifierFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  ImageClassifierFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  /** Starts the classification process */
  void GenerateData();

  /** Make a DataObject of the correct type to used as the specified
   * output. This method
   * is automatically called when DataObject::DisconnectPipeline() is
   * called.  
   * \sa ProcessObject
   */
  virtual DataObjectPointer MakeOutput(unsigned int idx);
 
private:
 
  unsigned int                     m_NumberOfClasses;

  /** Decision Rule */
  DecisionRulePointer              m_DecisionRule;

}; // end of class


} // end of namespace Statistics 
} // end of namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageClassifierFilter.txx"
#endif

#endif
