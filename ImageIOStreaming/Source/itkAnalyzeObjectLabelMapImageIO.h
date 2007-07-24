/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkAnalyzeObjectLabelMapImageIO.h,v $
Language:  C++
Date:      $Date: 2007/03/29 20:11:16 $
Version:   $Revision: 1.5 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __itkAnalyzeObjectLabelMapImageIO_h
#define __itkAnalyzeObjectLabelMapImageIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <fstream>
#include "itkImageIOBase.h"
#include <objectmap.h>
#include <itkImageRegionIterator.h>
#include <objectentry.h>

namespace itk
{

/** \class AnalyzeObjectLabelMapImageIO
 *
 */
class ITK_EXPORT AnalyzeObjectLabelMapImageIO : public ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef AnalyzeObjectLabelMapImageIO       Self;
  typedef ImageIOBase        Superclass;
  typedef SmartPointer<Self> Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(AnalyzeObjectLabelMapImageIO, Superclass);

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine if the file can be read with this ImageIO implementation.
   * \author Hans J Johnson
   * \param FileNameToRead The name of the file to test for reading.
   * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
   * \return Returns true if this ImageIO can read the file specified.
   */
  virtual bool CanReadFile(const char* FileNameToRead);

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation();

  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void* buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine if the file can be written with this ImageIO implementation.
   * \param FileNameToWrite The name of the file to test for writing.
   * \author Hans J. Johnson
   * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
   * \return Returns true if this ImageIO can write the file specified.
   */
  virtual bool CanWriteFile(const char * FileNameToWrite);

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteImageInformation();

  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void Write(const void* buffer);

  /** Calculate the region of the image that can be efficiently read 
   *  in response to a given requested region. */
  virtual ImageIORegion 
  CalculateStreamableReadRegionFromRequestedRegion( const ImageIORegion & requestedRegion ) const;

  virtual bool CanStreamRead()
    {
    return false;
    }
protected:
  AnalyzeObjectLabelMapImageIO() ;
  ~AnalyzeObjectLabelMapImageIO();
  void PrintSelf(std::ostream& os, Indent indent) const;
private:
  AnalyzeObjectEntry::Pointer AnaylzeObjectEntryArray[256];
  itk::AnalyzeObjectMap::Pointer m_AnalyzeObjectLabelMapImage;
  //  int           m_CollapsedDims[8];
  AnalyzeObjectLabelMapImageIO(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk

#endif // __itkAnalyzeObjectLabelMapImageIO_h
