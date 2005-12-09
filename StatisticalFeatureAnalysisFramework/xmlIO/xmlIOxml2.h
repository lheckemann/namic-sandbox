

/*!        \file xmlIOxml2.h
        \author Martin Styner
        
        \brief libxml2 Interface to XML parse/writer
        
        simple example:
        xmlIOxml2 * xmlDoc = new xmlIOxml2();
        xmlErrorVal openErrVal = xmlDoc->SetFilename("myXML.xml");
        if (openErrVal) cout << *(xmlErrors::getErrorString(openErrVal)) << endl;
        xmlDoc->Print();
        xmlDoc->Close(); // all temporary data is freed, as well as DOM
        
*/

#ifndef __xmlIOxml2_H
#define __xmlIOxml2_H
 
//
// Includes
//
#include <iostream>
#include <string>

#include "xmlIObase.h"

//libxml specific 
#include <libxml/tree.h>

// Class xmlIOxml2
// --------------
/*!        \brief Interface to XML parse/writer for libxml2 library
        More info about libxml2 can be found www.libxml.org
        
        simple example:
        xmlIOxml2 * xmlDoc = new xmlIOxml2();
        xmlErrorVal openErrVal = xmlDoc->SetFilename("myXML.xml");
        if (openErrVal) cout << *(xmlErrors::getErrorString(openErrVal)) << endl;
        xmlDoc->Print();
        xmlDoc->Close(); // all temporary data is freed, as well as DOM
*/

class xmlIOxml2 : public xmlIO
{
        
// Constructors, destructor, assignment
public:
  //! empty constructor
  xmlIOxml2() : xmlIO() { m_DOMdoc = NULL; m_rootElement = NULL;};
        
        //! Constructor with filename
        // if supplied filename exists then the contents are read and parsed, 
        //   a DOM representation is created
        // if supplied filename does not exist, then nothing is read
        // write access is checked
        xmlIOxml2(std::string filename): xmlIO(filename) {};
        
        //! Destructor
        ~xmlIOxml2() {};
        
        //! Copies the whole structure and performs an update operation with
        // the supplied filename. 
        virtual xmlIOxml2 * DeepCopy(std::string filename);
        
        //! Print the current DOM on the standard out
        virtual void Print();
        virtual void PrintLongFormat();
        
        
        //!Returns in sub_nodes all category nodes directly attached to Node. 
        // subNodes has to be created by the caller 
        virtual xmlErrorVal GetCategoryNodes(xmlHierarchy * hierarchy, std::vector<xmlHierarchy> * sub_nodes);
        
        //!Returns the sub_node the category node that forms the root node of the DOM
        virtual xmlErrorVal GetRootCategoryNode(xmlHierarchy * sub_node);
        
        //!Returns in attribute_nodes all attribute notes directly attached to Node
        virtual xmlErrorVal GetAttributeNodes(xmlHierarchy * hierarchy, std::vector<xmlHierarchy> * attribute_nodes);
        
        //!Creates a new category node at the specified hierarchy (member category by dedault)
        virtual xmlErrorVal CreateCategoryNode(xmlHierarchy * hierarchy);
        
        //! Returns true if the specified hierarchy points to an attribute node
        virtual bool IsAttributeNode(xmlHierarchy * hierarchy);
        
        //! Returns true if the specified hierarchy points to an category node
        virtual bool IsCategoryNode(xmlHierarchy * hierarchy);
        
        //! Sets category node to children category node.
    //  a child category is a true subtree only conceptionally
        virtual xmlErrorVal SetToChildCategoryNode(xmlHierarchy * hierarchy);
        
        //! Sets category node to member category node (default), i.e. not a children category node.
    //  a member category is not a true subtree conceptionally, 
        virtual xmlErrorVal SetToMemberCategoryNode(xmlHierarchy * hierarchy);
        
        //! Returns true if the specified hierarchy points to an child category node
        //  and thus not to a member category node
        virtual bool IsChildCategoryNode(xmlHierarchy * hierarchy);
        
        //! Returns true if the specified hierarchy points to an member category node,
        //  and thus not to a child category node
        virtual bool IsMemberCategoryNode(xmlHierarchy * hierarchy);
        
        
         //! Gets the type of a attribute node, no default is necessary as all attribute nodes 
         //  must have a type (basic type = "string")
        virtual xmlErrorVal GetType(xmlHierarchy * hierarchy, std::string &retvalue);
         //! Explicitly sets the type of a attribute node
        virtual xmlErrorVal SetType(xmlHierarchy * hierarchy, std::string setvalue);
 
         //! Gets an optional reference of a attribute node, must have a default value
        virtual xmlErrorVal GetRef(xmlHierarchy * hierarchy, std::string &retvalue, std::string defaultValue);
         //! Explicitly sets the reference info of a attribute node
        virtual xmlErrorVal SetRef(xmlHierarchy * hierarchy, std::string setvalue); 
        //! Is there a reference info to this attribute node
        virtual bool HasRef(xmlHierarchy * hierarchy);
        
