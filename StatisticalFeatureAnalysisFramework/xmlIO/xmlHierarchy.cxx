
/*!        \file xmlHierarchy.cxx
        \author Martin Styner
        
        \brief Hierarchy class
        
*/

#include "xmlHierarchy.h"

//
// Includes
//
#include <iostream>


static const int debug = 1;

// xmlHierarchy::xmlHierarchy
// ------------------
/*!        Constructor of the xmlHierarchy class.
*/
xmlHierarchy::xmlHierarchy()
{
         path = new hierachyPathType;        
}
  
// xmlHierarchy::xmlHierarchy
// ------------------
/*!        Copy Constructor
*/
xmlHierarchy::xmlHierarchy(const xmlHierarchy &hierarchy)
{ 
        path = new hierachyPathType( * (hierarchy.path) ); 
}
        
// xmlHierarchy::~xmlHierarchy
// -------------------
/*!        Destructor of the NewClass class.
*/
xmlHierarchy::~xmlHierarchy()
{
        delete path;
}

// xmlHierarchy::numNodes
// -------------------
/*! returns the number of nodes in this hierarchy
*/
int xmlHierarchy::NumNodes()
{ 
        return path->size();        
}

// xmlHierarchy::Clear
// -------------------
/*! Clears the hierarchy path
*/
void xmlHierarchy::Clear()
{ 
        return path->clear(); 
}


// xmlHierarchy::Print
// -------------------
/*! Prints the hierarchy path on the stdout
*/
void xmlHierarchy::Print()
{
        hierachyPathType::iterator pos;
        //std::cout << "Hierarchy: ";
        for (pos = path->begin(); pos != path->end(); pos++) {
                std::cout << " [ " << *pos << " ] " ;
        }
        std::cout << std::endl;
}

// xmlHierarchy::Append
// -------------------
/*! Appends a copy of the following path-node name to the current hierarchy
*/
int xmlHierarchy::Append(pathType entry) 
{ 
        check_name(entry);
        path->push_back(entry); 
        return 0;
}

// xmlHierarchy::Append
// -------------------
/*! Appends a copy of the whole specified hierarchy to the current hierarchy
*/
int xmlHierarchy::Append(const xmlHierarchy * hierarchy)
{ 
        if (!hierarchy) return -1;
        
        hierachyPathType::iterator pos;
        for (pos = (hierarchy->path)->begin();
                pos != (hierarchy->path)->end(); pos++) {
                path->push_back(*pos);
        }
        return 0;
}
        
// xmlHierarchy::GetIndex
// -------------------
/*! Get the level/Index of the specified node
        Returns -1 if the node does not exist
*/        
int xmlHierarchy::GetIndex(pathType entry)
{         
        check_name(entry);
        unsigned int index;
        for (index = 0; index < path->size() && ( path->at(index) != entry); index++) {}
        
        if (index >= path->size()) return -1;
        // else
        return index;
                
}
        
// xmlHierarchy::GetIndex
// -------------------
/*! Get the level/Index of the specified sub-hierarchy
        Returns -1 if not found
*/        
int xmlHierarchy::GetIndex(xmlHierarchy * hierarchy)
{ 
        if (!hierarchy) return -1;
        
        int foundIndex = -1;
        for (unsigned int index = 0; index < path->size() && foundIndex == -1; index++) {
                if (path->at(index) == (hierarchy->path)->at(0)) {
                        int indexFwd = index;
                        bool found = true;
                        for (unsigned int indexInput = 0; indexInput < (hierarchy->path)->size(); indexInput++) {
                                if (path->at(indexFwd) != (hierarchy->path)->at(indexInput)) {
                                        found = false;
                                }
                                indexFwd++;
                        }
                        if (found) { foundIndex = index; }
                }
        }
        
        // else
        return foundIndex;

}


// xmlHierarchy::Replace
// -------------------
/*! Replaces one node with another
        returns -1 if oldEntry is not containde in hierarchy
*/                
int xmlHierarchy::Replace(pathType oldEntry, pathType newEntry)
{         check_name(oldEntry);
        return Replace(GetIndex(oldEntry), newEntry);}
int xmlHierarchy::Replace(unsigned int index, pathType newEntry)
{ 
        check_name(newEntry);
        if (index < 0 || index >= path->size()) return -1;
        
        int insRet = Insert(index,newEntry);
        if (insRet < 0) return -1;
        
        return Remove(index+1,1);
}
int xmlHierarchy::ReplaceLast(pathType newEntry)
{ return Replace(path->size()-1, newEntry);
}
        
// xmlHierarchy::Replace
// -------------------
/*! Replaces one hier sub-hierarchy with another
*/        
int xmlHierarchy::Replace(xmlHierarchy * oldHierarchy, xmlHierarchy * newHierarchy)
{         return Replace(GetIndex(oldHierarchy),(oldHierarchy->path)->size(),newHierarchy);}
int xmlHierarchy::Replace(unsigned int index, int numOfNodes, xmlHierarchy * newHierarchy)
{ 
        if (index < 0 || index >= path->size()|| !newHierarchy ) return -1;
        
        int insRet = Insert(index, newHierarchy);
        if (insRet < 0) return -1;
        
        return Remove(index +(newHierarchy->path)->size(), numOfNodes);
        
}
        
// xmlHierarchy::Insert
// -------------------
/*! Insert the following node at this level/index
*/        
int xmlHierarchy::Insert(pathType posEntry, pathType newEntry)
{        check_name(posEntry);
        return Insert(GetIndex(posEntry),newEntry);}
