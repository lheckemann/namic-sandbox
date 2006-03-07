/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: mrmlSimpleFastMutexLock.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:29:26 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See MRMLCopyright.txt or http://www.mrml.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __mrmlSimpleFastMutexLock_h
#define __mrmlSimpleFastMutexLock_h

#include "mrmlMacro.h"

#ifdef MRML_USE_SPROC
#include <abi_mutex.h>
#endif

#ifdef MRML_USE_PTHREADS
#include <pthread.h>
#endif
 
#if defined(_WIN32) && !defined(MRML_USE_PTHREADS)
#include <windows.h>
#endif

namespace mrml
{

#ifdef MRML_USE_SPROC
#include <abi_mutex.h>
typedef abilock_t FastMutexType;
#endif

#ifdef MRML_USE_PTHREADS
#include <pthread.h>
typedef pthread_mutex_t FastMutexType;
#endif
 
#if defined(_WIN32) && !defined(MRML_USE_PTHREADS)
#include <winbase.h>
typedef CRITICAL_SECTION FastMutexType;
#endif

#ifndef MRML_USE_SPROC
#ifndef MRML_USE_PTHREADS
#ifndef _WIN32
typedef int FastMutexType;
#endif
#endif
#endif

/** \class SimpleFastMutexLock
 * \brief Critical section locking class that can be allocated on the stack.
 * 
 * SimpleFastMutexLock is used by FastMutexLock to perform mutex locking.
 * SimpleFastMutexLock is not a subclass of Object and is designed to be
 * allocated on the stack.
 *
 * \ingroup OSSystemObjects
 */

// Critical Section object that is not a mrmlObject.
class mrmlCommon_EXPORT SimpleFastMutexLock
{
public:
  /** Standard class typedefs.  */
  typedef SimpleFastMutexLock       Self;
  
  /** Constructor and destructor left public purposely because of stack allocation. */
  SimpleFastMutexLock();
  ~SimpleFastMutexLock();
  
  /** Lock access. */
  void Lock() const;

  /** Unlock access. */
  void Unlock() const;

protected:
  mutable FastMutexType   m_FastMutexLock;
};

}//end mrml namespace
#endif

