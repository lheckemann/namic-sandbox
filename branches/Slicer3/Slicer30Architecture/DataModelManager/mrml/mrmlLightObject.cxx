/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicerBase.h,v $
  Date:      $Date: 2006/01/06 17:56:51 $
  Version:   $Revision: 1.6 $

=========================================================================auto=*/
#include "mrmlLightObject.h"
//#include "mrmlObjectFactory.h"
#include "mrmlFastMutexLock.h"

#include <list>
#include <memory>

#if __GNUC__ > 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ > 0 )
#  include <cxxabi.h>
#endif

namespace mrml
{

LightObject::Pointer LightObject::New()
{
  Pointer smartPtr;
  LightObject *rawPtr = ::mrml::ObjectFactory<LightObject>::Create();
  if(rawPtr == NULL)
    {
    rawPtr = new LightObject;
    }
  smartPtr = rawPtr;
  rawPtr->UnRegister();
  return smartPtr;
}

LightObject::Pointer LightObject::CreateAnother() const
{
  return LightObject::New();
}

/**
 * Delete a mrml object. This method should always be used to delete an object
 * when the new operator was used to create it. Using the C++ delete method
 * will not work with reference counting.
 */
void LightObject::Delete()
{
  this->UnRegister();
}


/**
 * Avoid DLL boundary problems.
 */
#ifdef _WIN32
void* LightObject::operator new(size_t n)
{
  return new char[n];
}

void* LightObject::operator new[](size_t n)
{
  return new char[n];
}

void LightObject::operator delete(void* m)
{
  delete [] (char*)m;
}

void LightObject::operator delete[](void* m, size_t)
{
  delete [] (char*)m;
}
#endif


/**
 * This function will be common to all mrml objects.  It just calls the
 * header/self/trailer virtual print methods, which can be overriden by
 * subclasses (any mrml object).
 */
void LightObject::Print(std::ostream& os, Indent indent) const
{
  this->PrintHeader(os, indent);
  this->PrintSelf(os, indent.GetNextIndent());
  this->PrintTrailer(os, indent);
}


/**
 * Increase the reference count (mark as used by another object).
 */
void LightObject::Register() const
{
  m_ReferenceCountLock.Lock();
  m_ReferenceCount++;
  m_ReferenceCountLock.Unlock();
}


/**
 * Decrease the reference count (release by another object).
 */
void LightObject::UnRegister() const
{
  m_ReferenceCountLock.Lock();
  m_ReferenceCount--;
  m_ReferenceCountLock.Unlock();

  // ReferenceCount in now unlocked.  We may have a race condition
  // to delete the object.
  if ( m_ReferenceCount <= 0)
    {
    delete this;
    }
}


/**
 * Sets the reference count (use with care)
 */
void LightObject::SetReferenceCount(int ref)
{
  m_ReferenceCountLock.Lock();
  m_ReferenceCount = ref;
  m_ReferenceCountLock.Unlock();

  if ( m_ReferenceCount <= 0)
    {
    delete this;
    }
}

LightObject::~LightObject()
{
  /**
   * warn user if reference counting is on and the object is being referenced
   * by another object
   */
  if ( m_ReferenceCount > 0)
    {
    mrmlExceptionMacro(<<
      "Trying to delete object with non-zero reference count.");
    }
}

std::string UnmangledName(const char* mangledName)
{
  std::string result;
// Better name demangling for gcc
#if __GNUC__ > 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ > 0 )
  int status;
  char * unmangled = abi::__cxa_demangle(mangledName,0,0,&status);

  // Handle failure
  if(status == 0)
    {
    result = unmangled;
    free(unmangled);
    }
  else
    {
    result = mangledName;
    }
#else
  // handle other compilers?
  result = mangledName;
#endif

  return result;
}

/**
 * Chaining method to print an object's instance variables, as well as
 * its superclasses.
 */
void LightObject::PrintSelf(std::ostream& os, Indent indent) const
{
  os << indent << "RTTI typeinfo:   "
    << UnmangledName(typeid( *this ).name()) << std::endl;
  os << indent << "Reference Count: " << m_ReferenceCount << std::endl;
}


/**
 * Define a default print header for all objects.
 */
void LightObject::PrintHeader(std::ostream& os, Indent indent) const
{
  os << indent << this->GetNameOfClass() << " (" << this << ")\n";
}


/**
 * Define a default print trailer for all objects.
 */
void LightObject::PrintTrailer(std::ostream& /*os*/, Indent /*indent*/) const
{
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<(std::ostream& os, const LightObject& o)
{
  o.Print(os);
  return os;
}


} // end namespace mrml
