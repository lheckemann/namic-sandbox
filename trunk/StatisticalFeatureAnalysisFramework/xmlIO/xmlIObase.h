

/*!        \file xmlIObase.h
        \author Martin Styner
        \brief Interface to XML parse/writer for core platform project 
        
        Full description.
*/

#ifndef __xmlIO_H
#define __xmlIO_H
 
//
// Includes
//
#include <iostream>
#include <string>

#include "xmlHierarchy.h"
#include "xmlErrors.h"

#define XML_category_tag   "category"
#define XML_attribute_tag  "attribute"
#define XML_child_cat_tag  "childCat"

#define XML_type_tag       "type"
#define XML_reference_tag  "ref"

#define XML_VersionName   "XmlIOversion"
#define XML_VersionID     "0.9"
#define XML_Root          "XmlIO"

// Class xmlIO
// --------------
/*!        \brief Abstract Interface to XML parse/writer for core platform project .
        
        Error handling is provided via integer error return values that can be looked up in 
        virtual  xmlErrors.h
        For * hierarchy handlins see XML_hierarchy.h
        
        \verbatim
        simple example:
        xmlIOlib * xmlLib = new xmlIOlib();
        xmlIO * xmlDoc = xmlLib->createXmlIO();
        xmlErrorVal openErrVal = xmlDoc->SetFilename("myXML.xml");
        if (openErrVal != XML_NO_ERROR) cout << *(xmlErrors::getErrorString(openErrVal)) << endl;
        xmlDoc->Print();
        xmlDoc->Close(); // all temporary data is freed, as well as DOM
        \endverbatim
*/
class xmlIO 
{
public:
        //! typedef for all names and keywords
        typedef std::string nameType;
        
// Constructors, destructor, assignment
public:
        //! empty constructor
        xmlIO();
        
        //! Constructor with filename
        //! if supplied filename exists then the contents are read and parsed, 
        //!   a DOM representation is created
        //! if supplied filename does not exist, then nothing is read
        //! write access is checked
        xmlIO(std::string filename);
        
        //! Destructor
        virtual ~xmlIO() {};
        
        //! Copies the whole structure and performs an update operation with
        //! the supplied filename. 
        virtual xmlIO * DeepCopy(std::string filename) = 0;
        
        //! frees all data and associates a new, clean, fresh DOM with 
        //!  the current filename (no update is peformed)
        virtual xmlErrorVal Clean();
        
        //! Closes the xmlDoc and Updates is necessary, frees all intermediary data
        virtual xmlErrorVal Close();
        
        //! Print the current DOM on the standard out
        virtual void Print() = 0;
        virtual void PrintLongFormat() = 0;
        
        
        //! Sets the filename, write access is checked
        //! if supplied filename exists then the contents are read and parsed, 
        //!   the current DOM is replaced
        //! if supplied filename does not exist, then nothing is read
        //! write access is checked
        virtual xmlErrorVal SetFilename(std::string filename);
        
        //! Sets the filename, write access is checked, no loading is performed
        virtual xmlErrorVal SetFilenameNoLoad(std::string filename);
        
        //! Returns write access availability (checks physically on file system)        
        virtual bool IsWriteable();
        
        //! returns whether the XML file is valid
        virtual bool IsValid();
    
        //! Writes internal DOM to file. Returns error if no write access.
        virtual  xmlErrorVal Update();
    
    //! enables the use of generic attribute nodes, SerializeFunc has to generate the string
    //! serial_string that is the serialized representation of the object referenced by setval
        typedef  xmlErrorVal (*SerializeFunc)(void * setvalue, std::string &serial_string); 
        //! enables the use of generic attribute nodes, DeSerializeFunc has to generate the object
           //! getvalue that is the deserialized representation of the string serial_string
        typedef  xmlErrorVal (*DeSerializeFunc)(std::string serial_string, void * getvalue);
  
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierrarchy node. 
        virtual  xmlErrorVal GetUserAttribute(xmlHierarchy * hierarchy, void * retvalue, DeSerializeFunc UserFunc);
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierarchy node. If not found then the retvalue is set to the defaultValue  
        virtual  xmlErrorVal GetUserAttribute(xmlHierarchy * hierarchy, void * * retvalue, void * defaultValue, DeSerializeFunc UserFunc);
                                        
