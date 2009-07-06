/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkObjectByObjectLabelMapFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:09:03 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkObjectByObjectLabelMapFilter_h
#define __itkObjectByObjectLabelMapFilter_h

#include "itkLabelMapFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkLabelMapToBinaryImageFilter.h"
#include "itkLabelSelectionLabelMapFilter.h"
#include "itkAutoCropLabelMapFilter.h"
#include "itkPadLabelMapFilter.h"


namespace itk {

/** \class ObjectByObjectLabelMapFilter
 * \brief ObjectByObjectLabelMapFilter applies an image pipeline to all the objects of a label map and produce a new label map
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */
template<class TInputImage, class TOutputImage=TInputImage,
  class TInputFilter=ImageToImageFilter< 
    Image< unsigned char, ::itk::GetImageDimension<TInputImage >::ImageDimension >,
    Image< unsigned char, ::itk::GetImageDimension<TOutputImage>::ImageDimension > >,
  class TOutputFilter=TInputFilter,
  class TInternalInputImage=ITK_TYPENAME TInputFilter::InputImageType,
  class TInternalOutputImage=ITK_TYPENAME TOutputFilter::OutputImageType >
class ITK_EXPORT ObjectByObjectLabelMapFilter : 
    public LabelMapFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef ObjectByObjectLabelMapFilter                 Self;
  typedef LabelMapFilter<TInputImage, TOutputImage> Superclass;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;

  /** Some convenient typedefs. */
  typedef TInputImage                              InputImageType;
  typedef TOutputImage                             OutputImageType;
  typedef typename InputImageType::Pointer         InputImagePointer;
  typedef typename InputImageType::ConstPointer    InputImageConstPointer;
  typedef typename InputImageType::RegionType      InputImageRegionType;
  typedef typename InputImageType::PixelType       InputImagePixelType;
  typedef typename OutputImageType::Pointer        OutputImagePointer;
  typedef typename OutputImageType::ConstPointer   OutputImageConstPointer;
  typedef typename OutputImageType::RegionType     OutputImageRegionType;
  typedef typename OutputImageType::PixelType      OutputImagePixelType;
  typedef typename OutputImageType::SizeType       SizeType;
  typedef OutputImageType                          LabelMapType;
  typedef typename LabelMapType::LabelObjectType   LabelObjectType;
  
  typedef TInputFilter                          InputFilterType;
  typedef TOutputFilter                         OutputFilterType;
  
  typedef TInternalInputImage                         InternalInputImageType;
  typedef typename InternalInputImageType::RegionType InternalRegionType;
  typedef typename InternalInputImageType::SizeType   InternalSizeType;
  typedef typename InternalInputImageType::IndexType  InternalIndexType;
  typedef typename InternalInputImageType::OffsetType InternalOffsetType;
  typedef typename InternalInputImageType::PixelType  InternalInputPixelType;

  typedef TInternalOutputImage                        InternalOutputImageType;
  typedef typename InternalOutputImageType::PixelType InternalOutputPixelType;

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  // filter types used internally
  typedef itk::LabelSelectionLabelMapFilter< LabelMapType >                        SelectType;
  typedef itk::AutoCropLabelMapFilter< LabelMapType >                              CropType;
  typedef itk::PadLabelMapFilter< LabelMapType >                                   PadType;
  typedef itk::LabelMapToBinaryImageFilter< LabelMapType, InternalInputImageType>  LM2BIType;
  typedef itk::LabelImageToLabelMapFilter< InternalOutputImageType, LabelMapType>  LI2LMType;
  typedef itk::BinaryImageToLabelMapFilter< InternalOutputImageType, LabelMapType> BI2LMType;

  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(ObjectByObjectLabelMapFilter, 
               LabelMapFilter);

  void SetFilter(InputFilterType * filter);
  InputFilterType * GetFilter()
    {
    return this->m_InputFilter;
    }

  const InputFilterType * GetFilter() const
    {
    return this->m_InputFilter;
    }

  void SetInputFilter( InputFilterType * filter );
  itkGetObjectMacro( InputFilter, InputFilterType );

  void SetOutputFilter( OutputFilterType * filter );
  itkGetObjectMacro( OutputFilter, OutputFilterType );


  /** If KeepLabels is set to true, the filter will do its best to reuse the labels
   * of the input objects in the output ones. However, this is possible only if the
   * internal pipeline produce a single object - the other labels will be taken as
   * they come.
   * If KeepLabels is false, no care is made of the input labels, and a new label is produced
   * for all the objects using LabelMap::PushLabelObject().
   */
  itkSetMacro(KeepLabels, bool);
  itkGetMacro(KeepLabels, bool);
  itkBooleanMacro(KeepLabels);

  /** If PadSize is not zero, the image produce for each object will be padded */
  itkSetMacro(PadSize, SizeType);
  itkGetMacro(PadSize, SizeType);

  /** Padding by PadSize will be constrained to the input image region if
   * ConstrainPaddingToImage is true, and won't be constrained if it is set to false.
   * Default value is true.
   */
  itkSetMacro(ConstrainPaddingToImage, bool);
  itkGetMacro(ConstrainPaddingToImage, bool);
  itkBooleanMacro(ConstrainPaddingToImage);

  /** Set/Get whether the internal image produced by OutputFilter should be interpreted
   * as a binary image in which the filter have to search for connected components. If
   * set to false, the filter consider the image as a label image.
   * Default is false.
   */
  itkSetMacro(BinaryInternalOutput, bool);
  itkGetMacro(BinaryInternalOutput, bool);
  itkBooleanMacro(BinaryInternalOutput);

  /** The foreground value used internally to represent the object in the image passed to
   * InputFilter, and to read the data produced by OutputFilter, if BinaryInternalOutput
   * is true
   */
  itkSetMacro(InternalForegroundValue, InternalOutputPixelType);
  itkGetMacro(InternalForegroundValue, InternalOutputPixelType);

  /** The label of the object currently processed by the filter. This is intended to be
   * used with the IterationEvent sent before the processing of each object. It contains
   * a relevant value only during the filter update.
   */
  itkGetMacro(Label, InputImagePixelType);

protected:
  ObjectByObjectLabelMapFilter();
  ~ObjectByObjectLabelMapFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  virtual void GenerateData();
  

private:
  ObjectByObjectLabelMapFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool     m_ConstrainPaddingToImage;
  SizeType m_PadSize;
  bool     m_BinaryInternalOutput;

  bool     m_KeepLabels;
  
  InternalOutputPixelType m_InternalForegroundValue;
  

  typename InputFilterType::Pointer       m_InputFilter;
  typename OutputFilterType::Pointer      m_OutputFilter;

  typename SelectType::Pointer m_Select;
  typename CropType::Pointer   m_Crop;
  typename PadType::Pointer    m_Pad;
  typename LM2BIType::Pointer  m_LM2BI;
  typename LI2LMType::Pointer  m_LI2LM;
  typename BI2LMType::Pointer  m_BI2LM;
  
  InputImagePixelType          m_Label;

}; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkObjectByObjectLabelMapFilter.txx"
#endif

#endif
