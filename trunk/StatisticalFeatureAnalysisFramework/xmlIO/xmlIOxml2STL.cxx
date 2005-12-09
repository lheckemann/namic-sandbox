/*!        \file xmlIOxmlSTL.C
        \author Martin Styner
        
        \brief C++ source for XML io library interface, only STL related get/set functions
        
        Full description.
*/

#include "xmlIOxml2.h"

//
// Includes
//
#include <iostream>
#include <fstream>

#include <libxml/tree.h>
#include <libxml/entities.h>

static const int debug = 1;

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of std::string .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, std::string &retvalue) 
{
        
    xmlNode *hierarchy_node = getNode(hierarchy);
        if (!hierarchy_node) {
                return XML_NO_SUCH_HIERARCHY;
        } else if (!IsAttributeNode(hierarchy_node)){
                return XML_ERR_CATEGORY_NODE;
        }
        
        xmlChar * val = xmlNodeGetContent(hierarchy_node);
        if (val) {
                std::string this_retval((const char *) val);
                retvalue = this_retval;
                xmlFree(val);
        } else {
                return XML_UNSPEC_ERROR;
        }
        
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of std::string .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,std::string &retvalue, std::string defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of std::string .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
std::string * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, std::string defaultValue, xmlErrorVal &errorvalue)
{
        std::string * retval = new std::string (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of std::string .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,std::string setvalue)
{
        m_changedSinceLastUpdate = true;
    xmlNode *hierarchy_node = getCreateNode(hierarchy);
    
    if (!hierarchy_node) return XML_ERR_ATTRIBUTE_NODE;
    
        // node exists and now associate it with the content, but first make sure
        // no XML reserved character are in the string, 
        xmlChar * setChar = xmlEncodeEntitiesReentrant(m_DOMdoc, BAD_CAST setvalue.c_str());
        xmlNodeSetContent(hierarchy_node, setChar);
        if (xmlHasProp(hierarchy_node,BAD_CAST XML_type_tag)){
                xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "string");
        } else {
                xmlNewProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "string");
        }
        xmlFree(setChar); 
        
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of std::string. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, std::string &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of std::string. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, std::string &retvalue, std::string defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of std::string. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, std::string setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}

//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of long int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, long int &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        
        double scanVal;
        int numScan = sscanf(serial_string.c_str()," %lg ",&scanVal);
        retvalue = (long int) scanVal;
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of long int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,long int &retvalue, long int defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of long int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
long int * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, long int defaultValue, xmlErrorVal &errorvalue)
{
        long int * retval = new long int (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of long int .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,long int setvalue)
{
        char charString[100];
        sprintf(charString,"%ld", setvalue);
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "long int");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of long int. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, long int &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of long int. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, long int &retvalue, long int defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of long int. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, long int setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}

//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of short .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, short &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        
        double scanVal;
        int numScan = sscanf(serial_string.c_str()," %lg ",&scanVal);
        retvalue = (short) scanVal;
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of short .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,short &retvalue, short defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of short .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
short * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, short defaultValue, xmlErrorVal &errorvalue)
{
        short * retval = new short (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of short .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,short setvalue)
{
        char charString[100];
        int setvalue_int = setvalue;
        sprintf(charString,"%d", setvalue_int);
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "short");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of short. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, short &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of short. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, short &retvalue, short defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of short. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, short setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}


//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, int &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        
        double scanVal;
        int numScan = sscanf(serial_string.c_str()," %lg ",&scanVal);
        retvalue = (int) scanVal;
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,int &retvalue, int defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
int * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, int defaultValue, xmlErrorVal &errorvalue)
{
        int * retval = new int (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of int .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,int setvalue)
{
        char charString[100];
        sprintf(charString,"%d", setvalue);
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "int");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of int. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, int &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of int. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, int &retvalue, int defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of int. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, int setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}



//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned long int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, unsigned long int &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        
        
        double scanVal;
        int numScan = sscanf(serial_string.c_str()," %lg ",&scanVal);
        retvalue = (unsigned long int) scanVal;
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned long int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,unsigned long int &retvalue, unsigned long int defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned long int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
unsigned long int * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, unsigned long int defaultValue, xmlErrorVal &errorvalue)
{
        unsigned long int * retval = new unsigned long int (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of unsigned long int .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,unsigned long int setvalue)
{
        char charString[100];
        sprintf(charString,"%lu", setvalue);
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "unsigned long int");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned long int. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned long int &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned long int. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned long int &retvalue, unsigned long int defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of unsigned long int. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned long int setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}

//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned short .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, unsigned short &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        

        double scanVal;
        int numScan = sscanf(serial_string.c_str()," %lg ",&scanVal);
        retvalue = (unsigned short) scanVal;
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned short .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,unsigned short &retvalue, unsigned short defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned short .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
unsigned short * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, unsigned short defaultValue, xmlErrorVal &errorvalue)
{
        unsigned short * retval = new unsigned short (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of unsigned short .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,unsigned short setvalue)
{
        char charString[100];
        unsigned int setvalue_int = setvalue;
        sprintf(charString,"%u", setvalue_int);
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "unsigned short");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned short. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned short &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned short. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned short &retvalue, unsigned short defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of unsigned short. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned short setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}


