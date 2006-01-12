/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkMRImageReader.h,v $
  Language:  C++
  Date:      $Date: 2005/12/11 12:10:22 $
  Version:   $Revision: 1.3 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkMRImageReader_h
#define __igstkMRImageReader_h

#include "igstkDICOMImageReader.h"
#include "igstkMRImageSpatialObject.h"

namespace igstk
{

/** \class MRImageReader
 *  \brief This class implements a reader specific for MR modality.
 *
 *  This class derives from the DICOMImageReader. It is intended for loading
 *  MRI datasets and verify their modality to be MRI.
 *
 * \ingroup Readers
 */

class MRImageReader : 
         public DICOMImageReader< MRImageSpatialObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MRImageReader, DICOMImageReader< MRImageSpatialObject > )

protected:

  MRImageReader( void );
  virtual ~MRImageReader( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
  
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  MRImageReader(const Self&);         //purposely not implemented
  void operator=(const Self&);        //purposely not implemented


};

} // end namespace igstk

#endif // __igstkMRImageReader_h

