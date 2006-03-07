/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: mrmlEventObject.h,v $
  Language:  C++
  Date:      $Date: 2005/05/02 11:52:49 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.mrml.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __mrmlEventObject_h
#define __mrmlEventObject_h

#include "mrmlIndent.h"


namespace mrml
{

/** \class EventObject
 * \brief Abstraction of the Events used to communicating among filters
    and with GUIs.
 *
 * EventObject provides a standard coding for sending and receiving messages
 * indicating things like the initiation of processes, end of processes,
 * modification of filters.
 *
 * EventObjects form a hierarchy similar to the mrml::ExceptionObject allowing
 * to factorize common events in a tree-like structure. Higher detail can
 * be assigned by users by subclassing existing mrml::EventObjects.
 *
 * EventObjects are used by mrml::Command and mrml::Object for implementing the
 * Observer/Subject design pattern. Observers register their interest in 
 * particular kinds of events produced by a specific mrml::Object. This 
 * mechanism decouples classes among them.
 *
 * As opposed to mrml::Exception, mrml::EventObject does not represent error 
 * states, but simply flow of information allowing to trigger actions 
 * as a consequence of changes occurring in state on some mrml::Objects.
 *
 * mrml::EventObject carries information in its own type, it relies on the 
 * appropiate use of the RTTI (Run Time Type Information).
 *
 * A set of standard EventObjects is defined near the end of mrmlEventObject.h.
 *
 * \sa mrml::Command
 * \sa mrml::ExceptionObject
 *
 * \ingroup ITKSystemObjects 
 */
class MRMLCommon_EXPORT EventObject
{
public:
  /** Constructor and copy constructor.  Note that these functions will be
   * called when children are instantiated. */
  EventObject() {}
  
  /** Virtual destructor needed  */
  virtual ~EventObject() {}
  
  /**  Create an Event of this type This method work as a Factory for
   *  creating events of each particular type. */
  virtual EventObject* MakeObject() const=0;  
  
  /** Print Event information.  This method can be overridden by
   * specific Event subtypes.  The default is to print out the type of
   * the event. */
  virtual void Print(std::ostream& os) const;
  
  /** Return the StringName associated with the event. */
  virtual const char * GetEventName(void) const=0;
  
  /** Check if given event matches or derives from this event. */
  virtual bool CheckEvent(const EventObject*) const=0;
  
protected:
  /** Methods invoked by Print() to print information about the object
   * including superclasses. Typically not called by the user (use Print()
   * instead) but used in the hierarchical print process to combine the
   * output of several classes.  */
  virtual void PrintSelf(std::ostream& os, Indent indent) const;
  virtual void PrintHeader(std::ostream& os, Indent indent) const;
  virtual void PrintTrailer(std::ostream& os, Indent indent) const;
   
  EventObject(const EventObject&){};
private:
  typedef  EventObject * EventFactoryFunction();
  void operator=(const EventObject&);
};


/** Generic inserter operator for EventObject and its subclasses. */
inline std::ostream& operator<<(std::ostream& os, EventObject &e)
{
  (&e)->Print(os);
  return os;
}




/*
 *  Macro for creating new Events
 */
#define mrmlEventMacro( classname , super ) \
 /** \class classname */  \
 class  classname : public super { \
   public: \
     typedef classname Self; \
     typedef super Superclass; \
     classname() {} \
     virtual ~classname() {} \
     virtual const char * GetEventName() const { return #classname; } \
     virtual bool CheckEvent(const ::mrml::EventObject* e) const \
       { return dynamic_cast<const Self*>(e); } \
     virtual ::mrml::EventObject* MakeObject() const \
       { return new Self; } \
     classname(const Self&s) :super(s){}; \
   private: \
     void operator=(const Self&); \
 }



/**
 *      Define some common ITK events
 */
mrmlEventMacro( NoEvent            , EventObject );
mrmlEventMacro( AnyEvent           , EventObject );
mrmlEventMacro( DeleteEvent        , AnyEvent );
mrmlEventMacro( StartEvent         , AnyEvent );
mrmlEventMacro( EndEvent           , AnyEvent );
mrmlEventMacro( ProgressEvent      , AnyEvent );
mrmlEventMacro( ExitEvent          , AnyEvent );
mrmlEventMacro( AbortEvent         , AnyEvent );
mrmlEventMacro( ModifiedEvent      , AnyEvent );
mrmlEventMacro( InitializeEvent    , AnyEvent );
mrmlEventMacro( IterationEvent     , AnyEvent );
mrmlEventMacro( PickEvent          , AnyEvent );
mrmlEventMacro( StartPickEvent     , PickEvent );
mrmlEventMacro( EndPickEvent       , PickEvent );
mrmlEventMacro( AbortCheckEvent    , PickEvent );
mrmlEventMacro( FunctionEvaluationIterationEvent, IterationEvent );
mrmlEventMacro( GradientEvaluationIterationEvent, IterationEvent );
mrmlEventMacro( FunctionAndGradientEvaluationIterationEvent, IterationEvent );

mrmlEventMacro( UserEvent          , AnyEvent );

   
} // end namespace mrml

#endif

