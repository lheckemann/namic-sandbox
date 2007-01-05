/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkMultipleOutput.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/28 22:07:08 $
  Version:   $Revision: 1.2 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkMultipleOutput.h"

namespace igstk
{

MultipleOutput::MultipleOutput()
{
  m_Output.clear();
}


MultipleOutput::~MultipleOutput()
{
  this->Flush();
}


/** Adds an output stream to the MultipleOutput for writing. */
void 
MultipleOutput::AddOutputStream( StreamType & output )
{
  m_Output.insert( &output ); // insert the address
}


/** The Flush method flushes all the streams. */  
void 
MultipleOutput::Flush( void )
{
  ContainerType::iterator itr = m_Output.begin();
  ContainerType::iterator end = m_Output.end();
  while( itr != end )
    {
    (*itr)->flush(); 
    ++itr;
    }
}



}

