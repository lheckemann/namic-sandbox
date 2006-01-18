/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkCylinderObject.h,v $
  Language:  C++
  Date:      $Date: 2005/12/11 12:10:22 $
  Version:   $Revision: 1.5 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCylinderObject_h
#define __igstkCylinderObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include <itkCylinderSpatialObject.h>

namespace igstk
{

/** \class CylinderObject
 * 
 * \brief This class represents a cylinder object. The parameters of the object
 * are the height of the object, and the radius. Default representation axis is X.
 * 
 * \ingroup Object
 */

class IGSTK_EXPORT CylinderObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CylinderObject, SpatialObject )

public:

  /** Typedefs */
  typedef itk::CylinderSpatialObject     CylinderSpatialObjectType;

  /** Set the radius of the Cylinder */
  void SetRadius( double radius );
  
  /** Get the radius of the Cylinder */
  double GetRadius() const;
  
  /** Set the height of the Cylinder */
  void SetHeight( double height );
  
  /** Get the height of the Cylinder */
  double GetHeight() const;
  
protected:

  CylinderObject( void );
  ~CylinderObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  CylinderSpatialObjectType::Pointer   m_CylinderSpatialObject;

};

} // end namespace igstk

#endif // __igstkCylinderObject_h

