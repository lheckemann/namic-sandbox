/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLevelSetBasedCellSegmentation.h,v $
  Language:  C++
  Date:      $Date: 2004/06/01 18:33:44 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLevelSetBasedCellSegmentation_h_
#define __itkLevelSetBasedCellSegmentation_h_

#include "itkSegmentationLevelSetImageFilter.h"
#include "itkGeodesicActiveContourLevelSetFunction.h"

namespace itk {

/** \class LevelSetBasedCellSegmentation
 * \brief Segments cells
 *
 *
 * \ingroup LevelSetSegmentation
 */
template <class TFeatureImage,
          class TMaskImage,
          class TOutputLabelImage >
class ITK_EXPORT LevelSetBasedCellSegmentation
  : public ImageToImageFilter< TFeatureImage, TOutputLabelImage>
{
public:
  /** Standard class typedefs */
  typedef LevelSetBasedCellSegmentation                 Self;
  typedef ImageToImageFilter< TFeatureImage, 
                              TOutputLabelImage>        Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;

  /** Inherited typedef from the superclass. */
  typedef typename Superclass::OutputImageType          OutputImageType;
  typedef typename Superclass::InputImageType           InputImageType;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(LevelSetBasedCellSegmentation, SegmentationLevelSetImageFilter);

  /** Method for creation through the object factory */
  itkNewMacro(Self);
     
  /** Feature image to be used as input. It should look like an intensity
   * inversion of a gradient magnitude image. */  
  void SetFeatureImage( const TFeatureImage * featureImage );

  /** Mask image defining the foreground ( masks over the cells ). 
   **/  
  void SetMaskImage( const TMaskImage * maskImage );

protected:
  ~LevelSetBasedCellSegmentation();
  LevelSetBasedCellSegmentation();

  virtual void PrintSelf(std::ostream &os, Indent indent) const; 

  LevelSetBasedCellSegmentation(const Self &); // purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Overridden from Superclass to handle the case when PropagationScaling is zero.*/
  void GenerateData();

private:


};

} // end namespace itk



#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLevelSetBasedCellSegmentation.txx"
#endif

#endif
