/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSimpleFastMutexLock.cxx,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igtlSimpleFastMutexLock.h"

namespace igtl
{

// Construct a new SimpleMutexLock
SimpleFastMutexLock::SimpleFastMutexLock()
{
#ifdef OpenIGTLink_USE_SPROC
  init_lock( &m_FastMutexLock );
#endif

#if defined(_WIN32) && !defined(OpenIGTLink_USE_PTHREADS)
  //this->MutexLock = CreateMutex( NULL, FALSE, NULL );
  InitializeCriticalSection(&m_FastMutexLock);
#endif

#ifdef OpenIGTLink_USE_PTHREADS
#ifdef OpenIGTLink_HP_PTHREADS
  pthread_mutex_init(&(m_FastMutexLock), pthread_mutexattr_default);
#else
  pthread_mutex_init(&(m_FastMutexLock), NULL);
#endif
#endif

}

// Destruct the SimpleMutexVariable
SimpleFastMutexLock::~SimpleFastMutexLock()
{
#if defined(_WIN32) && !defined(OpenIGTLink_USE_PTHREADS)
  //CloseHandle(this->MutexLock);
  DeleteCriticalSection(&m_FastMutexLock);
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  pthread_mutex_destroy( &m_FastMutexLock);
#endif
}

// Lock the FastMutexLock
void SimpleFastMutexLock::Lock() const
{
#ifdef OpenIGTLink_USE_SPROC
  spin_lock( &m_FastMutexLock );
#endif

#if defined(_WIN32) && !defined(OpenIGTLink_USE_PTHREADS)
  //WaitForSingleObject( this->MutexLock, INFINITE );
  EnterCriticalSection(&m_FastMutexLock);
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  pthread_mutex_lock( &m_FastMutexLock);
#endif
}

// Unlock the FastMutexLock
void SimpleFastMutexLock::Unlock() const
{
#ifdef OpenIGTLink_USE_SPROC
  release_lock( &m_FastMutexLock );
#endif

#if defined(_WIN32) && !defined(OpenIGTLink_USE_PTHREADS)
  //ReleaseMutex( this->MutexLock );
  LeaveCriticalSection(&m_FastMutexLock);
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  pthread_mutex_unlock( &m_FastMutexLock);
#endif
}

}//end namespace igtl
