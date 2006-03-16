/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: mrmlLightObject.h,v $
  Date:      $Date: 2006/01/06 17:56:51 $
  Version:   $Revision: 1.6 $

=========================================================================auto=*/
#ifndef __mrmlLightObject_h
#define __mrmlLightObject_h

#include "mrmlSmartPointer.h"
#include "mrmlTimeStamp.h"
#include "mrmlIndent.h"
#include "mrmlSimpleFastMutexLock.h"
#include "mrmlMacro.h"

#include <iostream>
#include <typeinfo>

namespace mrml
{

/** \class LightObject
 * \brief Light weight base class for most mrml classes.
 *
 * LightObject is the highest level base class for most mrml objects. It
 * implements reference counting and the API for object printing.
 * It can be used as a lightweight base class in preference to Object.
 * (LightObject does not support callbacks or modified time as Object
 * does.) All MRML objects should be a subclass of LightObject or Object
 * with few exceptions (due to performance concerns).
 *
 * \sa TODO
 */
class MRMLCommon_EXPORT LightObject
{
public:
  /** Standard clas typedefs. */
  typedef LightObject         Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  static Pointer New();

  /** Create an object from an instance, potentially deferring to a
   * factory.  This method allows you to create an instance of an
   * object that is exactly the same type as the referring object.
   * This is useful in cases where an object has been cast back to a
   * base class. */
  virtual Pointer CreateAnother() const;

  /** Return the name of this class as a string. Used by the object factory
   * (implemented in New()) to instantiate objects of a named type. Also
   * used for debugging and other output information.  */
  virtual const char *GetNameOfClass() const
    {return "LightObject";}

#ifdef _WIN32
  /** Used to avoid dll boundary problems.  */
  void* operator new(size_t);
  void* operator new[](size_t);
  void operator delete(void*);
  void operator delete[](void*, size_t);
#endif

  /** Cause the object to print itself out. */
  void Print(std::ostream& os = std::cout, Indent indent=0) const;

  /** Increase the reference count (mark as used by another object).  */
  virtual void Register() const;

  /** Decrease the reference count (release by another object).  */
  virtual void UnRegister() const;

  /** Gets the reference count on this object. */
  virtual int GetReferenceCount() const
    {return m_ReferenceCount;}

  /** Sets the reference count on this object. This is a dangerous
   * method, use it with care. */
  virtual void SetReferenceCount(int);

protected:
  LightObject():m_ReferenceCount(1) {}
  virtual ~LightObject();

  /** Delete an mrml object.  This method should always be used to delete an
   * object when the new operator was used to create it. Using the C
   *  delete method will not work with reference counting.  */
  void Delete();

  /** Methods invoked by Print() to print information about the object
   * including superclasses. Typically not called by the user (use Print()
   * instead) but used in the hierarchical print process to combine the
   * output of several classes.  */
  virtual void PrintSelf(std::ostream& os, Indent indent) const;
  virtual void PrintHeader(std::ostream& os, Indent indent) const;
  virtual void PrintTrailer(std::ostream& os, Indent indent) const;

  /** Number of uses of this object by other objects. */
  mutable int m_ReferenceCount;

  /** Mutex lock to protect modification to the reference count */
  mutable SimpleFastMutexLock m_ReferenceCountLock;

private:
  LightObject(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace mrml

#endif
