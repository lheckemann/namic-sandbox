/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkYAFFImageIOFactory.h,v $
  Language:  C++
  Date:      $Date: 2007/03/22 14:28:51 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkYAFFImageIOFactory_h
#define __itkYAFFImageIOFactory_h

#include "itkObjectFactoryBase.h"
#include "itkImageIOBase.h"

namespace itk
{
/** \class YAFFImageIOFactory
 * \brief YAFF : Yet Another File Format: is a fake file format introduced
                 with the sole purpose of testing the streaming capabilities
                 of ITK.

         
   \warning    DO NOT USE THIS FILE FORMAT FOR ANY SERIOUS PURPOSE !.


   Consider it only a place holder for very basic testing.
 */
class ITK_EXPORT YAFFImageIOFactory : public ObjectFactoryBase
{
public:  
  /** Standard class typedefs. */
  typedef YAFFImageIOFactory       Self;
  typedef ObjectFactoryBase        Superclass;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  
  /** Class methods used to interface with the registered factories. */
  virtual const char* GetITKSourceVersion() const;
  virtual const char* GetDescription() const;
  
  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);
  static YAFFImageIOFactory* FactoryNew() { return new YAFFImageIOFactory;}

  /** Run-time type information (and related methods). */
  itkTypeMacro(YAFFImageIOFactory, ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory()
    {
    YAFFImageIOFactory::Pointer metaFactory = YAFFImageIOFactory::New();
    ObjectFactoryBase::RegisterFactory(metaFactory);
    }

protected:
  YAFFImageIOFactory();
  ~YAFFImageIOFactory();

private:
  YAFFImageIOFactory(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};
 
} // end namespace itk

#endif
