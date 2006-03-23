#ifndef __mrmlXMLParser_h
#define __mrmlXMLParser_h

#include "mrmlObject.h"
#include "mrmlObjectFactory.h"

extern "C"
{
  void mrmlXMLParserStartElement(void*, const char*, const char**);
  void mrmlXMLParserEndElement(void*, const char*);
  void mrmlXMLParserCharacterDataHandler(void*, const char*, int);  
}

namespace mrml
{
/** \class mrmlXMLParser
 * \brief Helper class for performing XML parsing
 *
 * Superclass for all XML parsers.
 */
class MRMLCommon_EXPORT XMLParser : public Object
{
public:
  XMLParser();
  virtual ~XMLParser();

  //! Parse given XML string
  virtual int ParseString(const char* string);

  //! Parse given XML file
  virtual int ParseFile(const char* file);
  
  /**
   * When parsing fragments of XML or streaming XML, use the following three
   * methods.  InitializeParser method initialize parser but does not perform
   * any actual parsing.  ParseChunk parses framgent of XML. This has to match
   * to what was already parsed. CleanupParser finishes parsing. If there were
   * errors, CleanupParser will report them.
   */
  virtual int InitializeParser();
  virtual int ParseChunk(const char* inputString, unsigned long length);
  virtual int CleanupParser();

protected:
  //! This variable is true if there was a parse error while parsing in chunks.
  int ParseError;

  //1 Expat parser structure.  Exists only during call to Parse().
  void* Parser;

  /**
   * Called before each block of input is read from the stream to check if
   * parsing is complete.  Can be replaced by subclasses to change the
   * terminating condition for parsing.  Parsing always stops when the end of
   * file is reached in the stream.
   */
  
  virtual int ParsingComplete();

  /**
   * Called when a new element is opened in the XML source.  Should be replaced
   * by subclasses to handle each element.
   *   name = Name of new element.
   *   atts = Null-terminated array of attribute name/value pairs.  Even
   *          indices are attribute names, and odd indices are values.
   */
  virtual void StartElement(const char* name, const char** atts);
  
  //! Called at the end of an element in the XML source opened when StartElement
  // was called.
  virtual void EndElement(const char* name);
  
  //! Called when there is character data to handle.
  virtual void CharacterDataHandler(const char* data, int length);  

  //! Called by Parse to report an XML syntax error.
  virtual void ReportXmlParseError();  

  //! Utility for convenience of subclasses.  Wraps isspace C library
  // routine.
  static int IsSpace(char c);  
  
  //! Send the given buffer to the XML parser.
  virtual int ParseBuffer(const char* buffer, unsigned long count);
  
  //! Send the given c-style string to the XML parser.
  int ParseBuffer(const char* buffer);

  void PrintSelf(std::ostream& os, Indent indent) const;

  //! Callbacks for the expat
  friend void mrmlXMLParserStartElement(void*, const char*, const char**);
  friend void mrmlXMLParserEndElement(void*, const char*);
  friend void mrmlXMLParserCharacterDataHandler(void*, const char*, int);
};
} // end namespace mrml
#endif
