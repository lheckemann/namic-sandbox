/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: mrmlFastMutexLock.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:29:07 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.mrml.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __mrmlFastMutexLock_h
#define __mrmlFastMutexLock_h

#include "mrmlObject.h"
#include "mrmlSimpleFastMutexLock.h"
#include "mrmlObjectFactory.h"

namespace mrml
{

/** \class FastMutexLock
 * \brief Critical section locking class.
 * 
 * FastMutexLock allows the locking of variables which are accessed 
 * through different threads.  This header file also defines 
 * SimpleFastMutexLock which is not a subclass of Object.
 * The API is identical to that of MutexLock, and the behavior is
 * identical as well, except on Windows 9x/NT platforms. The only difference
 * on these platforms is that MutexLock is more flexible, in that
 * it works across processes as well as across threads, but also costs
 * more, in that it evokes a 600-cycle x86 ring transition. The 
 * FastMutexLock provides a higher-performance equivalent (on 
 * Windows) but won't work across processes. Since it is unclear how,
 * in mrml, an object at the mrml level can be shared across processes
 * in the first place, one should use FastMutexLock unless one has
 * a very good reason to use MutexLock. If higher-performance equivalents
 * for non-Windows platforms (Irix, SunOS, etc) are discovered, they
 * should replace the implementations in this class
 *
 * \ingroup OSSystemObjects
 */
class MRMLCommon_EXPORT FastMutexLock : public Object
{
public:
  /** Run-time type information. */
  mrmlTypeMacro(FastMutexLock,Object);

  /** Method for creation. */
  mrmlNewMacro(Self);

  /** Lock the mrmlFastMutexLock. */
  void Lock();

  /** Unlock the FastMutexLock. */
  void Unlock();

protected:
  FastMutexLock() {}
  ~FastMutexLock() {}
  
  SimpleFastMutexLock   m_SimpleFastMutexLock;
  void PrintSelf(std::ostream& os, Indent indent) const;
  
private:
  FastMutexLock(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};


inline void FastMutexLock::Lock( void )
{
  m_SimpleFastMutexLock.Lock();
}

inline void FastMutexLock::Unlock( void )
{
  m_SimpleFastMutexLock.Unlock();
}


}//end mrml namespace
#endif

