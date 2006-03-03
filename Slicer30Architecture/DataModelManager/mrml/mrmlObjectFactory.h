/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: mrmlObjectFactory.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:29:18 $
  Version:   $Revision: 1.17 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.mrml.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __mrmlObjectFactory_h
#define __mrmlObjectFactory_h

#include "mrmlObjectFactoryBase.h"

namespace mrml
{

/** \class ObjectFactory
 * \brief Create instances of a class.
 *
 * ObjectFactory is a helper class used to created instances of a
 * class. Object factories are used for instantiation because they allow
 * run-time replacement of a class with a user-supplied version. For
 * example, if you wished to replace an algorithm with your own custom
 * version, or with a hardware-accelerated version, ObjectFactory
 * can be used to do this.
 *
 * This implementation of the object factory is templated and uses RTTI
 * (Run-Time Type Information) to create the name of the class it is to
 * instantiate. (The name may include template type parameters, depending
 * on the class definition.)
 *
 * \ingroup ITKSystemObjects
 */

template <class T>
class ObjectFactory : public ObjectFactoryBase
{
public:
  static typename T::Pointer Create()
  {
    LightObject::Pointer ret = ObjectFactory::CreateInstance(typeid(T).name());
    return dynamic_cast<T*>(ret.GetPointer());
  }
};

} // end namespace mrml

#endif

        

