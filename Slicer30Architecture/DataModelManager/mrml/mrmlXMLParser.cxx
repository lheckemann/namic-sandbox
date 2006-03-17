#include "mrmlXMLParser.h"

#include "mrmlexpat/expat.h"
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <mrmlsys/ios/sstream>

namespace mrml
{
//----------------------------------------------------------------------------
XMLParser::XMLParser()
{
  this->Parser = 0;
  this->ParseError = 0;
}

//----------------------------------------------------------------------------
XMLParser::~XMLParser()
{
  if ( this->Parser )
    {
    this->CleanupParser();
    }
}

//----------------------------------------------------------------------------
int XMLParser::ParseString(const char* string)
{
  return this->InitializeParser() &&
    this->ParseChunk(string, strlen(string)) &&
    this->CleanupParser();
}

//----------------------------------------------------------------------------
int XMLParser::ParseFile(const char* file)
{
  if ( !file )
    {
    return 0;
    }

  std::ifstream ifs(file);
  if ( !ifs )
    {
    return 0;
    }

  mrmlsys_ios::ostringstream str;
  str << ifs.rdbuf();
  return this->ParseString(str.str().c_str());
}

//----------------------------------------------------------------------------
int XMLParser::InitializeParser()
{
  if ( this->Parser )
    {
    std::cerr << "Parser already initialized" << std::endl;
    this->ParseError = 1;
    return 0;
    }

  // Create the expat XML parser.
  this->Parser = XML_ParserCreate(0);
  XML_SetElementHandler(static_cast<XML_Parser>(this->Parser),
                        &mrmlXMLParserStartElement,
                        &mrmlXMLParserEndElement);
  XML_SetCharacterDataHandler(static_cast<XML_Parser>(this->Parser),
                              &mrmlXMLParserCharacterDataHandler);
  XML_SetUserData(static_cast<XML_Parser>(this->Parser), this);
  this->ParseError = 0;
  return 1;
}

//----------------------------------------------------------------------------
int XMLParser::ParseChunk(const char* inputString, unsigned int length)
{
  if ( !this->Parser )
    {
    std::cerr << "Parser not initialized" << std::endl;
    this->ParseError = 1;
    return 0;
    }
  int res;
  res = this->ParseBuffer(inputString, length);
  if ( res == 0 )
    {
    this->ParseError = 1;
    }
  return res;
}

//----------------------------------------------------------------------------
int XMLParser::CleanupParser()
{
  if ( !this->Parser )
    {
    std::cerr << "Parser not initialized" << std::endl;
    this->ParseError = 1;
    return 0;
    }
  int result = !this->ParseError;
  if(result)
    {
    // Tell the expat XML parser about the end-of-input.
    if(!XML_Parse(static_cast<XML_Parser>(this->Parser), "", 0, 1))
      {
      this->ReportXmlParseError();
      result = 0;
      }
    }

  // Clean up the parser.
  XML_ParserFree(static_cast<XML_Parser>(this->Parser));
  this->Parser = 0;

  return result;
}

//----------------------------------------------------------------------------
int XMLParser::ParseBuffer(const char* buffer, unsigned int count)
{
  // Pass the buffer to the expat XML parser.
  if(!XML_Parse(static_cast<XML_Parser>(this->Parser), buffer, count, 0))
    {
    this->ReportXmlParseError();
    return 0;
    }
  return 1;
}

//----------------------------------------------------------------------------
int XMLParser::ParseBuffer(const char* buffer)
{
  return this->ParseBuffer(buffer, static_cast<int>(strlen(buffer)));
}

//----------------------------------------------------------------------------
int XMLParser::ParsingComplete()
{
  // Default behavior is to parse to end of stream.
  return 0;
}

//----------------------------------------------------------------------------
void XMLParser::StartElement(const char * name,
  const char ** atts)
{
  std::cout << "Start element: " << name << std::endl;
}

//----------------------------------------------------------------------------
void XMLParser::EndElement(const char * name)
{
  std::cout << "End element: " << name << std::endl;
}

//----------------------------------------------------------------------------
void XMLParser::CharacterDataHandler(const char* /*inData*/,
  int /*inLength*/)
{
}

//----------------------------------------------------------------------------
int XMLParser::IsSpace(char c)
{
  return isspace(c);
}

//----------------------------------------------------------------------------
void mrmlXMLParserStartElement(void* parser, const char *name,
                              const char **atts)
{
  // Begin element handler that is registered with the XML_Parser.
  // This just casts the user data to a XMLParser and calls
  // StartElement.
  static_cast<XMLParser*>(parser)->StartElement(name, atts);
}

//----------------------------------------------------------------------------
void mrmlXMLParserEndElement(void* parser, const char *name)
{
  // End element handler that is registered with the XML_Parser.  This
  // just casts the user data to a XMLParser and calls EndElement.
  static_cast<XMLParser*>(parser)->EndElement(name);
}

//----------------------------------------------------------------------------
void mrmlXMLParserCharacterDataHandler(void* parser, const char* data,
                                      int length)
{
  // Character data handler that is registered with the XML_Parser.
  // This just casts the user data to a XMLParser and calls
  // CharacterDataHandler.
  static_cast<XMLParser*>(parser)->CharacterDataHandler(data, length);
}

//----------------------------------------------------------------------------
void XMLParser::ReportXmlParseError()
{
  std::cerr << "Error parsing XML in stream at line "
    << XML_GetCurrentLineNumber(static_cast<XML_Parser>(this->Parser))
    << ": "
    << XML_ErrorString(XML_GetErrorCode(
        static_cast<XML_Parser>(this->Parser))) << std::endl;
}

//-----------------------------------------------------------------------------
void XMLParser::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
}

} // end namespace mrml
