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
  
//! \class FITSImageIO
//!
//! \brief Read FITS file format. 
//!
//! TODO: Add here a link to documentation...
//!
//! \ingroup IOFilters

class ITK_EXPORT FITSImageIO : public ImageIOBase
{
public:

  // Standard class typedefs:
  typedef FITSImageIO         Self;
  typedef ImageIOBase         Superclass;
  typedef SmartPointer<Self>  Pointer;
  
  //! Method for creation through the object factory.
  itkNewMacro(Self);

  //! Run-time type information (and related methods).
  itkTypeMacro(FITSImageIO, ImageIOBase);

  // Virtual methods implementing pure virtual methods of ImageIOBase:
  virtual bool CanReadFile(const char* filename) ;
  virtual void ReadImageInformation();
  virtual void Read(void* buffer);
  virtual bool CanWriteFile(const char*);
  virtual void WriteImageInformation();
  virtual void Write(const void* buffer);

protected:

  FITSImageIO() {}; // default ctor

  // Virtual methods overriding partially implemented methods of ImageIOBase:
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
