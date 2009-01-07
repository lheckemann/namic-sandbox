/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMultiZThreader.cxx,v $
  Language:  C++
  Date:      $Date: 2007/07/15 12:37:57 $
  Version:   $Revision: 1.39 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkMultiZThreader.h"
#include "itkObjectFactory.h"
#include "zthread/PoolExecutor.h"
#ifndef _WIN32
#include <unistd.h>
#endif
#ifdef _WIN32
#pragma warning ( disable : 4786 )
#include "itkWindows.h"
#include <process.h>
#endif

namespace itk {

void MultiZThreader::SetGlobalMaximumNumberOfThreads(int val)
{
  MultiThreader::SetGlobalMaximumNumberOfThreads ( val );
}
  
int MultiZThreader::GetGlobalMaximumNumberOfThreads()
{
  return MultiThreader::GetGlobalMaximumNumberOfThreads();
}

void MultiZThreader::SetGlobalDefaultNumberOfThreads(int val)
{
  MultiThreader::SetGlobalDefaultNumberOfThreads ( val );
}

int MultiZThreader::GetGlobalDefaultNumberOfThreads()
{
  return MultiThreader::GetGlobalDefaultNumberOfThreads();
}

// Constructor. Default all the methods to NULL. Since the
// ThreadInfoArray is static, the ThreadIDs can be initialized here
// and will not change.
MultiZThreader::MultiZThreader()
{
  m_SingleMethod = 0;
  m_SingleData = 0;
  m_NumberOfThreads = this->GetGlobalDefaultNumberOfThreads();
  m_NumberOfJobs = m_NumberOfThreads > 0 ? (unsigned int) m_NumberOfThreads : 1;
}

MultiZThreader::~MultiZThreader()
{
}

class ZThreadStruct : public ZThread::Runnable
{
public:
  MultiThreader::ThreadInfoStruct *threadInfoStruct;
  MultiZThreader* Threader;
  void *UserData;
  ThreadFunctionType ThreadFunction;
  ZThreadStruct() { threadInfoStruct = new MultiThreader::ThreadInfoStruct(); };
  ~ZThreadStruct() { delete this->threadInfoStruct; };
  virtual void run() {
    // execute the user specified threader callback, catching any exceptions
    try
      {
      /*
      std::cout << "\t\tStarting run method on thread " << threadInfoStruct->ThreadID << std::endl;
      std::cout << "\t\tMethod: " << ThreadFunction << std::endl;
      std::cout << "\t\tUserdata: " << UserData << std::endl;
      */
      if ( ThreadFunction ) 
        {
        (*ThreadFunction)(threadInfoStruct);
        threadInfoStruct->ThreadExitCode
          = MultiThreader::ThreadInfoStruct::SUCCESS;
        }
      else
        {
        // std::cout << "\t\tNo ThreadFunction specified" << std::endl;
        threadInfoStruct->ThreadExitCode = MultiThreader::ThreadInfoStruct::UNKNOWN;
        }
      
      }
    catch (ProcessAborted &)
      {
      threadInfoStruct->ThreadExitCode
        = MultiThreader::ThreadInfoStruct::ITK_PROCESS_ABORTED_EXCEPTION;
      }
    catch (ExceptionObject &)
      {
      threadInfoStruct->ThreadExitCode
        = MultiThreader::ThreadInfoStruct::ITK_EXCEPTION;
      }
    catch (std::exception &)
      {
      threadInfoStruct->ThreadExitCode
        = MultiThreader::ThreadInfoStruct::STD_EXCEPTION;
      }
    catch (...)
      {
      threadInfoStruct->ThreadExitCode
        = MultiThreader::ThreadInfoStruct::UNKNOWN;
      }
    if ( threadInfoStruct->ThreadExitCode != MultiThreader::ThreadInfoStruct::SUCCESS )
      {
      Threader->SetExceptionOccurred();
      }
  };
};

// Set the user defined method that will be run on NumberOfThreads threads
// when SingleMethodExecute is called.
void MultiZThreader::SetSingleMethod( ThreadFunctionType f, void *data )
{
  m_SingleMethod = f;
  m_SingleData   = data;
}

// Execute the method set as the SingleMethod on NumberOfThreads threads.
void MultiZThreader::SingleMethodExecute()
{
  m_ExceptionOccurred = false;
  ZThread::PoolExecutor executor(this->GetNumberOfThreads());
  // std::cout << "Starting SingleMethodExecute" << std::endl;
  try
    {
    for ( unsigned int i = 0; i < this->GetNumberOfJobs(); i++ )
      {
      // std::cout << "\tStarting job " << i << std::endl;
      ZThreadStruct* s = new ZThreadStruct();
      s->threadInfoStruct->ThreadID = i;
      s->threadInfoStruct->NumberOfThreads = this->GetNumberOfJobs();
      s->threadInfoStruct->UserData = (void*) m_SingleData;
      s->ThreadFunction = m_SingleMethod;
      executor.execute ( s );
      }
    // std::cout << "Waiting for all jobs to finish" << std::endl;
    // Let it all finish
    executor.wait();
    // std::cout << "All jobs finished" << std::endl;
    }
  catch ( ZThread::Synchronization_Exception &e )
    {
    itkGenericExceptionMacro ( << "Error adding runnable to executor: " << e.what() );
    }
  if (m_ExceptionOccurred)
    {
    itkExceptionMacro("Exception occurred during SingleMethodExecute");
    }
}

void MultiZThreader::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent); 
  os << indent << "Thread Count: " << m_NumberOfThreads << "\n";
  os << indent << "Job Count: " << m_NumberOfThreads << "\n";
}



} // end namespace itk
