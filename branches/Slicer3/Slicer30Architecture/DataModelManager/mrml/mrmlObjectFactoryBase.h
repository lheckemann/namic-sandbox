/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: mrmlObjectFactoryBase.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:29:19 $
  Version:   $Revision: 1.35 $

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __mrmlObjectFactoryBase_h
#define __mrmlObjectFactoryBase_h

#include "mrmlObject.h"
#include "mrmlCreateObjectFunction.h"
#include <mrmlsys/DynamicLoader.hxx>

#include <list>
#include <vector>

namespace mrml
{
/** \class ObjectFactoryBase
 * \brief Create instances of classes using an object factory.
 *
 * ObjectFactoryBase is used to create mrml objects. The base class
 * ObjectFactoryBase contains a static method CreateInstance() that is
 * used to create mrml objects from the list of registerd ObjectFactoryBase
 * sub-classes.  The first time CreateInstance() is called, all dll's or
 * shared libraries in the environment variable MRML_AUTOLOAD_PATH are loaded
 * into the current process.  The C function mrmlLoad is called on each dll.
 * mrmlLoad should return an instance of the factory sub-class implemented in
 * the shared library. MRML_AUTOLOAD_PATH is an environment variable
 * containing a colon separated (semi-colon on win32) list of paths.
 *
 * This can be use to overide the creation of any object in MRML.
 *
 * \ingroup SystemObjects
 */

class OverRideMap;

class MRMLCommon_EXPORT ObjectFactoryBase : public Object
{
public:  
  /** Run-time type information (and related methods). */
  mrmlTypeMacro(ObjectFactoryBase, Object);

  /** Create and return an instance of the named mrml object.
   * Each loaded ObjectFactoryBase will be asked in the order
   * the factory was in the MRML_AUTOLOAD_PATH.  After the
   * first factory returns the object no other factories are asked. */
  static LightObject::Pointer CreateInstance(const char* mrmlclassname);

  /** Create and return all possible instances of the named mrml object.
   * Each loaded ObjectFactoryBase will be asked in the order
   * the factory was in the MRML_AUTOLOAD_PATH.  All created objects
   * will be returned in the list. */
  static std::list<LightObject::Pointer>
  CreateAllInstance(const char* mrmlclassname);
  
  /** Re-check the MRML_AUTOLOAD_PATH for new factory libraries.
   * This calls UnRegisterAll before re-loading. */
  static void ReHash(); 

  /** Register a factory so it can be used to create mrml objects. */
  static void RegisterFactory(ObjectFactoryBase* );

  /** Remove a factory from the list of registered factories. */
  static void UnRegisterFactory(ObjectFactoryBase*);

  /** Unregister all factories. */
  static void UnRegisterAllFactories();
  
  /** Return the list of all registered factories.  This is NOT a copy,
   * do not remove items from this list! */
  static std::list<ObjectFactoryBase*> GetRegisteredFactories();

  /** All sub-classes of ObjectFactoryBase should must return the version of 
   * MRML they were built with.  This should be implemented with the macro
   * MRML_SOURCE_VERSION and NOT a call to Version::GetMRMLSourceVersion.
   * As the version needs to be compiled into the file as a string constant.
   * This is critical to determine possible incompatible dynamic factory loads. */
  virtual const char* GetMRMLSourceVersion(void) const = 0;

  /** Return a descriptive string describing the factory. */
  virtual const char* GetDescription(void) const = 0;

  /** Return a list of classes that this factory overrides. */
  virtual std::list<std::string> GetClassOverrideNames();

  /** Return a list of the names of classes that override classes. */
  virtual std::list<std::string> GetClassOverrideWithNames();

  /** Return a list of descriptions for class overrides. */
  virtual std::list<std::string> GetClassOverrideDescriptions();

  /** Return a list of enable flags. */
  virtual std::list<bool> GetEnableFlags();

  /** Set the Enable flag for the specific override of className. */
  virtual void SetEnableFlag(bool flag,
           const char* className,
           const char* subclassName);

  /** Get the Enable flag for the specific override of className. */
  virtual bool GetEnableFlag(const char* className,
                             const char* subclassName);

  /** Set all enable flags for the given class to 0.  This will
   * mean that the factory will stop producing class with the given
   * name. */
  virtual void Disable(const char* className);
  
  /** This returns the path to a dynamically loaded factory. */
  const char* GetLibraryPath();

  /** \class OverrideInformation
   * \brief Internal implementation class for ObjectFactorBase. */
  struct OverrideInformation
  {
    std::string m_Description;
    std::string m_OverrideWithName;
    bool m_EnabledFlag;
    CreateObjectFunctionBase::Pointer m_CreateObject;
  };

protected:
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  /** Register object creation information with the factory. */
  void RegisterOverride(const char* classOverride,
      const char* overrideClassName,
      const char* description,
      bool enableFlag,
      CreateObjectFunctionBase* createFunction);
    
  /** This method is provioded by sub-classes of ObjectFactoryBase.
   * It should create the named mrml object or return 0 if that object
   * is not supported by the factory implementation. */
  virtual LightObject::Pointer CreateObject(const char* mrmlclassname );
  
  ObjectFactoryBase();
  virtual ~ObjectFactoryBase();

private:
  OverRideMap* m_OverrideMap;

  ObjectFactoryBase(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Initialize the static members of ObjectFactoryBase.   RegisterDefaults
   * is called here. */
  static void Initialize();

  /** Register default factories which are not loaded at run time. */
  static void RegisterDefaults();

  /** Load dynamic factories from the MRML_AUTOLOAD_PATH */
  static void LoadDynamicFactories();

  /** Load all dynamic libraries in the given path */
  static void LoadLibrariesInPath( const char*);
  
  /** list of registered factories */
  static std::list<ObjectFactoryBase*>* m_RegisteredFactories; 
  
  /** Member variables for a factory set by the base class
   * at load or register time */
  mrmlsys::LibHandle m_LibraryHandle;
  unsigned long m_LibraryDate;
  std::string m_LibraryPath;
  };

} // end namespace mrml

#endif