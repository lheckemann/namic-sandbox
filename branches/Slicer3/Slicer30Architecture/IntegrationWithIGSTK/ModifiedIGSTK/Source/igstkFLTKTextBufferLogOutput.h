/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkFLTKTextBufferLogOutput.h,v $
  Language:  C++
  Date:      $Date: 2005/12/13 01:33:20 $
  Version:   $Revision: 1.4 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_FLTKTextBufferLogOutput_h_
#define __igstk_FLTKTextBufferLogOutput_h_

#include <iostream>
#include <string>

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkSimpleFastMutexLock.h"
#include "itkLogOutput.h"

#include <FL/Fl_Text_Buffer.H>

#include "igstkMacros.h"
#include "igstkStateMachine.h"


namespace igstk
{

/** \class FLTKTextBufferLogOutput
 *   \brief Class FLTKTextBufferLogOutput represents a standard stream output stream.
 *   This class provides thread safety for the standard stream output stream.
 *
 * \ingroup Logging
 */

class FLTKTextBufferLogOutput : public ::itk::LogOutput
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( FLTKTextBufferLogOutput, ::itk::LogOutput )

  /** Set up definitions for the Logger */
  igstkLoggerMacro()


public:

  typedef Fl_Text_Buffer  StreamType;
  typedef StreamType*     StreamPointerType;
  
  /** Get an output stream pointer */
  igstkGetMacro(Stream, StreamPointerType);

  /** Set an output stream pointer */
  void SetStream(StreamType &Stream);
  
  /** Flush a buffer */
  virtual void Flush();

  /** Write to multiple outputs */
  virtual void Write(double timestamp);

  /** Write to a buffer */
  virtual void Write(std::string const &content);

  /** Write to a buffer */
  virtual void Write(std::string const &content, double timestamp);

protected:
  /** Constructor */
  FLTKTextBufferLogOutput();

  /** Destructor */
  virtual ~FLTKTextBufferLogOutput();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Stream pointer */
  StreamPointerType m_Stream;

  /** Mutex lock */
  itk::SimpleFastMutexLock m_Mutex;
};

} // end namespace igstk

#endif //__igstk_FLTKTextBufferLogOutput_h_
