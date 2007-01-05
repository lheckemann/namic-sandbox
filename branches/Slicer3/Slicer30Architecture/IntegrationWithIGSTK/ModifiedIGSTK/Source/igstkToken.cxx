/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkToken.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/01 21:08:57 $
  Version:   $Revision: 1.2 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkToken.h"
#include "itkFastMutexLock.h"

#include <iostream>


namespace igstk
{



  /** The counter for providing pseudo-unique identifiers for tokens */
  Token::IdentifierType Token::IdentifierCounter = 1;

  /** Used for mutex locking */
  static ::itk::SimpleFastMutexLock    TokenMutex;
    


  Token::Token()
  {
    /** Start mutual exclusion section. This prevent race conditions when
     * multiple threads are creating Tokens simultaneously */
    TokenMutex.Lock();
    
    /** When the IdentifierCounter rolls over (reaches it maximum value and
     * restars from zero) the Uniqueness of identifiers can no longer be
     * guaranted. */
    this->m_Identifier  = IdentifierCounter++;

    TokenMutex.Unlock();
  }


  Token::~Token()
  {
  }


  /** Print Self function */
  void Token::PrintSelf( std::ostream& os, itk::Indent indent ) const
  {
    os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;

    os << indent << "Identifier: " << this->m_Identifier << std::endl;
  }

  void 
  Token
  ::Print(std::ostream& os, itk::Indent indent) const
  {
    os << indent << "Token" << " (" << this << ")\n";
    this->PrintSelf(os, indent.GetNextIndent());
  }

  std::ostream& operator<<(std::ostream& os, const Token& o)
  {
    o.Print(os, 0);
    return os;
  }


} // end namespace igstk



