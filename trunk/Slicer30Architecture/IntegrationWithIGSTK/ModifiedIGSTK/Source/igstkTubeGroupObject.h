/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkTubeGroupObject.h,v $
  Language:  C++
  Date:      $Date: 2005/12/11 12:10:22 $
  Version:   $Revision: 1.3 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTubeGroupObject_h
#define __igstkTubeGroupObject_h

#include "igstkMacros.h"
#include "igstkGroupObject.h"
#include "igstkTubeObject.h"

namespace igstk
{

/** \class TubeGroup
 * \brief Implements the 3-dimensional TubeGroup structure.
 *
 * \par Overview
 * TubeGroup implements the 3-dimensional TubeGroup structure. 
 *
 *
 * \inTubeGroup Object
 */

class IGSTK_EXPORT TubeGroupObject 
: public GroupObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TubeGroupObject, GroupObject )

public:
    
  /** Typedefs */
  typedef TubeObject                                TubeObjectType;

  /** Get the object */
  const TubeObjectType * GetTube(unsigned long id) const;
  
protected:

  /** Constructor */
  TubeGroupObject( void );

  /** Destructor */
  ~TubeGroupObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:


};

} // end namespace igstk

#endif // __igstkTubeGroupObject_h
