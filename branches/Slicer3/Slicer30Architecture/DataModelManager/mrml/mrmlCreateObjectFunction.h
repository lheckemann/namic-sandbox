/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: mrmlCreateObjectFunction.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:29:05 $
  Version:   $Revision: 1.22 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.mrml.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __mrmlCreateObjectFunction_h
#define __mrmlCreateObjectFunction_h

#include "mrmlObject.h"

namespace mrml
{

/** \class CreateObjectFunctionBase
 * \brief Define API for object creation callback functions.
 *
 * \ingroup SystemObjects
 */
class CreateObjectFunctionBase: public Object
{
public:
  /** Standard typedefs. */
  typedef CreateObjectFunctionBase  Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Create an object and return a pointer to it as an
   * mrml::LightObject. */
  virtual SmartPointer<LightObject> CreateObject() = 0;

protected:
  CreateObjectFunctionBase() {}
  ~CreateObjectFunctionBase() {}
  
private:
  CreateObjectFunctionBase(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented  
};


/** \class CreateObjectFunction
 * \brief CreateObjectFunction is used to create callback functions that
 * create ITK Objects for use with the mrml::ObjectFactory.
 * 
 * \ingroup ITKSystemObjects
 */
template <class T>
class CreateObjectFunction : public CreateObjectFunctionBase
{
public:
  /** Standard class typedefs. */
  typedef CreateObjectFunction  Self;
  typedef SmartPointer<Self>    Pointer;
    
  /** Methods from mrml:LightObject. */
  mrmlFactorylessNewMacro(Self);
  LightObject::Pointer CreateObject() { return T::New().GetPointer(); }
  
protected:
  CreateObjectFunction() {}
  ~CreateObjectFunction() {}
  
private:
  CreateObjectFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented  
};

} // end namespace mrml

#endif


