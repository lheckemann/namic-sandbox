
/*!     \file xmlIOxmlpublic.cxx
        \author Martin Styner

        \brief C++ source for XML io library interface, only general functions
        
        Full description.
*/

#include "xmlIOxml2.h"
//
// Includes
//
#include <iostream>
#include <fstream>

#include <libxml/tree.h>

static const int debug = 1;


// xmlIOxml2::Print
// --------------------------
/*!        Print the current DOM on the standard out
*/
void xmlIOxml2::PrintLongFormat()
{
        if (m_xmlFilename.empty()) {
                std::cout << std::endl << "No filename" << std::endl;
                return;
        }
        std::cout << std::endl << "******* FILENAME : " << m_xmlFilename << std::endl;
        
        printElementNames(m_rootElement); 
        std::cout << std::endl;
}
        

// xmlIOxml2::Print
// --------------------------
/*!        Print the current DOM on the standard out
*/
void xmlIOxml2::Print()
{
        if (m_xmlFilename.empty()) {
                std::cout << std::endl << "No filename" << std::endl;
        } else {
                std::cout << std::endl << "******* FILENAME : " << m_xmlFilename << std::endl;
        }
        printElementNamesShort(m_rootElement); 
        std::cout << std::endl;
}
     
// xmlIOxml2::DeepCopy
// --------------------------
/*!        Copies the whole structure and performs an update operation with
        the supplied filename (no Update on the original xmlIOxml2). 
        If the update operation on the copy fails then the error is disregarded.
        Use 'IsWriteable' to check for success of update.
        
        \param filename The filename of the file associated with of the copy
        \return The copy (== NULL upon error)
*/
xmlIOxml2* xmlIOxml2::DeepCopy(std::string filename)
{
        int do_recursive = 1; // Do the Copy of the DOM recursively
        
        xmlIOxml2 * retval = new xmlIOxml2(); // no filename, no need for check
        if (! retval) {
                return NULL;
        }
        // Copy variables
        retval->m_DOMdoc = xmlCopyDoc(m_DOMdoc, do_recursive);
        if (!retval->m_DOMdoc) {
                delete retval;
                return NULL;
        }
        retval->m_xmlFilename = filename; // new filename
        retval->m_rootElement = xmlDocGetRootElement(retval->m_DOMdoc);
        retval->m_xmlValidOK = m_xmlValidOK;
        retval->m_changedSinceLastUpdate = true;
        
        retval->Update(); // if fails, just disregard
        
        return retval;
}

// xmlIOxml2::CreateCategoryNode
// --------------------------
/*!         Creates a new category node at the specified hierarchy, 
        returns warning if the specified node already exists 
        returns error if the specified node exists and is an attribute node
 */        
xmlErrorVal xmlIOxml2::CreateCategoryNode(xmlHierarchy *hierarchy)
{        
    xmlNode *hierarchy_node = getNode(hierarchy);
        if (hierarchy_node) {
                if (IsAttributeNode(hierarchy_node)){
                        return XML_ERR_ATTRIBUTE_NODE;
            } else if (IsCategoryNode(hierarchy_node)){
                        return XML_WARN_CATEGORY_NODE;
            } else {
                        return XML_ERR_UNSPEC_NODE;
        }
        } 
        // recursively create the node
        m_changedSinceLastUpdate = true;
        
        xmlNode *parent = m_rootElement;
        int numNodes = hierarchy->NumNodes();
        for (int ind = 0; ind < numNodes; ind++) {
                xmlHierarchy sub_hierarchy (*hierarchy);
                sub_hierarchy.Remove(ind + 1, numNodes - 1 - ind);
                hierarchy_node = getNode(&sub_hierarchy);
                if (!hierarchy_node) {
                        // create new node below parent node
                        hierarchy_node = createCategoryNode(parent,sub_hierarchy.GetLastNode());
                } else if (IsAttributeNode(hierarchy_node)) {
                        // path passes through an attribute node
                        return XML_ERR_ATTRIBUTE_NODE;
                }
                parent = hierarchy_node;
        }
        return XML_NO_ERROR;
}

// xmlIOxml2::IsAttributeNode
// --------------------------
/*!         Returns true if the specified hierarchy points to an attribute node
 */         
bool xmlIOxml2::IsAttributeNode(xmlHierarchy * hierarchy)
{
    xmlNode *hierarchy_node = getNode(hierarchy);
        if (!hierarchy_node) return false;
        //else std::cout << hierarchy_node->name << std::endl;
        
        if (IsAttributeNode(hierarchy_node)) {
                return true;
    } else {
                return false;
    }
}

// xmlIOxml2::IsCategoryNode
// --------------------------
/*!         Returns true if the specified hierarchy points to an category node
 */         
bool xmlIOxml2::IsCategoryNode(xmlHierarchy * hierarchy)
{
    xmlNode *hierarchy_node = getNode(hierarchy);
        if (!hierarchy_node) return false;
        //else std::cout << hierarchy_node->name << std::endl;
        
        if (IsCategoryNode(hierarchy_node)) {
                return true;
    } else {
                return false;
    }
}

