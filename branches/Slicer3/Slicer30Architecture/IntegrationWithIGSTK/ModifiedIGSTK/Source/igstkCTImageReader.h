/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkCTImageReader.h,v $
  Language:  C++
  Date:      $Date: 2005/12/11 12:10:22 $
  Version:   $Revision: 1.3 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkCTImageReader_h
#define __igstkCTImageReader_h

#include "igstkDICOMImageReader.h"
#include "igstkCTImageSpatialObject.h"

namespace igstk
{

/** \class CTImageReader
 *  \brief This class implements a reader specific for CT modality.

 * This class derives from the DICOMImageReader. It expects to load a volume
 * from a set of DICOM files. It will verify that the image modality is CT.
 *
 * \ingroup Readers
 */

class IGSTK_EXPORT CTImageReader : 
         public DICOMImageReader< CTImageSpatialObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CTImageReader, DICOMImageReader< CTImageSpatialObject > )

protected:

  CTImageReader( void );
  virtual ~CTImageReader( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
  
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  CTImageReader(const Self&);         //purposely not implemented
  void operator=(const Self&);        //purposely not implemented


};

} // end namespace igstk

#endif // __igstkCTImageReader_h

