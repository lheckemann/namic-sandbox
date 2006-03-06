/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: mrmlVersion.h,v $
  Language:  C++
  Date:      $Date: 2006/03/06 01:10:06 $
  Version:   $Revision: 1.1928 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See MRMLCopyright.txt or http://www.mrml.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __mrmlVersion_h
#define __mrmlVersion_h

#include "mrmlObject.h"
#include "mrmlObjectFactory.h"

#define MRML_VERSION_TO_STRING(x) MRML_VERSION_TO_STRING0(x)
#define MRML_VERSION_TO_STRING0(x) #x
#define MRML_VERSION MRML_VERSION_TO_STRING(MRML_VERSION_MAJOR) \
                    MRML_VERSION_TO_STRING(MRML_VERSION_MINOR) \
                    MRML_VERSION_TO_STRING(MRML_VERSION_PATCH)
#define MRML_SOURCE_VERSION "mrml version " MRML_VERSION ", mrml source $Revision: 1.1928 $, $Date: 2006/03/06 01:10:06 $ (GMT)"

namespace mrml
{
/** \class Version
 * \brief Track the current version of the software.
 *
 * Holds methods for defining/determining the current mrml version
 * (major, minor, build).
 *
 * This file will change frequently to update the MRMLSourceVersion which
 * timestamps a particular source release.
 *
 * \ingroup MRMLSystemObjects
 */

class mrmlCommon_EXPORT Version : public Object 
{
public:
  /** Standard class typedefs. */
  typedef Version             Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Method for creation through the object factory. */
  mrmlNewMacro(Self);  

  /** Standard part of every mrml Object. */
  mrmlTypeMacro(Version,Object);

  /** Return the version of mrml this object is a part of.
   * A variety of methods are included. GetMRMLSourceVersion returns a string
   * with an identifier which timestamps a particular source tree.  */
  static const char *GetMRMLVersion() { return MRML_VERSION; };
  static int GetMRMLMajorVersion() { return MRML_VERSION_MAJOR; };
  static int GetMRMLMinorVersion() { return MRML_VERSION_MINOR; };
  static int GetMRMLBuildVersion() { return MRML_VERSION_PATCH; };
  static const char *GetMRMLSourceVersion() { return MRML_SOURCE_VERSION; };
    
protected:
  Version();
  ~Version();

private:
  Version(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace mrml

#endif 
