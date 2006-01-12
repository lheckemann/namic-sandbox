/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkObjectRepresentation.h,v $
  Language:  C++
  Date:      $Date: 2005/12/19 23:02:01 $
  Version:   $Revision: 1.13 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkObjectRepresentation_h
#define __igstkObjectRepresentation_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <vector>
#include "itkCommand.h"
#include "itkLogger.h"
#include "vtkProp.h"
#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "igstkStateMachine.h"


namespace igstk
{

class View;

/** \class ObjectRepresentation
 * 
 * \brief Base class for all the igstk representation objects.
 *
 * This class serves as the base class for all the representation objects that
 * will provide a VTK visualization of the Spatial Objects that are composing a
 * give scene.
 *
 *
 *  \image html  igstkObjectRepresentation.png  "ObjectRepresentation State Machine Diagram"
 *  \image latex igstkObjectRepresentation.eps  "ObjectRepresentation State Machine Diagram" 
 *
 * \ingroup Object
 */
class ObjectRepresentation 
  : public Object
{

public:

  typedef ObjectRepresentation               Self;
  typedef double                             ScalarType;
  typedef itk::SmartPointer < Self >         Pointer;
  typedef itk::SmartPointer < const Self >   ConstPointer;
  typedef Object                             Superclass;
  typedef SpatialObject                      SpatialObjectType;
  typedef std::vector<vtkProp*>            ActorsListType; 

  igstkTypeMacro(ObjectRepresentation, Object);

  igstkFriendClassMacro( View );

  /** Set the color */
  void SetColor(float r, float g, float b);

  /** Get each color component */
  float GetRed() const   {return m_Color[0];}
  float GetGreen() const {return m_Color[1];}
  float GetBlue() const  {return m_Color[2];}

  /** Set/Get the opacity */
  igstkSetMacro(Opacity,float);
  igstkGetMacro(Opacity,float);

  /** Has the object been modified */
  bool IsModified() const;

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** Declarations needed for the Logging */
  igstkLoggerMacro();
  
protected:

  ObjectRepresentation( void );
  ~ObjectRepresentation( void );

  /** Add an actor to the list */
  void AddActor( vtkProp * );

  /** Create the vtkActors */
  virtual void CreateActors()= 0;

  /** Get the VTK actors */
  igstkGetMacro( Actors, ActorsListType );

  /** Empty the list of actors */
  virtual void DeleteActors();

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Request the state machine to set a Spatial Object */
  void RequestSetSpatialObject( const SpatialObjectType * spatialObject );
  
private:

  ActorsListType              m_Actors;
  float                       m_Color[3];
  float                       m_Opacity;
  unsigned long               m_LastMTime;

  SpatialObjectType::ConstPointer  m_SpatialObject;

  /** update the visual representation with changes in the geometry */
  virtual void RequestUpdateRepresentation( const TimeStamp & time );

  /** update the visual representation with changes in the geometry. Only to be
   * called by the State Machine. This is an abstract method that MUST be
   * overloaded in every derived class. */
  virtual void UpdateRepresentationProcessing() = 0;
 
  /** update the visual representation position */
  virtual void RequestUpdatePosition( const TimeStamp & time );

  /** Null operation for a State Machine transition */
  void NoProcessing();

  /** update the position of the object. This should not be overloaded by other classes */
  void UpdatePositionProcessing();

  /** Set the spatial object for this class */
  void SetSpatialObjectProcessing(); 

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidSpatialObject );
  igstkDeclareInputMacro( NullSpatialObject );
  igstkDeclareInputMacro( UpdatePosition );
  igstkDeclareInputMacro( UpdateRepresentation );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullSpatialObject );
  igstkDeclareStateMacro( ValidSpatialObject );

  SpatialObjectType::ConstPointer m_SpatialObjectToAdd;

  /** Time stamp for the time at which the next rendering will take place */
  TimeStamp            m_TimeToRender;

};

} // end namespace igstk

#endif // __igstkObjectRepresentation_h

