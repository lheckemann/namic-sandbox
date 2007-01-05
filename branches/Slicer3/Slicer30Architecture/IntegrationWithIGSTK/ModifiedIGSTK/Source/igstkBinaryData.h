/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkBinaryData.h,v $
  Language:  C++
  Date:      $Date: 2005/09/13 20:16:51 $
  Version:   $Revision: 1.3 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkBinaryData_H
#define __igstkBinaryData_H


#include <itkObject.h>
#include <vector>
#include <string>
#include "igstkMacros.h"

namespace igstk 
{
 
/** \class BinaryData 
 *
 * \brief BinaryData representing a binary data chunk.
 * 
 * This class represents a binary data chunk.
 * 
 * This class can be used for storing binary stream during communication.
 *
 **/

class IGSTK_EXPORT BinaryData
{
public:

  /** Data container type definition */
  typedef std::vector<unsigned char>  ContainerType;
  
public:

  /** Constructor */
  BinaryData();
  
  /** Constructor that copies data from an encoded string. */
  BinaryData(const char* encodedString);

  /** Constructor that copies data from an encoded string. */
  BinaryData(const std::string& encodedString);

  /** Destructor */
  virtual ~BinaryData();

  /** SetSize method resizes/allocates memory */
  void SetSize(unsigned int size);

  /** GetSize method returns the size of data */
  unsigned int GetSize() const;

  /** Get a data container (returns constant) */
  const ContainerType& GetData() const { return this->m_data; }

  /** Get a data container */
  ContainerType& GetData() { return this->m_data; }

  /** Copy data from an array */
  void CopyFrom(unsigned char* inputBegin, unsigned int inputLength);

  /** Copy data into an array */
  void CopyTo(unsigned char* output) const;

  /** Append a byte */
  void Append(unsigned char byte);
  
  /** Append data from an array */
  void Append(const unsigned char* inputBegin, unsigned int inputLength);

  /** Assign the values of one BinaryData to another */
  const BinaryData & operator=( const BinaryData & inputBinaryData );

  /** operator== redefinition */
  bool operator==( const BinaryData & inputBinaryData ) const;

  /** operator!= redefinition */
  bool operator!=( const BinaryData & inputBinaryData ) const;

  /** operator< redefinition */
  bool operator<( const BinaryData & inputBinaryData ) const ;

  /** operator[] redefinition */
  unsigned char operator[]( const unsigned int index ) const;

  /** operator[] redefinition (returns reference) */
  unsigned char& operator[]( const unsigned int index );
  
  /** operator that converts BinaryData to std::string type after encoding as ASCII */
  operator std::string() const;

  /** Method for printing the member variables of this class to an ostream */
  void Print(std::ostream& os, itk::Indent indent) const;
  
  /** Encode method encodes binary data to ASCII string in std::string. */
  static void Encode( std::string& output, const unsigned char *data, unsigned int size );

  /** Decode method decodes encoded ASCII string to binary data */
  bool Decode( const std::string& asciiString );

protected:

  /** Method for printing the header to an ostream */
  void PrintHeader(std::ostream& os, itk::Indent indent) const;

  /** Method for printing the trailer to an ostream */
  void PrintTrailer(std::ostream& itkNotUsed(os), itk::Indent itkNotUsed(indent)) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** data container */
  ContainerType   m_data;

};

/** operator<< redefinition */
std::ostream& operator<<(std::ostream& os, const igstk::BinaryData& o);
}



#endif

