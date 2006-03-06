/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: mrmlMacro.h,v $
  Language:  C++
  Date:      $Date: 2006/01/13 13:14:04 $
  Version:   $Revision: 1.61 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See mrmlCopyright.txt or http://www.mrml.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * mrmlMacro.h defines standard system-wide macros, constants, and other
 * parameters. One of its most important functions is to define macros used
 * to interface to instance variables in a standard fashion. For example,
 * these macros manage modified time, debugging information, and provide a
 * standard interface to set and get instance variables.  Macros are
 * available for built-in types; for string classe; vector arrays;
 * object pointers; and debug, warning, and error printout information. 
 */
  
#ifndef __mrmlMacro_h
#define __mrmlMacro_h

#include "mrmlWin32Header.h"
#include "mrmlConfigure.h"

#include <string>

// Determine type of string stream to use.
#if !defined(CMAKE_NO_ANSI_STRING_STREAM)
#  include <sstream>
#elif !defined(CMAKE_NO_ANSI_STREAM_HEADERS)
#  include <strstream>
#  define mrml_NO_ANSI_STRING_STREAM
#else
#  include <strstream.h>
#  define mrml_NO_ANSI_STRING_STREAM
#endif

/** \namespace mrml
 * \brief The "mrml" namespace contains all Insight Segmentation and
 * Registration Toolkit (mrml) classes. There are several nested namespaces
 * within the mrml:: namespace. */
namespace mrml
{
} // end namespace mrml - this is here for documentation purposes

/** A convenience macro marks variables as not being used by a method,
 * avoiding compile-time warnings. */
#define mrmlNotUsed(x)

/** Macro to initialize static constants.  This is used frequently to replace
 * the use of enum's within a class.  Some compilers do not allow an enum of
 * one class to be passed as template argument to another class. Other
 * uses of this macro as possible.
 *
 * This is based (verbatim) on the BOOST_STATIC_CONSTANT macro. The original
 * Boost documentation is below.
 *
 * BOOST_STATIC_CONSTANT workaround --------------------------------------- //
 * On compilers which don't allow in-class initialization of static integral
 * constant members, we must use enums as a workaround if we want the constants
 * to be available at compile-time. This macro gives us a convenient way to
 * declare such constants.
 */
#if defined(_MSC_VER) && (_MSC_VER <= 1300) 
#   define mrml_NO_INCLASS_MEMBER_INITIALIZATION
#endif
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x540)
#   define mrml_NO_INCLASS_MEMBER_INITIALIZATION
#endif
#if defined(__SVR4) && !defined(__SUNPRO_CC)
#   define mrml_NO_INCLASS_MEMBER_INITIALIZATION
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1300) 
#define mrml_NO_SELF_AS_TRAIT_IN_TEMPLATE_ARGUMENTS
#endif

#ifdef mrml_NO_INCLASS_MEMBER_INITIALIZATION
#   define mrmlStaticConstMacro(name,type,value) enum { name = value }
#else
#   define mrmlStaticConstMacro(name,type,value) static const type name = value
#endif

#ifdef mrml_NO_SELF_AS_TRAIT_IN_TEMPLATE_ARGUMENTS
#   define mrmlGetStaticConstMacro(name) name
#else
#   define mrmlGetStaticConstMacro(name) (Self::name)
#endif