// xmlIOxml2::SetToChildCategoryNode
// --------------------------
/*!         Sets category node to children category node
 */         
xmlErrorVal xmlIOxml2::SetToChildCategoryNode(xmlHierarchy * hierarchy)
{
    xmlNode *hierarchy_node = getNode(hierarchy);
        if (hierarchy_node) {
                if (IsAttributeNode(hierarchy_node)){
                        return XML_ERR_ATTRIBUTE_NODE;
        }
        } else {
                return XML_NO_SUCH_HIERARCHY;
        }
        return setChildCategoryNode(hierarchy_node);
}
        
// xmlIOxml2::SetToMemberCategoryNode
// --------------------------
/*!         Sets category node to member category node (default), i.e. not a children category node
 */         
xmlErrorVal xmlIOxml2::SetToMemberCategoryNode(xmlHierarchy * hierarchy)
{
    xmlNode *hierarchy_node = getNode(hierarchy);
        if (hierarchy_node) {
                if (IsAttributeNode(hierarchy_node)){
                        return XML_ERR_ATTRIBUTE_NODE;
        }
        } else {
                return XML_NO_SUCH_HIERARCHY;
        }
        return setMemberCategoryNode(hierarchy_node);
}
        
// xmlIOxml2::IsChildCategoryNode
// --------------------------
/*!          Returns true if the specified hierarchy points to an child category node
                and thus not to a member category node
 */         
bool xmlIOxml2::IsChildCategoryNode(xmlHierarchy * hierarchy)
{
    xmlNode *hierarchy_node = getNode(hierarchy);
        if (!hierarchy_node) return false;
        //else std::cout << hierarchy_node->name << std::endl;
        
        if (IsCategoryNode(hierarchy_node)) {
                return IsChildCategoryNode(hierarchy_node);
    } else {
                return false;
    }
}
        
// xmlIOxml2::IsMemberCategoryNode
// --------------------------
/*!         Returns true if the specified hierarchy points to an member category node,
        and thus not to a child category node
 */         
bool xmlIOxml2::IsMemberCategoryNode(xmlHierarchy * hierarchy)
{
    xmlNode *hierarchy_node = getNode(hierarchy);
        if (!hierarchy_node) return false;
        //else std::cout << hierarchy_node->name << std::endl;
        
        if (IsCategoryNode(hierarchy_node)) {
                return !(IsChildCategoryNode(hierarchy_node));
    } else {
                return false;
    }
}

// xmlIOxml2::GetCategoryNodes
// --------------------------
/*!          Returns in sub_nodes all hierarchy nodes directly attached to Node.
        The supplied vector subNodes has to be created by the caller 
        The supplied vector subNodes is not emptied, the nodes are all appended to the vector
 */         
xmlErrorVal xmlIOxml2::GetCategoryNodes(xmlHierarchy * hierarchy, 
                                                          std::vector<xmlHierarchy> * sub_nodes) 
{ 
        if (!sub_nodes ||  !hierarchy) return XML_EMPTY_SUBNODE_POINTER;
    xmlNode *hierarchy_node = getNode(hierarchy);
        if (!hierarchy_node) return XML_NO_SUCH_HIERARCHY;
        
        
        xmlHierarchy nodePath (*hierarchy);
        int numNodes = nodePath.NumNodes();
        
        // list all hierarchy nodes that are children of this node
        // since libxml does not create a special m_rootElement, but rather the root element is the
        // first element in the file, we have to distinguish
        // whether the m_rootElement is found because of an empty hierarchy, or because of access of the
        // first element
        if (numNodes) { hierarchy_node = hierarchy_node->children; }
        
    for (xmlNode *cur_node = hierarchy_node; cur_node; cur_node = cur_node->next) {
        if (IsCategoryNode(cur_node)) {
                std::string name ((char *) cur_node->name);
                        nodePath.Append(name);
                        sub_nodes->push_back(nodePath);
                        nodePath.Remove(numNodes, 1); 
        }

    }
    
        return XML_NO_ERROR;
}

// xmlIOxml2::GetRootCategoryNode
// --------------------------
/*!          Returns in sub_node the hierarchy of the root node in the DOM 
        The supplied hierarchy subNodes has to be created by the caller 
 */         
//!Returns the sub_node the category node that forms the root node of the DOM
xmlErrorVal xmlIOxml2::GetRootCategoryNode(xmlHierarchy * sub_node)
{
        if (!sub_node) return XML_EMPTY_SUBNODE_POINTER;
        
        sub_node->Clear();
        sub_node->Append( (char *) m_rootElement->name);
    
        return XML_NO_ERROR;
}

// xmlIOxml2::GetAttributeNodes
// --------------------------
/*!          Returns in attribute_nodes all attribute notes directly attached to Node
        The supplied vector attribute_nodes has to be created by the caller 
        The supplied vector attribute_nodes is not emptied, the nodes are all appended to the vector
 */         
