/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkEvents.h,v $
  Language:  C++
  Date:      $Date: 2005/12/05 22:45:05 $
  Version:   $Revision: 1.9 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstk_Events_h_
#define __igstk_Events_h_

/** 
 * This file contains the declarations of IGSTK Events used to communicate among components.
 */

#include "igstkTransform.h"

namespace igstk 
{
#define igstkEventMacro( classname, superclassname ) \
            itkEventMacro( classname, superclassname )

#define igstkLoadedEventMacro( name, superclass, payloadtype ) \
class  name : public superclass \
{ \
public:  \
  typedef name Self; \
  typedef superclass Superclass; \
  typedef payloadtype PayloadType; \
  name() {} \
  virtual ~name() {} \
  virtual const char * GetEventName() const { return #name; } \
  virtual bool CheckEvent(const ::itk::EventObject* e) const \
    { return dynamic_cast<const Self*>(e); } \
  virtual ::itk::EventObject* MakeObject() const \
    { return new Self; } \
  name(const Self&s) :superclass(s){}; \
  const PayloadType & Get() const \
    { return m_Payload; }  \
  void Set( const payloadtype & _var ) \
    { m_Payload = _var; }  \
private: \
  void operator=(const Self&);  \
  PayloadType  m_Payload; \
};

igstkEventMacro( IGSTKEvent,               itk::UserEvent );
igstkEventMacro( PulseEvent,               IGSTKEvent );
igstkEventMacro( RefreshEvent,             IGSTKEvent );
igstkEventMacro( CompletedEvent,           IGSTKEvent );
igstkEventMacro( InputOutputErrorEvent,    IGSTKEvent );
igstkEventMacro( InputOutputTimeoutEvent,  IGSTKEvent );
igstkEventMacro( OpenPortErrorEvent,       IGSTKEvent );
igstkEventMacro( ClosePortErrorEvent,      IGSTKEvent );

namespace EventHelperType 
{
typedef itk::Point< double, 3 >    PointType;
typedef std::string                StringType;
typedef unsigned int               UnsignedIntType;
typedef signed int                 SignedIntType;
typedef float                      FloatType;
typedef double                     DoubleType;
typedef struct {
  unsigned int minimum;
  unsigned int maximum;
}                                  IntegerBoundsType;
}

igstkLoadedEventMacro( PointEvent, IGSTKEvent, EventHelperType::PointType );
igstkLoadedEventMacro( TransformModifiedEvent, IGSTKEvent, Transform );
igstkLoadedEventMacro( StringEvent, IGSTKEvent, EventHelperType::StringType );
igstkLoadedEventMacro( UnsignedIntEvent, IGSTKEvent, EventHelperType::UnsignedIntType );
igstkLoadedEventMacro( IntegerBoundsEvent, IGSTKEvent, EventHelperType::IntegerBoundsType );

igstkEventMacro( AxialSliceBoundsEvent,      IntegerBoundsEvent );
igstkEventMacro( SagittalSliceBoundsEvent,   IntegerBoundsEvent );
igstkEventMacro( CoronalSliceBoundsEvent,    IntegerBoundsEvent );

}


#endif
