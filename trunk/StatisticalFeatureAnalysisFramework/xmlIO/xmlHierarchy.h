
/*!        \file xmlHierarchy.h
        \author Martin Styner
        
        \brief Hierarchy class for accessing path's within XML documents
        
        Full description.
*/

#ifndef __xmlHierarchy_H
#define __xmlHierarchy_H

//
// Includes for base classes
//

//
// Includes
//
#include <string>
#include <vector>
//
// Forward declarations
//

// Class xmlHierarchy
// --------------
/*!        \brief Hierarchy class for accessing path's within XML documents
        
        This class is used for accessing path's within DOM XNL representations.
        It provides some basic functions to deal efficiently with hierarchies.
        There is no ordering of hierarchies at the same level.
        IMPORTANT: path entries have to start with a letter or '_',
         if not then the entries are adjusted 
*/
class xmlHierarchy 
{

// Constructors, destructor, assignment
public:
        //! typedef for path name
        typedef std::string pathType;
        typedef std::vector<pathType> hierachyPathType;

        //! Base constructor, root hierarchy
        xmlHierarchy() ;
        //! Creates Hierarchy as a copy of hierachy
        xmlHierarchy(const xmlHierarchy &hierarchy);
        
        //! Destructor
        virtual ~xmlHierarchy();

        //! returns the number of nodes in this hierarchy
        int NumNodes();
        
        //! Clears the hierarchy path
        void Clear();
        
        //! Prints the hierarchy path on the stdout
        void Print();
        
        //! Appends a copy of the following path-node name to the current hierarchy
   // If the entry is not conforming to xml standards, it will be changed to conform to it
        int Append(pathType entry) ;
        
        //! Appends a copy of the whole specified hierarchy to the current hierarchy
        int Append(const xmlHierarchy * hierarchy);
        
        //!        Replaces one node with another
        int Replace( pathType oldEntry,  pathType newEntry);
        int Replace( unsigned int index,  pathType newEntry);
        //!        Replaces one node with another
   // If the entry is not conforming to xml standards, it will be changed to conform to it
        int ReplaceLast(pathType newEntry);
        
        //! Replaces one hier sub-hierarchy with another
        int Replace( xmlHierarchy * oldHierarchy,  xmlHierarchy * newHierarchy);
        int Replace(unsigned int index, int numNodes, xmlHierarchy * newHierarchy);
        
        //! Get the level/Index of the specified node
   // If the entry is not conforming to xml standards, it will be changed to conform to it
        int GetIndex(pathType entry);
        
        //! Get the level/Index of the specified sub-hierarchy
        int GetIndex(xmlHierarchy * hierarchy);
        
        //! Insert the following node at this level/index
   // If the entry is not conforming to xml standards, it will be changed to conform to it
        int Insert(pathType posEntry, pathType newEntry);
        int Insert(xmlHierarchy * posHierarchy, pathType newEntry);
        int Insert(unsigned int index, pathType newEntry);
        
        //! Insert the following hierarchy at this level/index
   // If the entry is not conforming to xml standards, it will be changed to conform to it
        int Insert(pathType posEntry, xmlHierarchy * newHierarchy);
        int Insert(xmlHierarchy * posHierarchy, xmlHierarchy * newHierarchy);
        int Insert(unsigned int index, xmlHierarchy * newHierarchy);
        
        //! Remove numNodes nodes starting from the current index/level 
   // If the entry is not conforming to xml standards, it will be changed to conform to it
        int Remove(pathType posEntry, int numOfNodes = 1);
        int Remove(xmlHierarchy * posHierarchy, int numOfNodes = 1);
        int Remove(unsigned int index, int numOfNodes = 1);
        int RemoveLast();
        
        //! get the Node at the given index/level
        pathType GetNode(unsigned int index);
        pathType GetLastNode();
        
        //! get the SubHierarchy at the given index/level
   // If the entry is not conforming to xml standards, it will be changed to conform to it
        xmlHierarchy * GetSubHierarchy(pathType posEntry, int numOfNodes = 1);
        xmlHierarchy * GetSubHierarchy(xmlHierarchy * posHierarchy, int numOfNodes = 1);
        xmlHierarchy * GetSubHierarchy(unsigned int index, int numOfNodes = 1);
        
        // iterator ?
        
private:
        
        void check_name(pathType &entry);
        hierachyPathType * path;        
};

#endif // _xmlHierarchy__


