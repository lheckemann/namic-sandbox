/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkBinaryData.cxx,v $
  Language:  C++
  Date:      $Date: 2005/09/20 21:22:07 $
  Version:   $Revision: 1.5 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkBinaryData.h"


namespace igstk
{

/** Constructor */
BinaryData
::BinaryData()
{
}


/** Constructor that copies data from an encoded string. */
BinaryData
::BinaryData(const char* encodedString)
{
  this->Decode(encodedString);
}


/** Constructor that copies data from an encoded string. */
BinaryData
::BinaryData(const std::string& encodedString)
{
  this->Decode(encodedString);
}


/** Destructor */
BinaryData
::~BinaryData()
{
}


/** SetSize method resizes/allocates memory */
void 
BinaryData
::SetSize(unsigned int size)
{
  this->m_data.resize(size);
}


/** GetSize method returns the size of data */
unsigned int
BinaryData
::GetSize() const
{
  return this->m_data.size();
}


/** Copy data from an array */
void
BinaryData
::CopyFrom(unsigned char* inputBegin, unsigned int inputLength)
{
  this->SetSize(inputLength);
  unsigned int i;
  for( i = 0; i < inputLength; ++i )
    {
    this->m_data[i] = inputBegin[i];
    }
}


/** Copy data into an array */
void
BinaryData
::CopyTo(unsigned char* output) const
{
  const unsigned int size = this->m_data.size();
  for( unsigned int i = 0; i < size; ++i )
    {
    output[i] = this->m_data[i];
    }
}


const BinaryData &
BinaryData
::operator=( const BinaryData & inputBinaryData )
{
  this->m_data = inputBinaryData.m_data;
  return *this;
}



bool
BinaryData
::operator==( const BinaryData & inputBinaryData ) const
{
  return (this->m_data == inputBinaryData.m_data);
}



bool
BinaryData
::operator!=( const BinaryData & inputBinaryData ) const
{
  return (this->m_data != inputBinaryData.m_data );
}



bool
BinaryData
::operator<( const BinaryData & inputBinaryData ) const
{
  unsigned int i;
  unsigned int size_left = this->m_data.size();
  unsigned int size_right = inputBinaryData.m_data.size();
  unsigned int size_min = (size_left < size_right)? size_left : size_right;

  for( i = 0; i < size_min; ++i )
    {
    if( this->m_data[i] < inputBinaryData.m_data[i] )
      {
      return true;
      }
    else if( this->m_data[i] > inputBinaryData.m_data[i] )
      {
      return false;
      }
    }
  if( size_left < size_right )
    {
    return true;
    }
  return false;
}


unsigned char
BinaryData
::operator[]( const unsigned int index ) const
{
  return this->m_data[index];
}


unsigned char&
BinaryData
::operator[]( const unsigned int index )
{
  return this->m_data[index];
}


void 
BinaryData
::Print(std::ostream& os, itk::Indent indent) const
{
  this->PrintHeader(os, indent); 
  this->PrintSelf(os, indent.GetNextIndent());
  this->PrintTrailer(os, indent);
}


/**
 * Define a default print header for all objects.
 */
void 
BinaryData
::PrintHeader(std::ostream& os, itk::Indent indent) const
{
  os << indent << "BinaryData" << " (" << this << ")\n";
}


/**
 * Define a default print trailer for all objects.
 */
void 
BinaryData
::PrintTrailer(std::ostream& itkNotUsed(os), itk::Indent itkNotUsed(indent)) const
{
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<(std::ostream& os, const BinaryData& o)
{
  o.Print(os, 0);
  return os;
}


/** operator that converts BinaryData to std::string type after encoding as ASCII */
BinaryData::operator std::string() const
{
  std::string encodedString;
  
  BinaryData::Encode( encodedString, &this->m_data[0], this->GetSize() );
  return encodedString;
}


/** Encode method encodes binary data to ASCII string in std::string. */
void BinaryData::Encode( std::string& output, const unsigned char *data, unsigned int size )
{
  unsigned int i;
  itk::OStringStream os;
  
  for( i = 0; i < size; ++i )
    {
    if( data[i] == '\\' )
      {
      os << "\\\\";
      }
    else if( data[i] >= 0x20  &&  data[i] <= 0x7E )
      {
      os << data[i];
      }
    else
      {
      os << "\\x";
      os.width(2);
      os.fill('0');
      os << std::hex << std::uppercase << (int)data[i];
      }
    }
  
  output = os.str();
}


/** Decode method decodes encoded ASCII string to binary data */
bool BinaryData::Decode( const std::string& asciiString )
{
  unsigned int i;
  unsigned char byte;

  this->SetSize(0);
  for( i = 0; i < asciiString.length(); )
    {
    if( asciiString[i] == '\\' )
      {
      if( asciiString[i+1] == '\\' )
        {
        this->Append( '\\' );
        i+=2;
        }
      else if( asciiString[i+1] == 'x' )
        {
        if( isalpha(asciiString[i+2]) )
          {
          if( islower(asciiString[i+2]) )
            {
            byte = (asciiString[i+2] - 'a' + 0x0a) * 0x10;
            }
          else
            {
            byte = (asciiString[i+2] - 'A' + 0x0A) * 0x10;
            }
          }
        else
          {
          byte = (asciiString[i+2] - '0') * 0x10;
          }
          
        if( isalpha(asciiString[i+3]) )
          {
          if( islower(asciiString[i+3]) )
            {
            byte += (asciiString[i+3] - 'a' + 0x0a);
            }
          else
            {
            byte += (asciiString[i+3] - 'A' + 0x0A);
            }
          }
        else
          {
          byte += (asciiString[i+3] - '0');
          }
        i+=4;
        this->Append( byte );
        }
      else  // error
        {
        return false;
        }
      }
    else
      {
      this->Append( asciiString[i] );
      ++i;
      }
    }
  return true;
}


/** Append a byte */
void BinaryData::Append(unsigned char byte)
{
  m_data.push_back(byte);
}


/** Append data from an array */
void BinaryData::Append(const unsigned char* inputBegin, unsigned int inputLength)
{
  m_data.insert(m_data.end(), inputBegin, &inputBegin[inputLength]);
}


/** Print object information */
void BinaryData::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;

  os << indent << "Data Size: " << this->GetSize() << std::endl;
  os << indent << "Data: " << std::endl;
  os << indent;
  unsigned int i;
  for( i = 0; i < this->m_data.size(); ++i )
    {
    if( this->m_data[i] == '\\' )
      {
      os << "\\\\";
      }
    else if( this->m_data[i] >= 0x20  &&  this->m_data[i] <= 0x7E )
      {
      os << this->m_data[i];
      }
    else
      {
      os << "\\x";
      os.width(2);
      os.fill('0');
      os << std::hex << std::uppercase << (int)this->m_data[i];
      }
    }
  os << std::endl;
}


} // end namespace itk
