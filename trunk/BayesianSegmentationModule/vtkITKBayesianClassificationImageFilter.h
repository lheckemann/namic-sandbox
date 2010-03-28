/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKBayesianClassificationImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/04/19 18:47:06 $
  Version:   $Revision: 1.4 $
*/
// .NAME vtkITKBayesianClassificationImageFilter - Wrapper class around itk::BayesianClassificationImageFilter
// .SECTION Description
// vtkITKBayesianClassificationImageFilter


#ifndef __vtkITKBayesianClassificationImageFilter_h
#define __vtkITKBayesianClassificationImageFilter_h


#include "vtkRuleBasedSegmentationConfigure.h"

#include "vtkITKImageToImageFilterULUL.h"
#include "itkBayesianClassificationImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_RULEBASEDSEGMENTATION_EXPORT vtkITKBayesianClassificationImageFilter : public vtkITKImageToImageFilterULUL
{
 public:
  static vtkITKBayesianClassificationImageFilter *New();
  vtkTypeRevisionMacro(vtkITKBayesianClassificationImageFilter, vtkITKImageToImageFilterULUL);

  void PrintSelf( ostream &os, vtkIndent indent )
  {
    Superclass::PrintSelf( os, indent );
  }

  void SetNumberOfClasses( int n )
  {
    DelegateITKInputMacro( SetNumberOfClasses, static_cast< unsigned int >( n ) );
  }

  int GetNumberOfClasses()
  {
    DelegateITKOutputMacro( GetNumberOfClasses);
  }
 
  void SetNumberOfSmoothingIterations( int n )
  {
    DelegateITKInputMacro( SetNumberOfSmoothingIterations, static_cast< unsigned int >( n ) );
  }

  int GetNumberOfSmoothingIterations()
  {
    DelegateITKOutputMacro( GetNumberOfSmoothingIterations);
  }
//   void SetMaskImage( ??? )
//     {
//     DelegateITKInputMacro( SetMaskImage, static_cast< unsigned int >( n ) ); // CAN WE USE SOMETHING LIKE THIS?
//     }

//   int GetMaskImage()
//     {
//     DelegateITKOutputMacro( GetMaskImage); // CAN WE USE SOMETHING LIKE THIS?
//     }

//  void SetMaskValue( int n )
//  {
//    DelegateITKInputMacro( SetMaskValue, static_cast< unsigned int >( n ) );
//  }

//  int GetMaskValue()
//  {
//    DelegateITKOutputMacro( GetMaskValue );
//  }

protected:
  //BTX
//   typedef itk::BayesianClassificationImageFilter<
//   Superclass::InputImageType,
//   Superclass::OutputImageType,
//   Superclass::InputImageType  // TODO: Change to a MaskImageType
//   > ImageFilterType;
  typedef itk::BayesianClassificationImageFilter< Superclass::InputImageType,
                                                  Superclass::OutputImageType > ImageFilterType;
  vtkITKBayesianClassificationImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKBayesianClassificationImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX
  
private:
  vtkITKBayesianClassificationImageFilter(const vtkITKBayesianClassificationImageFilter&);  // Not implemented.
  void operator=(const vtkITKBayesianClassificationImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKBayesianClassificationImageFilter, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkITKBayesianClassificationImageFilter);

#endif