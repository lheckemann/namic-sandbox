/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkMeshObjectRepresentation.h,v $
  Language:  C++
  Date:      $Date: 2005/12/19 23:02:01 $
  Version:   $Revision: 1.7 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMeshObjectRepresentation_h
#define __igstkMeshObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkMeshObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class MeshObjectRepresentation
 * 
 * \brief This class represents a Mesh object.
 * 
 *
 *  \image html  igstkMeshObjectRepresentation.png  "MeshObjectRepresentation State Machine Diagram"
 *  \image latex igstkMeshObjectRepresentation.eps  "MeshObjectRepresentation State Machine Diagram" 
 *
 * \ingroup ObjectRepresentation
 */

class IGSTK_EXPORT MeshObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MeshObjectRepresentation, ObjectRepresentation )

public:
    
  /** Typedefs */
  typedef MeshObject                     MeshObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetMeshObject( const MeshObjectType * MeshObject );

protected:

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
  
  /** Constructor */
  MeshObjectRepresentation( void );

  /** Destructor */
  ~MeshObjectRepresentation( void );

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  MeshObjectType::ConstPointer   m_MeshObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetMeshObjectProcessing(); 

  /** Null operation for a State Machine transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidMeshObject );
  igstkDeclareInputMacro( NullMeshObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullMeshObject );
  igstkDeclareStateMacro( ValidMeshObject );

  MeshObjectType::ConstPointer m_MeshObjectToAdd;

};


} // end namespace igstk

#endif // __igstkMeshObjectRepresentation_h