/** Set an input. This defines the Set"name"Input() method */
#define mrmlSetInputMacro(name, type, number) \
  virtual void Set##name##Input(const type *_arg) \
  { \
    mrmlDebugMacro("setting input " #name " to " << _arg); \
    if (_arg != static_cast<type *>(this->ProcessObject::GetInput( number ))) \
      { \
      this->ProcessObject::SetNthInput( number, const_cast<type *>(_arg) ); \
      this->Modified(); \
      } \
  } \
  virtual void SetInput##number(const type *_arg) \
  { \
    mrmlDebugMacro("setting input " #number " to " << _arg); \
    if (_arg != static_cast<type *>(this->ProcessObject::GetInput( number ))) \
      { \
      this->ProcessObject::SetNthInput( number, const_cast<type *>(_arg) ); \
      this->Modified(); \
      } \
  } 


/** Get an input. This defines the Get"name"Input() method */
#define mrmlGetInputMacro(name, type, number) \
  virtual const type * Get##name##Input() const \
  { \
    mrmlDebugMacro("returning input " << #name " of " << static_cast<const type *>(this->ProcessObject::GetInput( number )) ); \
    return static_cast<const type *>(this->ProcessObject::GetInput( number )); \
  } \
  virtual const type * GetInput##number() const \
  { \
    mrmlDebugMacro("returning input " << #number " of " << static_cast<const type *>(this->ProcessObject::GetInput( number )) ); \
    return static_cast<const type *>(this->ProcessObject::GetInput( number )); \
  } 


/** Set built-in type.  Creates member Set"name"() (e.g., SetVisibility()); */
#define mrmlSetMacro(name,type) \
  virtual void Set##name (const type _arg) \
  { \
    mrmlDebugMacro("setting " #name " to " << _arg); \
    if (this->m_##name != _arg) \
      { \
      this->m_##name = _arg; \
      this->Modified(); \
      } \
  } 

/** Get built-in type.  Creates member Get"name"() (e.g., GetVisibility()); */
#define mrmlGetMacro(name,type) \
  virtual type Get##name () \
  { \
    mrmlDebugMacro("returning " << #name " of " << this->m_##name ); \
    return this->m_##name; \
  }

/** Get built-in type.  Creates member Get"name"() (e.g., GetVisibility());
 * This is the "const" form of the mrmlGetMacro.  It should be used unless
 * the member can be changed through the "Get" access routine. */
#define mrmlGetConstMacro(name,type) \
  virtual type Get##name () const \
  { \
    mrmlDebugMacro("returning " << #name " of " << this->m_##name ); \
    return this->m_##name; \
  }

/** Get built-in type.  Creates member Get"name"() (e.g., GetVisibility());
 * This is the "const" form of the mrmlGetMacro.  It should be used unless
 * the member can be changed through the "Get" access routine.
 * This versions returns a const reference to the variable. */
#define mrmlGetConstReferenceMacro(name,type) \
  virtual const type & Get##name () const \
  { \
    mrmlDebugMacro("returning " << #name " of " << this->m_##name ); \
    return this->m_##name; \
  }

/** Set character string.  Creates member Set"name"() 
 * (e.g., SetFilename(char *)). The macro assumes that
 * the class member (name) is declared a type std::string. */
#define mrmlSetStringMacro(name) \
  virtual void Set##name (const char* _arg) \
  { \
    if ( _arg && (_arg == this->m_##name) ) { return;} \
    if (_arg) \
      { \
      this->m_##name = _arg;\
      } \
     else \
      { \
      this->m_##name = ""; \
      } \
    this->Modified(); \
  } 


/** Get character string.  Creates member Get"name"() 
 * (e.g., SetFilename(char *)). The macro assumes that
 * the class member (name) is declared a type std::string. */
#define mrmlGetStringMacro(name) \
  virtual const char* Get##name () const \
  { \
    return this->m_##name.c_str(); \
  } 

/** Set built-in type where value is constrained between min/max limits.
 * Create member Set"name"() (e.q., SetRadius()). #defines are 
 * convienience for clamping open-ended values. */
#define mrmlSetClampMacro(name,type,min,max) \
  virtual void Set##name (type _arg) \
  { \
    mrmlDebugMacro("setting " << #name " to " << _arg ); \
    if (this->m_##name != (_arg<min?min:(_arg>max?max:_arg))) \
      { \
      this->m_##name = (_arg<min?min:(_arg>max?max:_arg)); \
      this->Modified(); \
      } \
  } 

/** Set pointer to object; uses Object reference counting methodology.
 * Creates method Set"name"() (e.g., SetPoints()). Note that using
 * smart pointers requires using real pointers when setting input,
 * but returning smart pointers on output. */
#define mrmlSetObjectMacro(name,type) \
  virtual void Set##name (type* _arg) \
  { \
    mrmlDebugMacro("setting " << #name " to " << _arg ); \
    if (this->m_##name != _arg) \
      { \
      this->m_##name = _arg; \
      this->Modified(); \
      } \
  } 

/** Get a smart pointer to an object.  Creates the member 
 * Get"name"() (e.g., GetPoints()). */
#define mrmlGetObjectMacro(name,type) \
  virtual type * Get##name () \
  { \
    mrmlDebugMacro("returning " #name " address " << this->m_##name ); \
    return this->m_##name.GetPointer(); \
  } 

/** Set const pointer to object; uses Object reference counting methodology.
 * Creates method Set"name"() (e.g., SetPoints()). Note that using
 * smart pointers requires using real pointers when setting input,
 * but returning smart pointers on output. */
#define mrmlSetConstObjectMacro(name,type) \
  virtual void Set##name (const type* _arg) \
  { \
    mrmlDebugMacro("setting " << #name " to " << _arg ); \
    if (this->m_##name != _arg) \
      { \
      this->m_##name = _arg; \
      this->Modified(); \
      } \
  } 


/** Get a smart const pointer to an object.  Creates the member 
 * Get"name"() (e.g., GetPoints()). */
#define mrmlGetConstObjectMacro(name,type) \
  virtual const type * Get##name () const \
  { \
    mrmlDebugMacro("returning " #name " address " << this->m_##name ); \
    return this->m_##name.GetPointer(); \
  } 

/** Get a const reference to a smart pointer to an object.  
 * Creates the member Get"name"() (e.g., GetPoints()). */
#define mrmlGetConstReferenceObjectMacro(name,type) \
  virtual const typename type::Pointer & Get##name () const \
  { \
    mrmlDebugMacro("returning " #name " address " << this->m_##name ); \
    return this->m_##name; \
  } 

/** Create members "name"On() and "name"Off() (e.g., DebugOn() DebugOff()).
 * Set method must be defined to use this macro. */
#define mrmlBooleanMacro(name) \
  virtual void name##On () { this->Set##name(true);} \
  virtual void name##Off () { this->Set##name(false);}

/** General set vector macro creates a single method that copies specified
 * number of values into object.
 * Examples: void SetColor(c,3) */
#define mrmlSetVectorMacro(name,type,count) \
  virtual void Set##name(type data[]) \
  { \
    unsigned int i; \
    for (i=0; i<count; i++) { if ( data[i] != this->m_##name[i] ) { break; }} \
    if ( i < count ) \
      { \
      this->Modified(); \
      for (i=0; i<count; i++) { this->m_##name[i] = data[i]; }\
      } \
  }

/** Get vector macro. Returns pointer to type (i.e., array of type).
 * This is for efficiency. */
#define mrmlGetVectorMacro(name,type,count) \
  virtual type *Get##name () const \
  { \
    return this->m_##name; \
  } 

/** Define two object creation methods.  The first method, New(),
 * creates an object from a class, potentially deferring to a factory.
 * The second method, CreateAnother(), creates an object from an
 * instance, potentially deferring to a factory.  This second method
 * allows you to create an instance of an object that is exactly the
 * same type as the referring object.  This is useful in cases where
 * an object has been cast back to a base class.
 *
 * These creation methods first try asking the object factory to create
 * an instance, and then default to the standard "new" operator if the
 * factory fails.
 *
 * These routines assigns the raw pointer to a smart pointer and then call
 * UnRegister() on the rawPtr to compensate for LightObject's constructor
 * initializing an object's reference count to 1 (needed for proper
 * initialization of process objects and data objects cycles). */
#define mrmlNewMacro(x) \
static Pointer New(void) \
{ \
  Pointer smartPtr; \
  x *rawPtr = ::mrml::ObjectFactory<x>::Create(); \
  if(rawPtr == NULL) \
    { \
    rawPtr = new x; \
    } \
  smartPtr = rawPtr; \
  rawPtr->UnRegister(); \
  return smartPtr; \
} \
virtual ::mrml::LightObject::Pointer CreateAnother(void) const \
{ \
  ::mrml::LightObject::Pointer smartPtr; \
  smartPtr = x::New().GetPointer(); \
  return smartPtr; \
}


/** Define two object creation methods.  The first method, New(),
 * creates an object from a class but does not defer to a factory.
 * The second method, CreateAnother(), creates an object from an
 * instance, again without deferring to a factory.  This second method
 * allows you to create an instance of an object that is exactly the
 * same type as the referring object.  This is useful in cases where
 * an object has been cast back to a base class.
 *
 * These creation methods first try asking the object factory to create
 * an instance, and then default to the standard "new" operator if the
 * factory fails.
 *
 * These routines assigns the raw pointer to a smart pointer and then call
 * UnRegister() on the rawPtr to compensate for LightObject's constructor
 * initializing an object's reference count to 1 (needed for proper
 * initialization of process objects and data objects cycles). */
#define mrmlFactorylessNewMacro(x) \
static Pointer New(void) \
{ \
  Pointer smartPtr; \
  x *rawPtr = new x; \
  smartPtr = rawPtr; \
  rawPtr->UnRegister(); \
  return smartPtr; \
} \
virtual LightObject::Pointer CreateAnother(void) const \
{ \
  LightObject::Pointer smartPtr; \
  smartPtr = x::New().GetPointer(); \
  return smartPtr; \
}

/** Macro used to add standard methods to all classes, mainly type
 * information. */
#define mrmlTypeMacro(thisClass,superclass) \
    virtual const char *GetNameOfClass() const \
        {return #thisClass;} 


namespace mrml
{
/** 
 * The following is used to output debug, warning, and error messages. 
 * Use a global function which actually calls:
 * OutputWindow::GetInstance()->DisplayText();
 * This is to avoid Object #include of OutputWindow
 * while OutputWindow #includes Object. */
extern mrmlCommon_EXPORT void OutputWindowDisplayText(const char*);
extern mrmlCommon_EXPORT void OutputWindowDisplayErrorText(const char*);
extern mrmlCommon_EXPORT void OutputWindowDisplayWarningText(const char*);
extern mrmlCommon_EXPORT void OutputWindowDisplayGenericOutputText(const char*);
extern mrmlCommon_EXPORT void OutputWindowDisplayDebugText(const char*);
} // end namespace mrml

/** This macro is used to print debug (or other information). They are
 * also used to catch errors, etc. Example usage looks like:
 * mrmlDebugMacro(<< "this is debug info" << this->SomeVariable); */
//#if defined(mrml_LEAN_AND_MEAN) || defined(__BORLANDC__)
#define mrmlDebugMacro(x)
/*
#else
#define mrmlDebugMacro(x) \
  { if (this->GetDebug() && ::mrml::Object::GetGlobalWarningDisplay())   \
    { ::mrml::OStringStream mrmlmsg; \
      mrmlmsg << "Debug: In " __FILE__ ", line " << __LINE__ << "\n" \
             << this->GetNameOfClass() << " (" << this << "): " x  \
             << "\n\n"; \
      ::mrml::OutputWindowDisplayDebugText(mrmlmsg.str().c_str());} \
}
#endif
*/

/** This macro is used to print warning information (i.e., unusual circumstance
 * but not necessarily fatal.) Example usage looks like:
 * mrmlWarningMacro(<< "this is warning info" << this->SomeVariable); */
#ifdef mrml_LEAN_AND_MEAN
#define mrmlWarningMacro(x)
#else
#define mrmlWarningMacro(x) \
{ if (mrml::Object::GetGlobalWarningDisplay()) \
    { ::mrml::OStringStream mrmlmsg; \
      mrmlmsg << "WARNING: In " __FILE__ ", line " << __LINE__ << "\n" \
             << this->GetNameOfClass() << " (" << this << "): " x  \
             << "\n\n"; \
      mrml::OutputWindowDisplayWarningText(mrmlmsg.str().c_str());} \
}
#endif

namespace mrml
{

/**
 * mrml::OStringStream wrapper to hide differences between
 * std::ostringstream and the old ostrstream.  Necessary for
 * portability.
 */
#if !defined(mrml_NO_ANSI_STRING_STREAM)
class OStringStream: public std::ostringstream
{
public:
  OStringStream() {}
private:
  OStringStream(const OStringStream&);
  void operator=(const OStringStream&);
};
#else
namespace OStringStreamDetail
{
  class Cleanup
  {
  public:
    Cleanup(std::ostrstream& ostr): m_OStrStream(ostr) {}
    ~Cleanup() { m_OStrStream.rdbuf()->freeze(0); }
    static void IgnoreUnusedVariable(const Cleanup&) {}
  protected:
    std::ostrstream& m_OStrStream;
  };
}//namespace OStringStreamDetail

class OStringStream: public std::ostrstream
{
public:
  typedef std::ostrstream Superclass;
  OStringStream() {}
  std::string str()
    {
      OStringStreamDetail::Cleanup cleanup(*this);
      OStringStreamDetail::Cleanup::IgnoreUnusedVariable(cleanup);
      int pcount = this->pcount();
      const char* ptr = this->Superclass::str();
      return std::string(ptr?ptr:"", pcount);
    }
private:
  OStringStream(const OStringStream&);
  void operator=(const OStringStream&);
};
#endif

}//namespace mrml

#if defined(mrml_CPP_FUNCTION)
  #if defined(__BORLANDC__)
    #define mrml_LOCATION __FUNC__
  #elif defined(_WIN32) && !defined(__MINGW32__) && !defined(__CYGWIN__)
    #define mrml_LOCATION __FUNCSIG__
  #elif defined(__GNUC__)
    #define mrml_LOCATION __PRETTY_FUNCTION__
  #else
    #define mrml_LOCATION __FUNCTION__
  #endif
#else
  #define mrml_LOCATION "unknown"
#endif

//#include "mrmlExceptionObject.h"

/** The exception macro is used to print error information (i.e., usually 
 * a condition that results in program failure). Example usage looks like:
 * mrmlExceptionMacro(<< "this is error info" << this->SomeVariable); */
#define mrmlExceptionMacro(x) 

#if 0
#define mrmlExceptionMacro(x) \
  { \
  ::mrml::OStringStream message; \
  message << "mrml::ERROR: " << this->GetNameOfClass() \
          << "(" << this << "): " x; \
  ::mrml::ExceptionObject e_(__FILE__, __LINE__, message.str().c_str(),mrml_LOCATION); \
  throw e_; /* Explicit naming to work around Intel compiler bug.  */ \
  }
#endif

#define mrmlGenericExceptionMacro(x) \
  { \
  ::mrml::OStringStream message; \
  message << "mrml::ERROR: " x; \
  ::mrml::ExceptionObject e_(__FILE__, __LINE__, message.str().c_str(),mrml_LOCATION); \
  throw e_; /* Explicit naming to work around Intel compiler bug.  */ \
  }

#ifdef mrml_LEAN_AND_MEAN
#define mrmlGenericOutputMacro(x)
#else
#define mrmlGenericOutputMacro(x) \
{ if (::mrml::Object::GetGlobalWarningDisplay()) \
    { ::mrml::OStringStream mrmlmsg; \
      mrmlmsg << "WARNING: In " __FILE__ ", line " << __LINE__ << "\n" \
             x << "\n\n"; \
      mrml::OutputWindowDisplayGenericOutputText(mrmlmsg.str().c_str());} \
}
#endif



//----------------------------------------------------------------------------
// Macros for simplifying the use of logging
//
#define mrmlLogMacro( x, y)  \
{         \
  if (this->GetLogger() ) \
    {  \
    this->GetLogger()->Write(::mrml::LoggerBase::x, y); \
    }  \
}


#define mrmlLogMacroStatic( obj, x, y)  \
{         \
  if (obj->GetLogger() ) \
    {  \
    obj->GetLogger()->Write(::mrml::LoggerBase::x, y); \
    }  \
}


//----------------------------------------------------------------------------
// Setup legacy code policy.

// Define mrmlLegacy macro to mark legacy methods where they are
// declared in their class.  Example usage:
//
//   // @deprecated Replaced by MyOtherMethod() as of mrml 2.0.
//   mrmlLegacy(void MyMethod());
#if defined(mrml_LEGACY_REMOVE)
  // Remove legacy methods completely.
# define mrmlLegacy(method)
#elif defined(mrml_LEGACY_SILENT) || defined(mrml_WRAPPING_CXX)
  // Provide legacy methods with no warnings.
# define mrmlLegacy(method) method
#else
  // Setup compile-time warnings for uses of deprecated methods if
  // possible on this compiler.
# if defined(__GNUC__) && !defined(__INTEL_COMPILER) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#  define mrmlLegacy(method) method __attribute__((deprecated))
# elif defined(_MSC_VER) && _MSC_VER >= 1300
#  define mrmlLegacy(method) __declspec(deprecated) method
# else
#  define mrmlLegacy(method) method
# endif
#endif

// Macros to create runtime deprecation warning messages in function
// bodies.  Example usage:
//
//   void mrmlMyClass::MyOldMethod()
//   {
//     mrmlLegacyBody(mrmlMyClass::MyOldMethod, 2.0);
//   }
//
//   void mrmlMyClass::MyMethod()
//   {
//     mrmlLegacyReplaceBody(mrmlMyClass::MyMethod, 2.0,
//                          mrmlMyClass::MyOtherMethod);
//   }
#if defined(mrml_LEGACY_REMOVE) || defined(mrml_LEGACY_SILENT)
# define mrmlLegacyBody(method, version)
# define mrmlLegacyReplaceBody(method, version, replace)
#else
# define mrmlLegacyBody(method, version) \
  mrmlWarningMacro(#method " was deprecated for mrml " #version " and will be removed in a future version.")
# define mrmlLegacyReplaceBody(method, version, replace) \
  mrmlWarningMacro(#method " was deprecated for mrml " #version " and will be removed in a future version.  Use " #replace " instead.")
#endif


#endif //end of mrmlMacro.h


