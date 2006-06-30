/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFITSImageIO.h,v $
  Language:  C++
  Date:      $Date: 2005/09/28 15:41:54 $
  Version:   $1.0$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFITSImageIO_h
#define __itkFITSImageIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <fstream>
#include <stdio.h>
#include <fitsio.h>
#include <itkImageIOBase.h>

namespace itk
{

  
/** \class FITSImageIO
 *
 *  \brief Read FITS file format. 
 *  TODO: Add here a link to documentation...
 *
 *  \ingroup IOFilters
 *
 */
class ITK_EXPORT FITSImageIO : public ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef FITSImageIO         Self;
  typedef ImageIOBase         Superclass;
  typedef SmartPointer<Self>  Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(FITSImageIO, ImageIOBase);

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanReadFile(const char* filename) ;

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation();
  
  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void* buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine the file type. Returns true if this ImageIO can write the
   * file specified. */
  virtual bool CanWriteFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteImageInformation();
  
  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void Write(const void* buffer);


protected:

  FITSImageIO() {}; /* default ctor */
  virtual void PrintSelf(std::ostream& os, Indent indent) const;
  
private:

  // Deactivate object copying:
  FITSImageIO(const Self&);
  void operator=(const Self&);

  // Instance variables:
  fitsfile* m_fitsFile;
};

} // end namespace itk

#endif // __itkFITSImageIO_h
