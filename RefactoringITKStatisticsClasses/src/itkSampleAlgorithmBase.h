/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSampleAlgorithmBase.h,v $
  Language:  C++
  Date:      $Date: 2003/12/15 01:00:46 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkSampleAlgorithmBase_h
#define __itkSampleAlgorithmBase_h

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include "itkMacro.h"
#include "itkObjectFactory.h"
#include "itkObject.h"

namespace itk{ 
  namespace Statistics{
  
/** \class SampleAlgorithmBase
 * \brief calculates sample mean
 *
 * You plug in the target sample data using SetSample method. Then call
 * the GenerateData method to run the alogithm.
 *
 * The return value that the GetOutput method 
 * \f$ = \frac{1}{n}\sum^{n}_{i=1} \f$ where \f$n\f$ is the
 * number of measurement vectors in the target 
 *
 */

template< class TInputSample >
class ITK_EXPORT SampleAlgorithmBase : public Object
{
public:
  /**Standard class typedefs. */
  typedef SampleAlgorithmBase Self;
  typedef Object Superclass ;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /**Standard Macros */
  itkTypeMacro(SampleAlgorithmBase, Object);
  itkNewMacro(Self) ;
  
  /** Sample typedefs alias */
  typedef TInputSample InputSampleType ;

  /** Stores the sample pointer */
  void SetInputSample( const TInputSample * sample ) 
  {
    if ( m_InputSample != sample )
      {
        m_InputSample = sample ;
        this->Modified() ;
      }
  }

  const TInputSample * GetInputSample() const
  { return m_InputSample.GetPointer() ; }

  /** dummy function that calls the GenerateData() function to generate
   * output. It exists for future compatibility with ProcessObject 
   * without streaming */
  void Update()
  { this->GenerateData() ; }
    
protected:
  SampleAlgorithmBase() ;
  virtual ~SampleAlgorithmBase() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Calculates the mean and save it */
  virtual void GenerateData() ;

private:
  /** Target sample data pointer */
  typename TInputSample::ConstPointer m_InputSample ;
} ; // end of class
    
  } // end of namespace Statistics 
} // end of namespace itk 

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSampleAlgorithmBase.txx"
#endif

#endif

