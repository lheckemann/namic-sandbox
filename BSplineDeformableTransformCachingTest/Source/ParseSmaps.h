
#ifndef __ParseSmaps__h_
#define __ParseSmaps__h_

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

class SmapRecord
{
public:
  SmapRecord() : Header(), HeaderName()
    {
    Size = 0;
    Rss = 0;
    SharedClean = 0;
    SharedDirty = 0;
    PrivateClean = 0;
    PrivateDirty = 0;
    Referenced = 0;
    }

  unsigned int Size;
  unsigned int Rss;
  unsigned int SharedClean;
  unsigned int SharedDirty;
  unsigned int PrivateClean;
  unsigned int PrivateDirty;
  unsigned int Referenced;
  std::string  Header;
  std::string  HeaderName;
};

class ParseSmaps
{
public:
  typedef std::vector< std::string >  StringVectorType;
  typedef SmapRecord                  RecordType;
  typedef std::vector< RecordType >   RecordVectorType;

  ParseSmaps() : m_SizeToken("Size"), m_HeapToken("heap")
    {
    m_HeapUsage = 0;
    m_TokenVector.push_back("Size");
    m_TokenVector.push_back("Rss");
    m_TokenVector.push_back("Shared_Clean");
    m_TokenVector.push_back("Shared_Dirty");
    m_TokenVector.push_back("Private_Clean");
    m_TokenVector.push_back("Private_Dirty");
    m_TokenVector.push_back("Referenced");
    }

  void ParseFile(std::string& filename)
    {
    this->Initialize();

    std::ifstream inputFile( filename.c_str() );

    if ( inputFile.is_open() == false )
      {
      std::cerr << "Could not open : " << filename << std::endl;
      return;
      }

    StringVectorType stringVector;

    while( inputFile.eof() == false)
      {
      this->ReadRecord( inputFile, stringVector );

      RecordType record;
      this->ParseRecord( stringVector,  record );
      // Byte copy since no copy ctor for record.
      m_Records.push_back( record );
      }

    }

  unsigned long GetHeapUsage()
    {
    //return m_HeapUsage;
    return GetMemoryUsage("heap");
    }

  unsigned long GetStackUsage()
    {
    return GetMemoryUsage("stack");
    }

  unsigned long GetMemoryUsage( const std::string& filter )
    {
    unsigned long memoryUsage = 0;
    for (RecordVectorType::iterator riter = m_Records.begin();
          riter != m_Records.end();
          riter++)
      {
      if ( (*riter).Header.find( filter ) != std::string::npos )
        {
        memoryUsage += (*riter).Size;
        }
      }
    return memoryUsage;
    }

  unsigned long GetTotalMemoryUsage( )
    {
    unsigned long memoryUsage = 0;
    for (RecordVectorType::iterator riter = m_Records.begin();
          riter != m_Records.end();
          riter++)
      {
      memoryUsage += (*riter).Size;
      }
    return memoryUsage;
    }

  void PrintMemoryUsage()
    {
    for (RecordVectorType::iterator riter = m_Records.begin();
          riter != m_Records.end();
          riter++)
      {
      std::cout << (*riter).HeaderName << " : " << (*riter).Size << " kB." << std::endl;
      }    
    }

  void PrintMemoryUsage2()
    {
    for (RecordVectorType::iterator riter = m_Records.begin();
          riter != m_Records.end();
          riter++)
      {
      std::cout << (*riter).Header << " : " << (*riter).Size << " kB." << std::endl;
      }
    }

protected:

  void Initialize()
    {
    m_HeapFound = false;
    m_HeapUsage = 0;
    m_Records.clear();
    }

  bool StartsWithToken( std::string& aString )
    {
    // Terminate at ':'
    std::string bString;
    for (std::string::iterator siter = aString.begin();
          siter != aString.end();
          siter++)
      {
      if ( *siter != ':' )
        {
        bString.push_back( *siter );
        }
      else
        {
        break;
        }
      }

    if ( std::find( m_TokenVector.begin(), m_TokenVector.end(), bString ) 
            != m_TokenVector.end() )
      {
      return true;
      }
    else
      {
      return false;
      }
    }

  bool ReadRecord( std::ifstream& inputFile, StringVectorType& stringVector )
    {
    stringVector.clear();

    std::string aString;

    // Header line
    getline( inputFile, aString );
    stringVector.push_back( aString );

    int lastPos = inputFile.tellg();

    for (unsigned int i = 1; i < 8; i++) // at most 8 lines
      {
      std::string aString;
      getline( inputFile, aString );
      if (this->StartsWithToken( aString ) == true )
        {
        // Inefficient copy.
        stringVector.push_back( aString );
        }
      else
        {
        // Break out early if the line does not contain a token.
        inputFile.seekg (lastPos, std::ios::beg);
        break;
        }
      }

    return true; 
    }

  bool ParseRecord( StringVectorType& stringVector, RecordType& record )
    {
    this->ParseHeaderLine( *stringVector.begin(), record );

    for (StringVectorType::iterator sviter = stringVector.begin() + 1;
            sviter != stringVector.end();
            sviter++)
      {
      // Probably should loop over a list of possible tokens.
      unsigned int matchLoc = (*sviter).find( m_SizeToken );
      if ( matchLoc != std::string::npos )
        {
        unsigned long size;
        this->ParseLine( *sviter, matchLoc, m_SizeToken, size  );
        record.Size = size;
        }
      }

    // Shortcut
    /*
    if ( false == m_HeapFound && true == this->IsHeapRecord( record ) )
      {
      m_HeapFound = true;
      m_HeapUsage = record.Size;
      }
    */

    return true;
    }

  bool ParseHeaderLine( const std::string& headerLine, RecordType& record )
    {
    record.Header = headerLine;
      char first[256];
      char second[256];
      char third[256];
      char time[256];
      unsigned int val = 0;
      char name[512];
      memset( name, 0, 512 );
      sscanf( headerLine.c_str(), "%s %s %s %s %d %s", &first, &second, &third, &time, &val, &name );
    record.HeaderName = name;
    return true;
    }

  bool ParseLine( std::string& inString, unsigned int matchLoc, const std::string& token, unsigned long& val )
    {
    val = 0;
    // Start after the token.
    const char* inStringChar = inString.c_str() + matchLoc + token.length();

    sscanf( inStringChar, ":\t%d kB", &val);
    return true;
    }

  typedef std::string           TokenType;

  TokenType         m_SizeToken;
  TokenType         m_HeapToken;
  RecordVectorType  m_Records;
  unsigned long     m_HeapUsage;
  bool              m_HeapFound;

  typedef std::vector< TokenType > TokenVectorType;
  TokenVectorType   m_TokenVector;

};

#endif // __ParseSmaps__h_
