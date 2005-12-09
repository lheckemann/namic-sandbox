
/*!        \file xmlIObase.cxx
        \author Martin Styner
        
        \brief C++ source for XML io library interface, only general functions
        
        Full description.
*/

#include "xmlIObase.h"
//
// Includes
//
#include <iostream>
#include <fstream>

static const int debug = 0;

/*! empty constructor
*/
xmlIO::xmlIO()
{
    initXML();
    createEmptyDocument();
}

/*! Constructor with filename
        if supplied filename exists then the contents are read and parsed, 
        a DOM representation is created
        if supplied filename does not exist, then nothing is read
         write access is checked
         */
xmlIO::xmlIO(std::string filename)
{
  // check whether .xml at end
  char *extension = strrchr(strdup(filename.c_str()),'.');
  if (!extension || strcmp(extension,".xml")) {
    filename = filename + std::string(".xml");
  }

  initXML();
  
  openDoc(filename); // disregard return value
}

// xmlIO::Clean
// --------------------------
/*!        frees all data and associates a new, clean, fresh DOM with 
  the current filename (no update is peformed)
  */
xmlErrorVal xmlIO::Clean()
{
  std::string tmp_filename = m_xmlFilename;
  cleanUpDoc();
  createEmptyDocument();
  m_xmlFilename = tmp_filename;
  
  return XML_NO_ERROR;
}

// xmlIO::Close
// --------------------------
/*!         Closes the xmlDoc and Updates is necessary, frees all intermediary data
*/
xmlErrorVal xmlIO::Close()
{
  // writing the internal DOM to disk
  xmlErrorVal retval = Update();
  
  // clean up
  cleanUpDoc();
  
  
  return retval;
}

// xmlIO::SetFilename
// --------------------------
/*!         Sets a new filename, and deletes current DOM representation
                does NOT write old XML tree to disk, use 'Update' prior to this call  to do so
        Both read and write access are checked
        if supplied filename exists then the contents are read and parsed, 
          the current DOM is replaced
        if supplied filename does not exist, then nothing is read
        If file does not exist or no write access is established then a error/warning is returned
 */         
xmlErrorVal xmlIO::SetFilename(std::string filename) 
{ 
  // check whether .xml at end
  char *extension = strrchr(strdup(filename.c_str()),'.');
  if (!extension || strcmp(extension,".xml")) {
    filename = filename + std::string(".xml");
  }

        // clean up
        cleanUpDoc();
        return openDoc(filename);
}


// xmlIO::SetFilenameNoLoad
// --------------------------
/*!         Sets a new filename, and deletes current DOM representation
                does NOT write old XML tree to disk, use 'Update' prior to this call  to do so
        Both read and write access are checked, nothing is being read even if file exists
        if no write access is established then a error/warning is returned
 */         
xmlErrorVal  xmlIO::SetFilenameNoLoad(std::string filename)
{
  // check whether .xml at end
  char *extension = strrchr(strdup(filename.c_str()),'.');
  if (!extension || strcmp(extension,".xml")) {
    filename = filename + std::string(".xml");
  }
        // clean up
        cleanUpDoc();
        
        createEmptyDocument();
        m_xmlFilename = filename;
        IsWriteable();
        
         // return values 
         if (!m_readAccessOK) {
                if (debug) std::cout << "WARN: No read access " << m_xmlFilename << std::endl;
                return XML_NO_ERROR;
         } else if (!m_writeAccessOK) {
                if (debug) std::cout << "WARN: No write access " << m_xmlFilename << std::endl;
                return XML_NO_WRITE_ACCESS;
         } 
         // else
        return XML_NO_ERROR;
}
        
// xmlIO::IsWriteable
// --------------------------
/*!          Returns write access availability (checks physically on file system)        
        Updates the internal access variables for read and write access
 */         
bool xmlIO::IsWriteable() 
{ 
  if (m_xmlFilename.empty()) // Do we have a filename?
    return false;
  
  m_readAccessOK = false;
  m_writeAccessOK = false;
  //read access
  std::fstream file(m_xmlFilename.c_str(), std::ios::in);
  if (file) {        // read access ok
    m_readAccessOK = true;
    file.close();
  } 
  // write access (appends, does not empty file, creates if necessary)
  std::fstream file2(m_xmlFilename.c_str(), std::ios::app | std::ios::out);
  if (file2) {        
    m_writeAccessOK =  true;
    file2.close();
    if (!m_readAccessOK) {
      remove(m_xmlFilename.c_str());
    }
  }
  
  return m_writeAccessOK;

}

// xmlIO::IsValid
// --------------------------
/*!         Returns whether the current XML file is valid
 */         
bool xmlIO::IsValid()
{
        return m_xmlValidOK;
}
// xmlIO::Update
// --------------------------
/*!          Writes internal DOM to file. Returns error if no write access.
 */         
xmlErrorVal xmlIO::Update() 
{ 
        if (m_xmlFilename.empty()) {
                return XML_NO_FILENAME;
        }
        IsWriteable();
        if (! m_writeAccessOK) {
                return XML_NO_WRITE_ACCESS;
        }
        if (!m_changedSinceLastUpdate) {
                return XML_NO_ERROR;
        }
        if (!m_xmlValidOK) {
                return XML_NO_VALIDATION;
        }
        
    std::fstream file(m_xmlFilename.c_str(), std::ios::out); // create empty file
    if (file) {        file.close();}
        
    return writeDoc(m_xmlFilename);

}

// xmlIO::GetUserAttribute
// --------------------------
/*!        General read access with deserialize function.  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/ 
xmlErrorVal xmlIO::GetUserAttribute(xmlHierarchy * hierarchy, void * retvalue,  DeSerializeFunc UserFunc) 
{ 
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        return UserFunc(serial_string, retvalue);
}

// xmlIO::GetUserAttribute
// --------------------------
/*!        General read access with deserialize function.  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIO::GetUserAttribute(xmlHierarchy * hierarchy, void * * retvalue, void * defaultValue,  DeSerializeFunc UserFunc) 
{ 
        xmlErrorVal getErr = GetUserAttribute(hierarchy,*retvalue,UserFunc);
        if (getErr < XML_NO_ERROR) {  *retvalue = defaultValue; }
        return getErr;
}
                                        

// xmlIO::SetUserAttribute
// --------------------------
/*!  General write access with serialize function. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIO::SetUserAttribute(xmlHierarchy * hierarchy, void * setvalue,  SerializeFunc UserFunc) 
{ 
        std::string serial_string;
        xmlErrorVal serErr = UserFunc(setvalue, serial_string);
        if (serErr) { return serErr; }
        return SetAttribute(hierarchy,serial_string);
}


