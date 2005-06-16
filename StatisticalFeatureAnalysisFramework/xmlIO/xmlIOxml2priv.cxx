

/*!        \file xmlIOxml2priv.cxx
        \author Martin Styner
        
        \brief C++ source for XML io library interface, only private member functions
        
        Full description.
*/

#include "xmlIOxml2.h"
//
// Includes
//
#include <iostream>
#include <fstream>

#include <libxml/parser.h>
#include <libxml/tree.h>

static const int debug = 0;

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
// private and protected member functions
//*****************************************************************************
//*****************************************************************************

// printElementNames
// --------------------------
/*!        Print all names of nodes attached to the specified node
        
        \param a_node print all names of nodes attached to a_node
*/
void xmlIOxml2::printElementNamesShort(xmlNode * a_node)
{
        static int level = 0;
        
        level++;
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
            //std::cout << cur_node->name << std::endl;
        if (cur_node->type == XML_ELEMENT_NODE) {
                if (IsCategoryNode(cur_node)) {
                    std::cout << "Node L"<< level << ":" << cur_node->name << std::endl;
                } else if (IsAttributeNode(cur_node)) {
                           xmlChar * val = xmlNodeGetContent(cur_node);
                           if (val) {
                                   std::cout << "  Attribute L"<< level << ", " << cur_node->name << ":"
                                           << val << std::endl;
                           } else {
                                   std::cout << "  Attribute L"<< level << ", " << cur_node->name << std::endl;
                           }
                           xmlFree(val);
           }
        }

        printElementNamesShort(cur_node->children);
    }
        level--;
}


// printElementNames
// --------------------------
/*!        Print all names of nodes attached to the specified node
        
        \param a_node print all names of nodes attached to a_node
*/
void xmlIOxml2::printElementNames(xmlNode * a_node)
{
        static int level = 0;
        
        level++;
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
            //std::cout << cur_node->name << std::endl;
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::cout << "Element L"<< level << ", name:" << cur_node->name << std::endl;
            //std::cout << cur_node->line << std::endl;
                        if (cur_node->properties) {
                    xmlAttr * cur_attr;
                               for (cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {
                                   std::cout << "  Attribute L"<< level << ", name:" << cur_attr->name;
                                   xmlChar * val = xmlGetProp(cur_node, cur_attr->name);
                                   if (val){
                                           std::cout << " content : " << val << std::endl;
                                           xmlFree(val);
                                   } else {
                                           std::cout << std::endl;
                                   }
                                   }
           }
        }

        printElementNames(cur_node->children);
    }
        level--;
}

   
// initXML
// --------------------------
/*!        INIT the XML library
*/
void xmlIOxml2::initXML()
{
    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION
    
    m_DOMdoc = NULL;
}

// creteEmptyDocument
// --------------------------
/*!        Create san empty DOM document
*/
void xmlIOxml2::createEmptyDocument()
{
  // if (debug) std::cout << "creating basic XML document" << std::endl;
    m_DOMdoc = xmlNewDoc(BAD_CAST "1.0");
        xmlCreateIntSubset(m_DOMdoc, BAD_CAST XML_Root, NULL, NULL);
    m_rootElement = xmlNewNode(NULL, BAD_CAST XML_Root);
    xmlDocSetRootElement(m_DOMdoc, m_rootElement);
        xmlNewProp(m_rootElement,BAD_CAST XML_category_tag, BAD_CAST "y");
        xmlNodeAddContent(m_rootElement, BAD_CAST"\n"); // for nicer look
        m_changedSinceLastUpdate = true;
    m_xmlValidOK = true;
        
        xmlHierarchy nodePath;
        GetRootCategoryNode(&nodePath);
        nodePath.Append(XML_VersionName);
        std::string version(XML_VersionID);
        SetAttribute(&nodePath,version);
}

