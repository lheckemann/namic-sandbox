/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkMRImageSpatialObjectRepresentation.h,v $
  Language:  C++
  Date:      $Date: 2005/12/11 12:10:22 $
  Version:   $Revision: 1.3 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkMRImageSpatialObjectRepresentation_h
#define __igstkMRImageSpatialObjectRepresentation_h

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkMRImageSpatialObject.h"

namespace igstk
{

/** \class MRImageSpatialObjectRepresentation
 *  \brief This class implements a spatial object represenation for MRI modality.
 *
 * \ingroup Spatial object representation
 */

class MRImageSpatialObjectRepresentation : 
         public ImageSpatialObjectRepresentation< MRImageSpatialObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MRImageSpatialObjectRepresentation, \
                        ImageSpatialObjectRepresentation< MRImageSpatialObject > )

protected:

  MRImageSpatialObjectRepresentation( void );
  virtual ~MRImageSpatialObjectRepresentation( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkMRImageSpatialObjectRepresentation_h

