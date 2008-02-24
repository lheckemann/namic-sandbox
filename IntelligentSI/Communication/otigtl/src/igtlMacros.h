/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: itkImage.h,v $
  Language:  C++
  Date:      $Date: 2008/01/13 19:48:38 $
  Version:   $Revision: 1.142 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igtlMacros_h
#define __igtlMacros_h

/** \file igtlMacros.h defines standard system-wide macros, constants, 
 * and other  common parameters in the OpenIGTLink Library. 
 * It includes the Get, Set and Log macros.
 */

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
// warning C4503: 'insert' : decorated name length exceeded, 
// name was truncated
#pragma warning ( disable : 4503 )
#endif

namespace igtl
{


/** Set built-in type.  Creates member Set"name"() (e.g., SetTimeStep(time)); */
#define  igtlSetMacro(name,type) \
virtual void Set##name (const type & _arg) \
{ \
  if (this->m_##name != _arg) \
    { \
    this->m_##name = _arg; \
    } \
} 


/** Get built-in type.  Creates member Get"name"() (e.g., GetTimeStep(time));
 */
#define igtlGetMacro(name,type) \
virtual const type & Get##name () const \
{ \
  return this->m_##name; \
}


/** New Macro creates a new object of a class that is using SmartPointers. This
 * macro differs from the one in ITK in that it DOES NOT uses factories.
 * Factories add run-time uncertainty that is undesirable for OpenIGTLink. */
#define igtlNewMacro(x) \
static Pointer New(void) \
{ \
  Pointer smartPtr; \
  x *rawPtr = new x; \
  smartPtr = rawPtr; \
  /*rawPtr->UnRegister();*/                     \
  return smartPtr; \
} 


/** Type Macro defines the GetNameOfClass() method for every class where it is
 * invoked. */
#define igtlTypeMacro(thisClass,superclass) \
    virtual const char *GetNameOfClass() const {return #thisClass;} 


/** Set character string.  Creates member Set"name"()
 * (e.g., SetFilename(char *)). The macro assumes that
 * the class member (name) is declared a type std::string. */
#define igtlSetStringMacro(name) \
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

}
#endif // __igtl_Macros_h_
