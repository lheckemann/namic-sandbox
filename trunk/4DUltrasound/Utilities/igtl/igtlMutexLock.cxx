/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMutexLock.cxx,v $
  Language:  C++
  Date:      $Date: 2003-09-10 14:29:17 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igtlMutexLock.h"

namespace igtl
{


// New for the SimpleMutex
SimpleMutexLock *SimpleMutexLock::New()
{
  return new SimpleMutexLock;
}

// Construct a new MutexLock 
SimpleMutexLock::SimpleMutexLock()
{
#ifdef OpenIGTLink_USE_SPROC
  init_lock( &m_MutexLock );
#endif

#ifdef OpenIGTLink_USE_WIN32_THREADS
  m_MutexLock = CreateMutex( NULL, FALSE, NULL ); 
#endif

#ifdef OpenIGTLink_USE_PTHREADS
#ifdef OpenIGTLink_HP_PTHREADS
  pthread_mutex_init(&m_MutexLock, pthread_mutexattr_default);
#else
  pthread_mutex_init(&m_MutexLock, NULL);
#endif
#endif

}

// Destruct the MutexVariable
SimpleMutexLock::~SimpleMutexLock()
{
#ifdef OpenIGTLink_USE_WIN32_THREADS
  CloseHandle(m_MutexLock);
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  pthread_mutex_destroy( &m_MutexLock);
#endif
}

// Lock the MutexLock
void SimpleMutexLock::Lock()
{
#ifdef OpenIGTLink_USE_SPROC
  spin_lock( &m_MutexLock );
#endif

#ifdef OpenIGTLink_USE_WIN32_THREADS
  WaitForSingleObject( m_MutexLock, INFINITE );
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  pthread_mutex_lock( &m_MutexLock);
#endif
}

// Unlock the MutexLock
void SimpleMutexLock::Unlock()
{
#ifdef OpenIGTLink_USE_SPROC
  release_lock( &m_MutexLock );
#endif

#ifdef OpenIGTLink_USE_WIN32_THREADS
  ReleaseMutex( m_MutexLock );
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  pthread_mutex_unlock( &m_MutexLock);
#endif
}

void MutexLock::PrintSelf(std::ostream& os) const
{
  Superclass::PrintSelf(os);
}

}//end namespace igtl