// writeDoc
// --------------------------
/*!        Writes DOM to the file
*/
xmlErrorVal xmlIOxml2::writeDoc(std::string filename)
{
        // write to disk
        int indentFormat = 1; // indent the save file for better readability
    xmlKeepBlanksDefault(0);
        int saveRetVal = xmlSaveFormatFile(m_xmlFilename.c_str(), m_DOMdoc, indentFormat);
        
        if (saveRetVal < 0) { // error on saving
                return XML_UNSPEC_SAVE_ERROR;
        }
        
        return XML_NO_ERROR;
}
        
// openDoc
// --------------------------
/*!        Opens this file and returns correct errror and warnings
*/
xmlErrorVal xmlIOxml2::openDoc(std::string filename)
{
  if (debug) { std::cout << "opening " << filename << std::endl; }
  
  m_xmlFilename = filename;
        
  // initializes the DOM from the specified file and checks the integrity
  
  // Update access variables (if allowed will create empty document, when so far non-existing)
  IsWriteable();
  
  /*parse the file and get the DOM */
  m_DOMdoc = xmlParseFile(m_xmlFilename.c_str());
  
  m_changedSinceLastUpdate = false;
  
  // Set XML validity
  if (m_DOMdoc == NULL && m_readAccessOK) {        
    m_xmlValidOK = false;
  } else {
    m_xmlValidOK = true;
  }
  
  if (!m_DOMdoc) {// create empty document
    createEmptyDocument();
  }
    /*Get the root element node */
  m_rootElement = xmlDocGetRootElement(m_DOMdoc);
  
  // return values 
  if (!m_readAccessOK) {
    if (debug) std::cout << "WARN: No read access " << m_xmlFilename << std::endl;
    return XML_NO_ERROR;
  } else if (!m_xmlValidOK) {
    if (debug) std::cout << "WARN: XML not valid " << m_xmlFilename << std::endl;
    return XML_NO_VALIDATION;
  } else if (!m_writeAccessOK) {
    if (debug) std::cout << "WARN: No write access " << m_xmlFilename << std::endl;
    return XML_NO_WRITE_ACCESS;
  } 
  // else
  return XML_NO_ERROR;

}

// cleanUpDoc
// --------------------------
/*!        Clean up the current document and parser
*/
void xmlIOxml2::cleanUpDoc()
{        
    if (!m_DOMdoc) return;
    
    xmlIO::cleanUpDoc();
        
    /*free the document */
    xmlFreeDoc(m_DOMdoc);
    m_DOMdoc = NULL;
    
        // Cleaning the internal DOM 
    xmlCleanupParser();
}


// getNode
// --------------------------
/*!        return the xmlNode associated with the specified hierarchy, returns NULL on error
*/
xmlNode *  xmlIOxml2::getNode(xmlHierarchy * hierarchy)
{
    int numNodes = hierarchy->NumNodes();
    
           if (numNodes == 0) { 
                   return m_rootElement; 
    } else {
        return getNode(hierarchy, m_rootElement, 0);
           }
}

// getCreateNode
// --------------------------
/*!        return the xmlNode associated with the specified hierarchy, 
        creates the node (and sub-nodes) if necessary
*/
xmlNode *  xmlIOxml2::getCreateNode(xmlHierarchy * hierarchy)
{
        xmlNode * hierarchy_node = getNode(hierarchy);
        if (!hierarchy_node) {
                // no key yet -> check whether category nodes exist
                xmlHierarchy sub_hierarchy (*hierarchy);
                sub_hierarchy.Remove(hierarchy->NumNodes() - 1 , 1); // remove last node
                xmlErrorVal errval = CreateCategoryNode(&sub_hierarchy);
                if (errval < XML_NO_ERROR) { // an error and not a warning
                        return NULL; 
                }
                xmlNode *parent = getNode(&sub_hierarchy);
                hierarchy_node = createAttributeNode(parent,hierarchy->GetLastNode());
        } 
        
        return hierarchy_node;
}
xmlNode *  xmlIOxml2::getNode(xmlHierarchy * hierarchy, xmlNode * a_node, int level)
{
    xmlNode *cur_node = NULL;
    
    //std::cout << "- getnode: " << hierarchy->NumNodes() << "," << level;
        // the part of the hierarchy that belongs to this level
        std::string hierarchy_name = hierarchy->GetNode(level);
        
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
                std::string cur_name ((char * ) cur_node->name); 
                if (hierarchy_name == cur_name) {
                        if (hierarchy->NumNodes() <= (level + 1)) {
                                return cur_node;
                        } else {
                                return getNode(hierarchy, cur_node->children, level + 1);
                        }
                }
        }
    }
    
    return NULL;
}