        //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual xmlErrorVal SetUserAttribute(xmlHierarchy * hierarchy, void * setvalue,  SerializeFunc UserFunc);
    
    
        //!Returns in sub_nodes all category nodes directly attached to Node. 
        // subNodes has to be created by the caller 
        virtual xmlErrorVal GetCategoryNodes(xmlHierarchy * hierarchy, std::vector<xmlHierarchy> * sub_nodes) = 0;
        
        //!Returns the sub_node the category node that forms the root node of the DOM
        virtual xmlErrorVal GetRootCategoryNode(xmlHierarchy * sub_node) = 0;
        
        //!Returns in attribute_nodes all attribute notes directly attached to Node
        virtual xmlErrorVal GetAttributeNodes(xmlHierarchy * hierarchy, std::vector<xmlHierarchy> * attribute_nodes) = 0;
        
        //!Creates a new category node at the specified hierarchy (dafault member category)
        virtual xmlErrorVal CreateCategoryNode(xmlHierarchy * hierarchy) = 0;
        
        //! Returns true if the specified hierarchy points to an attribute node
        virtual bool IsAttributeNode(xmlHierarchy * hierarchy) = 0;
        
        //! Returns true if the specified hierarchy points to an category node
        virtual bool IsCategoryNode(xmlHierarchy * hierarchy) = 0;
        
        //! Sets category node to children category node.
    //  a child category is a true subtree only conceptionally
        virtual xmlErrorVal SetToChildCategoryNode(xmlHierarchy * hierarchy) = 0;
        
        //! Sets category node to member category node (default), i.e. not a children category node.
    //  a member category is not a true subtree conceptionally, but rather the whole subtree
    //  should be interpreted as Member
        virtual xmlErrorVal SetToMemberCategoryNode(xmlHierarchy * hierarchy) = 0;
        
        //! Returns true if the specified hierarchy points to an child category node
        //  and thus not to a member category node
        virtual bool IsChildCategoryNode(xmlHierarchy * hierarchy) = 0;
        
        //! Returns true if the specified hierarchy points to an member category node,
        //  and thus not to a child category node
        virtual bool IsMemberCategoryNode(xmlHierarchy * hierarchy) = 0;
 
 
         //! Gets the type of a attribute node, no default is necessary as all attribute nodes 
         //  must have a type (basic type = "string")
        virtual xmlErrorVal GetType(xmlHierarchy * hierarchy, std::string &retvalue) = 0;
         //! Explicitly sets the type of a attribute node
        virtual xmlErrorVal SetType(xmlHierarchy * hierarchy, std::string setvalue) = 0;
 
         //! Gets an optional reference of a attribute node, must have a default value
    //  the reference can be any string (e.g. path, or id, or name)
        virtual xmlErrorVal GetRef(xmlHierarchy * hierarchy, std::string &retvalue, std::string defaultValue) = 0;
         //! Explicitly sets the reference info of a attribute node
    //  the reference can be any string (e.g. path, or id, or name)
        virtual xmlErrorVal SetRef(xmlHierarchy * hierarchy, std::string setvalue) = 0;         
        //! Is there a reference info to this attribute node
        virtual bool HasRef(xmlHierarchy * hierarchy) = 0;
        
