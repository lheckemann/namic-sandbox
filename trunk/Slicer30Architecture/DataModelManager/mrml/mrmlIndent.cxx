/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: mrmlIndent.cxx,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:29:13 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.mrml.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mrmlIndent.h"
#include "mrmlObjectFactory.h"

#define ITK_STD_INDENT 2
#define ITK_NUMBER_OF_BLANKS 40

namespace mrml
{

static const char blanks[ITK_NUMBER_OF_BLANKS+1] =
"                                        ";

/**
 * Instance creation.
 */
Indent*
Indent::
New()
{
  return new Self;
}
  
  
/**
 * Determine the next indentation level. Keep indenting by two until the 
 * max of forty.
 */
Indent 
Indent
::GetNextIndent()
{
  int indent = m_Indent + ITK_STD_INDENT;
  if ( indent > ITK_NUMBER_OF_BLANKS )
    {
    indent = ITK_NUMBER_OF_BLANKS;
    }
  return indent;
}
 
/**
 * Print out the indentation. Basically output a bunch of spaces.
 */
std::ostream& 
operator<<(std::ostream& os, const Indent& ind)
{
  os << blanks + (ITK_NUMBER_OF_BLANKS-ind.m_Indent) ;
  return os;
}

} // end namespace mrml