        //Access of std::string    
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, std::string &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, std::string &retvalue, std::string defaultValue);
        std::string * GetAttribute(xmlHierarchy * hierarchy, std::string defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy,std::string setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, std::string &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, std::string &retvalue, std::string defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy, std::string setvalue);
            
        //Access of  float   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  float &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  float &retvalue,  float defaultValue);
        float * GetAttribute(xmlHierarchy * hierarchy,  float defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, float setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  float &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  float &retvalue,  float defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  float setvalue);
            
        //Access of  double   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  double &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  double &retvalue,  double defaultValue);
        double * GetAttribute(xmlHierarchy * hierarchy,  double defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, double setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  double &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  double &retvalue,  double defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  double setvalue);
            
        //Access of  unsigned short   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned short &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned short &retvalue,  unsigned short defaultValue);
        unsigned short * GetAttribute(xmlHierarchy * hierarchy,  unsigned short defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, unsigned short setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned short &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned short &retvalue,  unsigned short defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned short setvalue);
            
        //Access of  unsigned int   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned int &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned int &retvalue,  unsigned int defaultValue);
        unsigned int * GetAttribute(xmlHierarchy * hierarchy,  unsigned int defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, unsigned int setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned int &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned int &retvalue,  unsigned int defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned int setvalue);
            
        //Access of unsigned long int   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, unsigned long int &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, unsigned long int &retvalue, unsigned long int defaultValue);
        unsigned long int * GetAttribute(xmlHierarchy * hierarchy, unsigned long int defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy,unsigned long int setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned long int &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned long int &retvalue, unsigned long int defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy, unsigned long int setvalue);
        
        //Access of  char   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  char &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  char &retvalue,  char defaultValue);
        char * GetAttribute(xmlHierarchy * hierarchy,  char defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, char setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  char &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  char &retvalue,  char defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  char setvalue);
        
        //Access of  unsigned char   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned char &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  unsigned char &retvalue,  unsigned char defaultValue);
        unsigned char * GetAttribute(xmlHierarchy * hierarchy,  unsigned char defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, unsigned char setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned char &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned char &retvalue,  unsigned char defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  unsigned char setvalue);
        
        //Access of  bool   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  bool &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  bool &retvalue,  bool defaultValue);
        bool * GetAttribute(xmlHierarchy * hierarchy,  bool defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, bool setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  bool &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  bool &retvalue,  bool defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  bool setvalue);
            
        //Access of  short   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  short &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  short &retvalue,  short defaultValue);
        short * GetAttribute(xmlHierarchy * hierarchy,  short defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, short setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  short &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  short &retvalue,  short defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  short setvalue);
            
        //Access of  int   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  int &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy,  int &retvalue,  int defaultValue);
        int * GetAttribute(xmlHierarchy * hierarchy,  int defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy, int setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  int &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy,  int &retvalue,  int defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy,  int setvalue);
            
        //Access of long int   
        //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        //hierarchy node. 
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, long int &retvalue);
        xmlErrorVal GetAttribute(xmlHierarchy * hierarchy, long int &retvalue, long int defaultValue);
        long int * GetAttribute(xmlHierarchy * hierarchy, long int defaultValue, xmlErrorVal &errorvalue);

    //!Finds keyword in DOM tree and sets attribute to retvalue. Returns error if no write access.
        xmlErrorVal SetAttribute(xmlHierarchy * hierarchy,long int setvalue);
                                        
        //!Opens file, parse and check. Finds keyword, closes the file and
        // returns attribute in retvalue 
        // Notifies if specified keyword leads to a hierarchy node. 
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, long int &retvalue);
        xmlErrorVal GetAttribute(std::string filename, xmlHierarchy * hierarchy, long int &retvalue, long int defaultValue);
        //!Opens file, parse and check. Finds keyword, closes the file and sets attribute to setvalue. 
        // Returns  error if no write access.
        xmlErrorVal SetAttribute(std::string filename, xmlHierarchy * hierarchy, long int setvalue);
            
        
protected:
        
        //! clean up all the internal representation
        virtual void cleanUpDoc();
        
        //! Opens this file and returns correct error and warnings
        virtual xmlErrorVal openDoc(std::string filename);
        
        //!        Writes DOM to the file
        virtual xmlErrorVal writeDoc(std::string filename);
        
        //! Initialize XML library
        virtual void initXML();
        
        //! create an empty document  
          virtual void createEmptyDocument();
  
private:

        //! Inquire about nodetype
        bool IsAttributeNode(xmlNode * hierarchy_node);
        bool IsCategoryNode(xmlNode * hierarchy_node);
        bool IsChildCategoryNode(xmlNode * hierarchy_node);
        
        //! print all element names of nodes attached to this node
        void printElementNames(xmlNode * a_node);
        void printElementNamesShort(xmlNode * a_node);
        
        //! return the xmlNode associated with the specified * hierarchy, returns NULL on error
        xmlNode * getNode(xmlHierarchy * hierarchy);
        xmlNode * getNode(xmlHierarchy * hierarchy, xmlNode * a_node, int level);
        
        //! return the xmlNode associated with hierarchy, node is created if non-existing
        xmlNode * getCreateNode(xmlHierarchy * hierarchy);
        
        //! create a new category node attached to 'parent' called 'name'
        xmlNode*  createCategoryNode(xmlNode * parent,std::string name);
        //!         Sets the current categpry node to a child category node
        xmlErrorVal setChildCategoryNode(xmlNode * hierarchy_node);
        //!         Sets the current categpry node to a member category node
        xmlErrorVal setMemberCategoryNode(xmlNode * hierarchy_node);

        //! create a new attribute node attached to 'parent' called 'name'
        xmlNode*  createAttributeNode(xmlNode * parent,std::string name);
        
        xmlDoc * m_DOMdoc;
        xmlNode * m_rootElement;
        
};



#endif // __xmlIOxml2_H