        //!Access of std::string    
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, std::string &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, std::string &retvalue, std::string defaultValue) = 0;
        virtual std::string * GetAttribute(xmlHierarchy * hierarchy, std::string defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy,std::string setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //! returns attribute in retvalue 
        //! Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, std::string &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, std::string &retvalue, std::string defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //! Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy, std::string setvalue) = 0;
            
        //Access of  float   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  float &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  float &retvalue,  float defaultValue) = 0;
        virtual  float * GetAttribute(xmlHierarchy * hierarchy,  float defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, float setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //! returns attribute in retvalue 
        //! Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  float &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  float &retvalue,  float defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //! Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  float setvalue) = 0;
            
        //Access of  double   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  double &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  double &retvalue,  double defaultValue) = 0;
        virtual  double * GetAttribute(xmlHierarchy * hierarchy,  double defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, double setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //! returns attribute in retvalue 
        //! Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  double &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  double &retvalue,  double defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //! Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  double setvalue) = 0;
            
        //Access of  unsigned short   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned short &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned short &retvalue,  unsigned short defaultValue) = 0;
        virtual  unsigned short * GetAttribute(xmlHierarchy * hierarchy,  unsigned short defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, unsigned short setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //! returns attribute in retvalue 
        //!Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned short &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned short &retvalue,  unsigned short defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //! Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned short setvalue) = 0;
            
        //Access of  unsigned int   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned int &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned int &retvalue,  unsigned int defaultValue) = 0;
        virtual  unsigned int * GetAttribute(xmlHierarchy * hierarchy,  unsigned int defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, unsigned int setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //! returns attribute in retvalue 
        //! Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned int &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned int &retvalue,  unsigned int defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //! Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned int setvalue) = 0;
            
        //Access of unsigned long int   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, unsigned long int &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, unsigned long int &retvalue, unsigned long int defaultValue) = 0;
        virtual  unsigned long int * GetAttribute(xmlHierarchy * hierarchy, unsigned long int defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy,unsigned long int setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //! returns attribute in retvalue 
        //! Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned long int &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned long int &retvalue, unsigned long int defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //! Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned long int setvalue) = 0;
        
        //Access of  char   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  char &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  char &retvalue,  char defaultValue) = 0;
        virtual  char * GetAttribute(xmlHierarchy * hierarchy,  char defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, char setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //!returns attribute in retvalue 
        //!Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  char &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  char &retvalue,  char defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //!Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  char setvalue) = 0;
        
        //Access of  unsigned char   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierrarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned char &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned char &retvalue,  unsigned char defaultValue) = 0;
        virtual  unsigned char * GetAttribute(xmlHierarchy * hierarchy,  unsigned char defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, unsigned char setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //!returns attribute in retvalue 
        //!Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned char &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned char &retvalue,  unsigned char defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //!Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned char setvalue) = 0;
        
        //Access of  bool   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierrarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  bool &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  bool &retvalue,  bool defaultValue) = 0;
        virtual  bool * GetAttribute(xmlHierarchy * hierarchy,  bool defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, bool setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //!returns attribute in retvalue 
        //!Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  bool &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  bool &retvalue,  bool defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //!Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  bool setvalue) = 0;
            
        //Access of  short   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierrarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  short &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  short &retvalue,  short defaultValue) = 0;
        virtual  short * GetAttribute(xmlHierarchy * hierarchy,  short defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, short setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //!returns attribute in retvalue 
        //!Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  short &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  short &retvalue,  short defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //!Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  short setvalue) = 0;
            
        //Access of  int   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierrarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  int &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  int &retvalue,  int defaultValue) = 0;
        virtual  int * GetAttribute(xmlHierarchy * hierarchy,  int defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, int setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //!returns attribute in retvalue 
        //!Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  int &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  int &retvalue,  int defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //!Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  int setvalue) = 0;
            
        //Access of long int   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //!hierrarchy node. 
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, long int &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, long int &retvalue, long int defaultValue) = 0;
        virtual  long int * GetAttribute(xmlHierarchy * hierarchy, long int defaultValue, xmlErrorVal &errorvalue) = 0;

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        virtual  xmlErrorVal SetAttribute(xmlHierarchy * hierarchy,long int setvalue) = 0;
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        //!returns attribute in retvalue 
        //!Notifies if specified keyword leads to a hierarchy node. 
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, long int &retvalue) = 0;
        virtual  xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, long int &retvalue, long int defaultValue) = 0;
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        //!Returns  error if no write access.
        virtual  xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy, long int setvalue) = 0;
        
protected:
        
        bool m_writeAccessOK; // set by the procedure IsWriteable
        bool m_readAccessOK; // set by the procedure IsWriteable
        bool m_xmlValidOK;  // set by openDoc
        bool m_changedSinceLastUpdate; // should be set when anything is changed
        std::string m_xmlFilename;  // is set by openDoc
        
        // all of the following need to be overloaded
        
        //! clean up all the internal representation
        virtual void cleanUpDoc() { m_xmlFilename.clear(); };
        
        //! Opens this file and returns correct error and warnings
        virtual  xmlErrorVal openDoc(std::string filename) 
                { std::cout << " openDoc base" << std::endl; return XML_NO_ERROR; };
        
        //!        Writes DOM to the file
        virtual  xmlErrorVal writeDoc(std::string filename) 
                { std::cout << " writeDoc base" << std::endl; return XML_NO_ERROR;};
        
        //! Initialize XML library
        virtual void initXML() {};
        
        //! create an empty document  
          virtual void createEmptyDocument() {};
          
};

#endif // __xmlIO_H


