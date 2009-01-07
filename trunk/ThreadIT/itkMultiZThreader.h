/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMultiZThreader.h,v $
  Language:  C++
  Date:      $Date: 2005/08/22 20:23:36 $
  Version:   $Revision: 1.28 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMultiZThreader_h
#define __itkMultiZThreader_h

#include "itkObject.h"
#include "itkSimpleFastMutexLock.h"
#include "itkMultiThreader.h"
#include "zthread/Runnable.h"

namespace itk
{
/** \class MultiZThreader
 * \brief A class for performing multithreaded execution
 *
 * Multithreader is a class that provides support for multithreaded
 * execution using sproc() on an SGI, or pthread_create on any platform
 * supporting POSIX threads.  This class can be used to execute a single
 * method on multiple threads, or to specify a method per thread.
 *
 * \ingroup OSSystemObejcts
 *
 * \par Note
 * If ITK_USE_SPROC is defined, then sproc() will be used to create
 * multiple threads on an SGI. If ITK_USE_PTHREADS is defined, then
 * pthread_create() will be used to create multiple threads (on
 * a sun, for example). */

  
class ITKCommon_EXPORT MultiZThreader : public Object 
{
public:
  /** Standard class typedefs. */
  typedef MultiZThreader         Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(MultiZThreader, Object);

  /** Get/Set the number of threads to create. It will be clamped to the range
   * 1 - ITK_MAX_THREADS, so the caller of this method should check that the
   * requested number of threads was accepted. */
  itkSetClampMacro( NumberOfThreads, int, 1, ITK_MAX_THREADS );
  itkGetMacro( NumberOfThreads, int );
    
  /** Set/Get the maximum number of threads to use when multithreading.
   * This limits and overrides any other settings for multithreading.
   * A value of zero indicates no limit. */
  static void SetGlobalMaximumNumberOfThreads(int val);
  static int  GetGlobalMaximumNumberOfThreads();
  
  /** Set/Get the value which is used to initialize the NumberOfThreads
   * in the constructor.  Initially this default is set to the number of 
   * processors or 8 (which ever is less). 
   * NB: these methods delegate to itkMultiThreader */
  static void SetGlobalDefaultNumberOfThreads(int val);
  static int  GetGlobalDefaultNumberOfThreads();
  

  /** Set/Get the number of jobs to divide this run into.  Default is 
    * the GlobalDefaultNumberOfThreads */
  void SetNumberOfJobs ( unsigned int j ) { m_NumberOfJobs = j; };
  unsigned int GetNumberOfJobs () { return m_NumberOfJobs; };

  /** Execute the SingleMethod (as define by SetSingleMethod) using
   * m_NumberOfThreads threads. */
  void SingleMethodExecute();

  /** Set the SingleMethod to f() and the UserData field of the
   * ThreadInfoStruct that is passed to it will be data.
   * This method (and all the methods passed to SetMultipleMethod)
   * must be of type itkThreadFunctionType and must take a single argument of
   * type void *. */
  void SetSingleMethod(ThreadFunctionType, void *data );

  /** Exception occurred.  This may be called by multiple threads.
    */
  void SetExceptionOccurred() {
    m_Lock.Lock();
    this->m_ExceptionOccurred = true;
    m_Lock.Unlock();
  }

protected:
  MultiZThreader();
  ~MultiZThreader();
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  MultiZThreader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Mutex */
  SimpleFastMutexLock m_Lock;

  /** The number of threads to use. */
  int                        m_NumberOfThreads;

  /** The number jobs to use */
  int                        m_NumberOfJobs;

  /** Did we have an exception some where along the way? */
  bool                       m_ExceptionOccurred;

  /** The methods to invoke. */
  ThreadFunctionType         m_SingleMethod;

  /** Internal storage of the data. */
  void                       *m_SingleData;

  /** Friends of Multithreader.
   * ProcessObject is a friend so that it can call PrintSelf() on its
   * Multithreader. */
  friend class ProcessObject;
}; 
 
}  // end namespace itk
#endif
