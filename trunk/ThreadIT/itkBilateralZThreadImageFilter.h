/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBilateralZThreadImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/24 16:03:16 $
  Version:   $Revision: 1.19 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBilateralZThreadImageFilter_h
#define __itkBilateralZThreadImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkBilateralImageFilter.h"
#include "itkMultiZThreader.h"

namespace itk
{
/**
 * \class BilateralZThreadImageFilter
 *        Test out new MultiZThreader.
*/
template <class TInputImage, class TOutputImage >
class ITK_EXPORT BilateralZThreadImageFilter :
    public BilateralImageFilter< TInputImage, TOutputImage > 
{
public:
  /** Standard class typedefs. */
  typedef BilateralZThreadImageFilter Self;
  typedef BilateralImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(BilateralZThreadImageFilter, BilateralImageFilter);
  
  /** Return the MultiZThreader used by this class */
  MultiZThreader* GetMultiZThreader()
  { return m_ZThreader; }

 protected:
  BilateralZThreadImageFilter()
    {
    m_ZThreader = MultiZThreader::New();
    }

  /**
    * MultiZThreader
    */
  MultiZThreader::Pointer m_ZThreader;

  /** Internal structure used for passing image data into the threading library */
  struct ThreadStruct
  {
   Pointer Filter;
  };

  /** Override GenerateData to use our threader.
   */
  virtual void GenerateData()
  {
    // Call a method that can be overriden by a subclass to allocate
    // memory for the filter's outputs
    this->AllocateOutputs();
  
    // Call a method that can be overridden by a subclass to perform
    // some calculations prior to splitting the main computations into
    // separate threads
    this->BeforeThreadedGenerateData();
  
    // Set up the multithreaded processing
    ThreadStruct str;
    str.Filter = this;
  
    this->GetMultiZThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
    this->GetMultiZThreader()->SetSingleMethod(this->ThreaderCallback, &str);
  
    // multithread the execution
    this->GetMultiZThreader()->SingleMethodExecute();

    // Call a method that can be overridden by a subclass to perform
    // some calculations after all the threads have completed
    this->AfterThreadedGenerateData();
  }


private:
  BilateralZThreadImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};
  
} // end namespace itk

#endif
