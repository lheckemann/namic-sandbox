/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkEllipsoidObject.h,v $
  Language:  C++
  Date:      $Date: 2005/12/11 12:10:22 $
  Version:   $Revision: 1.5 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkEllipsoidObject_h
#define __igstkEllipsoidObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "itkEllipseSpatialObject.h"

namespace igstk
{

/** \class EllipsoidObject
 * 
 * \brief This class represents an ellipsoid object. The parameters of the object
 * are the three radii defining the lenght of the principal axes of the ellipsoid.
 * The axes of the ellipsoid are orthogonal.
 * 
 * \ingroup Object
 */

class IGSTK_EXPORT EllipsoidObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( EllipsoidObject, SpatialObject )

public:

  /** Typedefs */
  typedef itk::EllipseSpatialObject<3>   EllipseSpatialObjectType;
  typedef EllipseSpatialObjectType::ArrayType         ArrayType;

  /** Set the radius of the underlying ITK Spatial object */
  void SetRadius( double rx, double ry, double rz );
  void SetRadius( const ArrayType & radius );

  /** Get the three radius */
  const ArrayType & GetRadius() const;

protected:

  EllipsoidObject( void );
  ~EllipsoidObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  EllipseSpatialObjectType::Pointer   m_EllipseSpatialObject;

};

} // end namespace igstk

#endif // __igstkEllipsoidObject_h