xmlErrorVal xmlIOxml2::GetAttributeNodes(xmlHierarchy * hierarchy, 
                                                          std::vector<xmlHierarchy> * attribute_nodes) 
{ 
        if (!attribute_nodes || !hierarchy) return XML_EMPTY_SUBNODE_POINTER;
    xmlNode *hierarchy_node = getNode(hierarchy);
        if (!hierarchy_node) return XML_NO_SUCH_HIERARCHY;
        
        xmlHierarchy nodePath (*hierarchy);
        int numNodes = nodePath.NumNodes();
        
        // list all attribute nodes that are children of this node
        // since libxml does not create a special m_rootElement, but rather the root element is the
        // first element in the file, we have to distinguish
        // whether the m_rootElement is found because of an empty hierarchy, or because of access of the
        // first element
        if (numNodes) { hierarchy_node = hierarchy_node->children; }
        
    for (xmlNode *cur_node = hierarchy_node; cur_node; cur_node = cur_node->next) {
        if (IsAttributeNode(cur_node)) {
                std::string name ((char *) cur_node->name);
                        nodePath.Append(name);
                        attribute_nodes->push_back(nodePath);
                        nodePath.Remove(numNodes, 1); 
        }

    }
    
        return XML_NO_ERROR;
}


// xmlIOxml2::GetType
// --------------------------
/*!         
 */         
//! Gets the type of a attribute node, no default is necessary as all attribute nodes 
//  must have a type (basic type = "string")
xmlErrorVal xmlIOxml2::GetType(xmlHierarchy * hierarchy, std::string &retvalue)
{
    xmlNode *hierarchy_node = getNode(hierarchy);
    if (!hierarchy_node) return XML_NO_SUCH_HIERARCHY;
    if (!IsAttributeNode(hierarchy_node)) return XML_ERR_CATEGORY_NODE;
    
        xmlChar * val = xmlGetProp(hierarchy_node, BAD_CAST XML_type_tag);
        if (val) {
                std::string this_retval((const char *) val);
                retvalue = this_retval;
                xmlFree(val);
        } else {
                std::string this_retval("string");
                retvalue = this_retval;
        }
    return XML_NO_ERROR;
}

// xmlIOxml2::SetType
// --------------------------
/*!         Explicitly sets the type of a attribute node
 */         
xmlErrorVal xmlIOxml2::SetType(xmlHierarchy * hierarchy, std::string setvalue)
{
        m_changedSinceLastUpdate = true;
    xmlNode *hierarchy_node = getNode(hierarchy);
    
    if (!hierarchy_node) return XML_NO_SUCH_HIERARCHY;
    if (!IsAttributeNode(hierarchy_node)) return XML_ERR_CATEGORY_NODE;
    
        if (xmlHasProp(hierarchy_node,BAD_CAST XML_type_tag)){
                xmlSetProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST setvalue.c_str());
        } else {
                xmlNewProp(hierarchy_node,BAD_CAST XML_type_tag, BAD_CAST setvalue.c_str());
        }
        
    return XML_NO_ERROR;
}

// xmlIOxml2::GetRef
// --------------------------
/*!         Gets an optional reference of a attribute node, must have a default value
 */         
xmlErrorVal xmlIOxml2::GetRef(xmlHierarchy * hierarchy, std::string &retvalue, std::string defaultValue)
{
    xmlNode *hierarchy_node = getNode(hierarchy);
    if (!hierarchy_node) return XML_NO_SUCH_HIERARCHY;
    if (!IsAttributeNode(hierarchy_node)) return XML_ERR_CATEGORY_NODE;
    
        xmlChar * val = xmlGetProp(hierarchy_node, BAD_CAST XML_reference_tag);
        if (val) {
                std::string this_retval((const char *) val);
                retvalue = this_retval;
                xmlFree(val);
        } else {
                retvalue = defaultValue;
        }
    return XML_NO_ERROR;
}         

// xmlIOxml2::HasRef
// --------------------------
/*!         Is there a reference info to this attribute node
 */         
bool xmlIOxml2::HasRef(xmlHierarchy * hierarchy)
{
    xmlNode *hierarchy_node = getNode(hierarchy);
    if (!hierarchy_node) return XML_NO_SUCH_HIERARCHY;
    if (!IsAttributeNode(hierarchy_node)) return XML_ERR_CATEGORY_NODE;
    
    return xmlHasProp(hierarchy_node,BAD_CAST XML_reference_tag);
}

// xmlIOxml2::SetRef
// --------------------------
/*!         Explicitly sets the reference info of a attribute node
 */         
xmlErrorVal xmlIOxml2::SetRef(xmlHierarchy * hierarchy, std::string setvalue)
{
        m_changedSinceLastUpdate = true;
    xmlNode *hierarchy_node = getNode(hierarchy);
    
    if (!hierarchy_node) return XML_NO_SUCH_HIERARCHY;
    if (!IsAttributeNode(hierarchy_node)) return XML_ERR_CATEGORY_NODE;
    
        if (xmlHasProp(hierarchy_node,BAD_CAST XML_reference_tag)){
                xmlSetProp(hierarchy_node,BAD_CAST XML_reference_tag, BAD_CAST setvalue.c_str());
        } else {
                xmlNewProp(hierarchy_node,BAD_CAST XML_reference_tag, BAD_CAST setvalue.c_str());
        }
    return XML_NO_ERROR;
}


        