// xmlIOxml2::IsAttributeNode
// --------------------------
/*!         Returns true if the specified xml node  to an attribute node
 */         
bool xmlIOxml2::IsAttributeNode(xmlNode * hierarchy_node)
{
        return (hierarchy_node->type == XML_ELEMENT_NODE && 
                xmlHasProp(hierarchy_node, BAD_CAST XML_attribute_tag));
}

// xmlIOxml2::IsCategoryNode
// --------------------------
/*!         Returns true if the specified xml node to an category node
 */         
bool xmlIOxml2::IsCategoryNode(xmlNode * hierarchy_node)
{
        return (hierarchy_node->type == XML_ELEMENT_NODE && 
                xmlHasProp(hierarchy_node, BAD_CAST XML_category_tag));
}

// xmlIOxml2::IsChildCategoryNode
// --------------------------
/*!         Returns true if the specified xml node to an child category node
 */         
bool xmlIOxml2::IsChildCategoryNode(xmlNode * hierarchy_node)
{
        xmlChar * val = xmlGetProp(hierarchy_node, BAD_CAST XML_child_cat_tag);
        bool retval = false;
        if (val[0] == 'y') {
                retval = true;
            xmlFree (val);
        }
        return (hierarchy_node->type == XML_ELEMENT_NODE && retval);
}


// xmlIOxml2::createCategoryNode
// --------------------------
/*!         Creates new category node below given parent with given name
        returns pointer to created node
        by default no children category node
 */         
xmlNode* xmlIOxml2::createCategoryNode(xmlNode * parent,std::string name)
{
        xmlNode * retval = xmlNewChild(parent, NULL, BAD_CAST name.c_str(), BAD_CAST "\n");
        xmlNewProp(retval,BAD_CAST XML_category_tag, BAD_CAST "y");
        xmlNewProp(retval,BAD_CAST XML_child_cat_tag, BAD_CAST "n");
        xmlNodeAddContent(parent, BAD_CAST"\n"); // for nicer look
        return retval;
}


// xmlIOxml2::setChildCategoryNode
// --------------------------
/*!         Sets the current categpry node to a child category node
 */         
xmlErrorVal xmlIOxml2::setChildCategoryNode(xmlNode * hierarchy_node)
{
        xmlSetProp(hierarchy_node,BAD_CAST XML_child_cat_tag, BAD_CAST "y");
        return XML_NO_ERROR;
}


// xmlIOxml2::setMemberCategoryNode
// --------------------------
/*!         Sets the current categpry node to a member category node
 */         
xmlErrorVal xmlIOxml2::setMemberCategoryNode(xmlNode * hierarchy_node)
{
        xmlSetProp(hierarchy_node,BAD_CAST XML_child_cat_tag, BAD_CAST "n");
        return XML_NO_ERROR;
}

// xmlIOxml2::createAttributeNode
// --------------------------
/*!         Creates new attribute node below given parent with given name
        returns pointer to created node
 */         
xmlNode* xmlIOxml2::createAttributeNode(xmlNode * parent,std::string name)
{
        xmlNode * retval = xmlNewChild(parent, NULL, BAD_CAST name.c_str(), NULL);
        xmlNewProp(retval,BAD_CAST XML_attribute_tag, BAD_CAST "y");
        xmlNodeAddContent(parent, BAD_CAST"\n"); // for nicer look
        return retval;
}