int xmlHierarchy::Insert(xmlHierarchy * posHierarchy, pathType newEntry)
{        return Insert(GetIndex(posHierarchy),newEntry);}
int xmlHierarchy::Insert(unsigned int index, pathType newEntry)
{ 
        check_name(newEntry);
        if (index < 0 || index >= path->size()) return -1;
        
        //jump to index position
        hierachyPathType::iterator pos;
        unsigned int curIndex = 0;
        for (pos = path->begin(); pos != path->end() && curIndex != index; pos++) { curIndex++; }
        
        // insert
        pos = path->insert(pos, newEntry); 
        // inserts before the current entry , returns new position 
        
        return 0;
}
        
// xmlHierarchy::Insert
// -------------------
/*! Insert the following hierarchy at this level/index
*/        
int xmlHierarchy::Insert(pathType posEntry, xmlHierarchy * newHierarchy)
{        check_name(posEntry);
        return Insert(GetIndex(posEntry), newHierarchy);}
int xmlHierarchy::Insert(xmlHierarchy * posHierarchy, xmlHierarchy * newHierarchy)
{        return Insert(GetIndex(posHierarchy), newHierarchy);}
int xmlHierarchy::Insert(unsigned int index, xmlHierarchy * newHierarchy)
{
        if (index < 0 || index >= path->size() || !newHierarchy ) return -1;
        
        //jump to index position
        hierachyPathType::iterator pos;
        unsigned int curIndex = 0;
        for (pos = path->begin(); pos != path->end() && curIndex != index; pos++) { curIndex++; }
        
        // insert
        hierachyPathType::iterator pos2;
        for (pos2 = (newHierarchy->path)->begin(); 
                pos2 != (newHierarchy->path)->end(); pos2++) { 
                pos = path->insert(pos, *pos2); // inserts before the current entry , returns new position 
                pos ++;
        }
        
        return 0;
}
        
// xmlHierarchy::Remove
// -------------------
/*! Remove numNodes nodes starting from the current index/level 
*/     
int xmlHierarchy::RemoveLast()
{ return Remove(path->size()-1, 1);}   
int xmlHierarchy::Remove(pathType posEntry, int numOfNodes)
{ check_name(posEntry);
        return Remove(GetIndex(posEntry), numOfNodes);}
int xmlHierarchy::Remove(xmlHierarchy * posHierarchy, int numOfNodes)
{        return Remove(GetIndex(posHierarchy), numOfNodes);}
int xmlHierarchy::Remove(unsigned int index, int numOfNodes)
{ 
        if (index < 0 || index >= path->size() || numOfNodes <= 0 ) return -1;
        
        //jump to index position
        hierachyPathType::iterator pos;
        unsigned int curIndex = 0;
        for (pos = path->begin(); pos != path->end() && curIndex != index; pos++) { curIndex++; }
        
        //delete
        for (int i = 0; i < numOfNodes; i++) { 
                pos = path->erase(pos);
        }

        return 0;
}
        
// xmlHierarchy::getNode
// -------------------
/*! get the Node name at the given index/level
        if index is out-of-range then the first entry is returned
*/                
xmlHierarchy::pathType xmlHierarchy::GetNode(unsigned int index)
{
        if (index < 0 || index >= path->size() ) return (*(path->begin()));
        
        //jump to index position
        hierachyPathType::iterator pos;
        unsigned int curIndex = 0;
        for (pos = path->begin(); pos != path->end() && curIndex != index; pos++) { curIndex++; }
        
        return *pos;
}
        
xmlHierarchy::pathType xmlHierarchy::GetLastNode()
{
        return ((*path)[path->size() - 1]);
}
        
        
// xmlHierarchy::getSubHierarchy
// -------------------
/*! get the SubHierarchy at the given index/level
*/        
xmlHierarchy * xmlHierarchy::GetSubHierarchy(pathType posEntry, int numOfNodes)
{ check_name(posEntry);
        return GetSubHierarchy(GetIndex(posEntry), numOfNodes);}
xmlHierarchy * xmlHierarchy::GetSubHierarchy(xmlHierarchy * posHierarchy, int numOfNodes)
{ return GetSubHierarchy(GetIndex(posHierarchy), numOfNodes);}
xmlHierarchy * xmlHierarchy::GetSubHierarchy(unsigned int index, int numOfNodes)
{ 
        if (index < 0 || index >= path->size() || numOfNodes <= 0 ) return NULL;
        
        xmlHierarchy * retval = new xmlHierarchy (*this);
        retval->Remove((unsigned int) 0, index);
        int length = (retval->path)->size();
        retval->Remove(numOfNodes, length - numOfNodes);
        
        return retval;
}

// xmlHierarchy::check_name
// -------------------
/*! check integrity of the entry name, adjust if necessary
*/        
void xmlHierarchy::check_name(pathType &entry)
{
        char thisChar;
        
        std::string::iterator currLoc = entry.begin();
        thisChar = *currLoc;
        if (!( (thisChar >= 'a' && thisChar <= 'z') ||
                   (thisChar >= 'A' && thisChar <= 'Z') ||
                    thisChar == '_')){
                        *currLoc = '_'; 
                        // force to start only with letters and _ 
                        // no use of ':' as reserved for name space
        }
        for (currLoc = entry.begin(); currLoc != entry.end(); currLoc++) {
                thisChar = *currLoc;
                if (!( (thisChar >= 'a' && thisChar <= 'z') ||
                   (thisChar >= 'A' && thisChar <= 'Z') ||
                   (thisChar >= '0' && thisChar <= '9') ||
                           thisChar == '_')){
                        *currLoc = '_'; 
                        // force to have only letters, numbers and _
                        // no use of ':' as reserved for name space
                }
        }
}

