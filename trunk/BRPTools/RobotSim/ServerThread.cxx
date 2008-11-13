/*=========================================================================

  Program:   Simple Robot Simulator / Server Thread class
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "ServerThread.h"

#include "igtlMultiThreader.h"
#include "igtlMutexLock.h"
#include "igtlObjectFactory.h"

namespace igtl {

ServerThread::ServerThread() : Object()
{
  this->m_Thread = new MultiThreader();



}




}