//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, unsigned int &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        
        
        double scanVal;
        int numScan = sscanf(serial_string.c_str()," %lg ",&scanVal);
        retvalue = (unsigned int) scanVal;
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,unsigned int &retvalue, unsigned int defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned int .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
unsigned int * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, unsigned int defaultValue, xmlErrorVal &errorvalue)
{
        unsigned int * retval = new unsigned int (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of unsigned int .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,unsigned int setvalue)
{
        char charString[100];
        sprintf(charString,"%u", setvalue);
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "unsigned int");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned int. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned int &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned int. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned int &retvalue, unsigned int defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of unsigned int. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned int setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}

//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned char .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, unsigned char &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        

        double scanVal;
        int numScan = sscanf(serial_string.c_str()," %lg ",&scanVal);
        retvalue = (unsigned char) scanVal;
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned char .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,unsigned char &retvalue, unsigned char defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned char .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
unsigned char * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, unsigned char defaultValue, xmlErrorVal &errorvalue)
{
        unsigned char * retval = new unsigned char (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of unsigned char .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,unsigned char setvalue)
{
        char charString[100];
        unsigned int setvalue_uint = setvalue;
        sprintf(charString,"%u", setvalue_uint);
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "unsigned char");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned char. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned char &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of unsigned char. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned char &retvalue, unsigned char defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of unsigned char. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned char setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}


//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of char .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, char &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        
        
        double scanVal;
        int numScan = sscanf(serial_string.c_str()," %lg ",&scanVal);
        retvalue = (char) scanVal;
        
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of char .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,char &retvalue, char defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of char .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
char * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, char defaultValue, xmlErrorVal &errorvalue)
{
        char * retval = new char (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of char .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,char setvalue)
{
        char charString[100];
        int setvalue_int = setvalue;
        sprintf(charString,"%d", setvalue_int);
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "char");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of char. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, char &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of char. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, char &retvalue, char defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of char. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, char setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}

//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of bool .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, bool &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        
        int retvalue_int;
        int numScan = sscanf(serial_string.c_str()," %d ",&retvalue_int);
        if (retvalue_int ==0) retvalue = false;
        else retvalue = true;
        
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of bool .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,bool &retvalue, bool defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of bool .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
bool * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, bool defaultValue, xmlErrorVal &errorvalue)
{
        bool * retval = new bool (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of bool .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,bool setvalue)
{
        char charString[100];
        int setvalue_int = setvalue;
        sprintf(charString,"%d", setvalue_int);
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "bool");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of bool. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, bool &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of bool. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, bool &retvalue, bool defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of bool. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, bool setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}


//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of double .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, double &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        
        int numScan = sscanf(serial_string.c_str()," %lg ",&retvalue);
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of double .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,double &retvalue, double defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of double .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
double * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, double defaultValue, xmlErrorVal &errorvalue)
{
        double * retval = new double (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of double .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,double setvalue)
{
        char charString[100];
        sprintf(charString,"%.20lg", setvalue); // precision set arbitrarily at 20
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "double");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of double. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, double &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of double. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, double &retvalue, double defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of double. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, double setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}


//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        
//************************************************************        

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of float .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, float &retvalue) 
{
        std::string serial_string;
    xmlErrorVal getErr = GetAttribute(hierarchy,serial_string);
        if (getErr < XML_NO_ERROR) { return getErr; }
        
        double retvalue_double;
        int numScan = sscanf(serial_string.c_str()," %lg ",&retvalue_double);
        retvalue = retvalue_double;
        if (numScan != 1) {return XML_GET_ERROR;}
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of float .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy,float &retvalue, float defaultValue)
{
        retvalue = defaultValue;
        return GetAttribute(hierarchy,retvalue);
}
// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of float .  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. returns defaultValue if the keyword is not found.
*/
float * xmlIOxml2::GetAttribute(xmlHierarchy * hierarchy, float defaultValue, xmlErrorVal &errorvalue)
{
        float * retval = new float (defaultValue);
        errorvalue = GetAttribute(hierarchy,*retval);
        return retval;
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of float .  
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(xmlHierarchy * hierarchy,float setvalue)
{
        char charString[100];
        sprintf(charString,"%.20g", setvalue); // precision is set arbitrarily at 20
        std::string serial_string(charString);
        xmlErrorVal errVal =  SetAttribute(hierarchy,serial_string);
    if (errVal >= XML_NO_ERROR) {
            xmlNode *hierarchy_node = getNode(hierarchy);
            if (hierarchy_node) {
                        xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST "float");
            }
    }
    return errVal;
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of float. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue remains unchanged if the keyword is 
        not found and a warning is returned.
*/         
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, float &retvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue);
}

// xmlIOxml2::GetAttribute
// --------------------------
/*!        Read Access of float. Opens file, parse and check. 
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is set to defaultValue if the keyword is 
        not found and a warning is returned.
*/
xmlErrorVal xmlIOxml2::GetAttribute(std::string filename, xmlHierarchy * hierarchy, float &retvalue, float defaultValue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        return GetAttribute(hierarchy, retvalue, defaultValue);
}

// xmlIOxml2::SetAttribute
// --------------------------
/*!        Write Access of float. Opens file, parse and check. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns error if no write access.
*/
xmlErrorVal xmlIOxml2::SetAttribute(std::string filename, xmlHierarchy * hierarchy, float setvalue)
{
        if (m_xmlFilename != filename) SetFilename(filename);
        xmlErrorVal errval = SetAttribute(hierarchy, setvalue);
        Update();
        return errval;
}



