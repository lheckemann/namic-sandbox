/*!     \file SubjectInfo.cxx
        \author Martin Styner
        
        \brief C++ source for reading/writing xmlIO based files describing subjects containing
 objects and feature sets
        
        
*/

#include "SubjectInfo.h"
//
// Includes
//
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::endl;

const int numSubjectXMLkeywords = 35;
const char * subjectXMLkeywords[numSubjectXMLkeywords] = {
  "SubjectVersion","0.1", "Subject",
  "SubjectID","SubjectName",
  "SubjectAgeVal","SubjectAgeDesc",
  "SubjectGenderVal","SubjectGenderDesc",
  "SubjectRaceVal","SubjectRaceDesc",
  "SubjectGroupVal","SubjectGroupDesc",
  "SubjectIQVal","SubjectIQDesc",
  "SubjectHeightVal","SubjectHeightDesc",
  "SubjectWeightVal","SubjectWeightDesc",
  "SubjectAcquisitionDateVal","SubjectAcquisitionDateDesc",
  "SubjectMedicationVal","SubjectMedicationDesc",
  "SubjectNumObjects",
  "Object", "ObjectName","ObjectID",
  "ObjectBinImgSegRef",
  "ObjectOrigSurfSegRef",
  "ObjectRigidAlignSurfRef",
  "ObjectScaleAlignSurfRef",
  "ObjectOrigSPHARMRef",
  "ObjectRigidAlignSPHARMRef",
  "ObjectScaleAlignSPHARMRef",
  "ObjectDistanceMapRef"
};

// SubjectInfo::GetKeywordString
// --------------------------
/*! Returns the string associated with the keyword
 */
std::string SubjectInfo::GetKeywordString(keywordType keyword)
{
  return string(subjectXMLkeywords[keyword]);
}

/*! Initialization
*/
void SubjectInfo::Initialize()
{
  int randomNumber = rand(); 
  char randomName[30];
  sprintf(randomName,"Subject%010d",randomNumber);
  SetFilenameNoLoad(randomName);
}
/*! Initialization
*/
SubjectInfo::SubjectInfo()
{
  m_xmlDoc = xmlIOlib::createXmlIO();
  int randomNumber = rand(); 
  char randomName[30];
  sprintf(randomName,"Subject%010d",randomNumber);
  SetFilenameNoLoad(randomName);
  m_rootNode = GetRootCategoryNode();
}

/*!  deconstructor
*/
SubjectInfo::~SubjectInfo()
{
  m_xmlDoc->Clean();
  delete m_xmlDoc;
}

// SubjectInfo::Clean
// --------------------------
/*! frees all data and associates a new, clean, fresh Subject with 
  the current filename (no update is peformed)
  */
void SubjectInfo::Clean()
{
  xmlErrorVal error = m_xmlDoc->Clean();
  if (error < XML_NO_ERROR)  
    {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
    }

}

// SubjectInfo::SetFilename
// --------------------------
/*!         Sets a new filename, and deletes current Subject representation
                does NOT write old Subject to disk, use 'Update' prior to this call  to do so
        Both read and write access are checked
        if supplied filename exists then the contents are read and parsed
        if supplied filename does not exist, then nothing is read
        If file does not exist or no write access is established then a error/warning is returned
 */         
void SubjectInfo::SetFilename(std::string filename) 
{
  // clean up
  Clean();

  m_filename = filename;
  xmlErrorVal error = m_xmlDoc->SetFilename(filename);
  if (error >= XML_NO_ERROR) {
    bool valid = CheckDocValidity();
    if (! valid ) {
      SetFilenameNoLoad(filename);
    } else {
      // get number of pops
      m_rootNode->Append(subjectXMLkeywords[SubjectNumObjects]);
      unsigned int numObjects = 0;
      m_xmlDoc->GetAttribute(m_rootNode,numObjects);
      m_rootNode->RemoveLast();
      
      cout << "object ID's: ";
      for (unsigned int i = 0; i < numObjects; i ++) {
   int id;
   const string nodeString = GetObjectCategoryKey(i);
   
   xmlHierarchy * nodePath = GetRootCategoryNode();
   nodePath->Append(nodeString);
   nodePath->Append(subjectXMLkeywords[ObjectID]);
   m_xmlDoc->GetAttribute(nodePath,id);
   cout << id << ", ";
   
   delete nodePath;
      }
      cout << endl;
    }
    
  } else {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  }
}


// SubjectInfo::SetFilenameNoLoad
// --------------------------
/*! Sets a new filename, and deletes current DOM representation
  does NOT write old XML tree to disk, use 'Update' prior to this call  to do so
  Both read and write access are checked, nothing is being read even if file exists
  if no write access is established then a error/warning is returned
 */         
void  SubjectInfo::SetFilenameNoLoad(std::string filename)
{ 
  m_filename = filename;
  xmlErrorVal error = m_xmlDoc->SetFilenameNoLoad(filename);
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
  CreateBasicEntries();
}
        
// SubjectInfo::IsWriteable
// --------------------------
/*!          Returns write access availability (checks physically on file system)        
        Updates the internal access variables for read and write access
 */         
bool SubjectInfo::IsWriteable() 
{ 
  return m_xmlDoc->IsWriteable();
}

// SubjectInfo::IsValid
// --------------------------
/*!         Returns whether the current XML file is valid
 */         
bool SubjectInfo::IsValid()
{
  bool valid = m_xmlDoc->IsValid();
  if (valid && !CheckDocValidity()) {
    valid = false;
  }

  return valid;
}

// SubjectInfo::UpdateFile
// --------------------------
/*!          Writes internal DOM to file. Returns error if no write access.
 */         
void  SubjectInfo::UpdateFile() 
{ 
  xmlErrorVal error = m_xmlDoc->Update();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
}

// SubjectInfo::Print
// --------------------------
/*!        Print the current subject on the standard out
*/
void SubjectInfo::Print()
{
  m_xmlDoc->Print();
}

// SubjectInfo::PrintSelf
// --------------------------
/*!        Print the current subject on the given stream
*/
void SubjectInfo::PrintSelf(std::ostream& os, itk::Indent indent) const
{  
  Superclass::PrintSelf(os,indent);   
  os << indent << "Use Print for printing the whole DOM onto cout " << std::endl;

}

// SubjectInfo::GetUserAttribute
// --------------------------
/*!        General read access with deserialize function.  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/ 
void SubjectInfo::GetUserAttribute(xmlHierarchy * hierarchy, void * retvalue,  DeSerializeFunc UserFunc) 
{ 
  xmlErrorVal error = m_xmlDoc->GetUserAttribute(hierarchy,retvalue,UserFunc);
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
}
                             

// SubjectInfo::SetUserAttribute
// --------------------------
/*!  General write access with serialize function. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
void SubjectInfo::SetUserAttribute(xmlHierarchy * hierarchy, void * setvalue,  SerializeFunc UserFunc) 
{ 
  xmlErrorVal error = m_xmlDoc->SetUserAttribute(hierarchy,setvalue,UserFunc);
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
}


// SubjectInfo::AddObject
// --------------------------
/*! Add a new population to the pop. Load the population if the file exists 
*/
int SubjectInfo::AddObject()
{
  int objectID = rand(); 

  return AddObject(objectID);
}

// SubjectInfo::AddObject
// --------------------------
/*! Add a new object to this subject. No test is done to prevent duplicates/uniqueness of ID
*/
int SubjectInfo::AddObject(int objectID)
{
  string objectName("UNKNOWN");

  return AddObject(objectID, objectName);
}

// SubjectInfo::AddObject
// --------------------------
/*! Add a new object to this subject. No test is done to prevent duplicates/uniqueness of ID
*/
int SubjectInfo::AddObject(string objectName)
{
  int objectID = rand(); 

  return AddObject(objectID, objectName);
}

// SubjectInfo::AddObject
// --------------------------
/*! Add a new object to this subject. No test is done to prevent duplicates/uniqueness of ID
*/
int SubjectInfo::AddObject(int objectID, string objectName)
{
  unsigned int numObjects = GetNumberOfObjects();
  
  const string nodeString = GetObjectCategoryKey(numObjects);
  
  // create the category node
  m_rootNode->Append(nodeString);
  xmlErrorVal error = m_xmlDoc->CreateCategoryNode(m_rootNode);
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
  m_rootNode->RemoveLast();

  // increase number of Object
  numObjects++;
  m_rootNode->Append(subjectXMLkeywords[SubjectNumObjects]);
  error =m_xmlDoc->SetAttribute(m_rootNode,numObjects);
  m_rootNode->RemoveLast();

  // set ID and Name
  xmlHierarchy * nodePath = GetRootCategoryNode();
  nodePath->Append(nodeString);
  nodePath->Append(subjectXMLkeywords[ObjectName]);
  m_xmlDoc->SetAttribute(nodePath,objectName);
  
  nodePath->ReplaceLast(subjectXMLkeywords[ObjectID]);
  m_xmlDoc->SetAttribute(nodePath,objectID);

  delete nodePath;

  return objectID;
}


// SubjectInfo::GetObjectIDlist
// --------------------------
/*! Returns a list of all populations in the pop
*/
const SubjectInfo::ObjectIDListType SubjectInfo::GetObjectIDlist()
{
  unsigned int numObjects = GetNumberOfObjects();
  static ObjectIDListType retval; 

  retval.clear();
  for (unsigned int i = 0; i < numObjects; i ++) {
    int id;
    const string nodeString = GetObjectCategoryKey(i);
    
    xmlHierarchy * nodePath = GetRootCategoryNode();
    nodePath->Append(nodeString);
    nodePath->Append(subjectXMLkeywords[ObjectID]);
    m_xmlDoc->GetAttribute(nodePath,id);
    retval.push_back(id);

    delete nodePath;
  }
  return retval;
}

// SubjectInfo::GetNumberOfObjects
// --------------------------
/*! Returns the number of population associated with this pop
*/
unsigned int SubjectInfo::GetNumberOfObjects()
{
  int numObjects = 0;
  QuerySubjectAttribute(SubjectNumObjects,numObjects);
  return numObjects;
}

// SubjectInfo::SetSubjectID
// --------------------------
/*! Sets the the subjectID, keyword SubjectID
*/
void SubjectInfo::SetSubjectID( int subjectID)
{
  SetSubjectAttribute(SubjectID,subjectID);
}

// SubjectInfo::GetSubjectID
// --------------------------
/*! Returns the the subjectID, keyword SubjectID
*/
int SubjectInfo::GetSubjectID()
{
  int subjectID;
  QuerySubjectAttribute(SubjectID,subjectID);
  return subjectID;
}

// SubjectInfo::SetSubjectName
// --------------------------
/*! Sets the the subjectName, keyword SubjectName
*/
void SubjectInfo::SetSubjectName( std::string subjectName)
{
  SetSubjectAttribute(SubjectName,subjectName);
}

// SubjectInfo::GetSubjectName
// --------------------------
/*! Returns the the subjectName, keyword SubjectName
*/
std::string SubjectInfo::GetSubjectName()
{
  std::string subjectName;
  QuerySubjectAttribute(SubjectName,subjectName);
  return subjectName;
}

// SubjectInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::SetObjectAttribute(int objectID,keywordType keyword, double entry)
{
  xmlHierarchy * objectkey = GetObjectHierarchy(objectID);
  xmlErrorVal error;
  if (objectkey) {
    objectkey->Append(subjectXMLkeywords[keyword]);
    error = m_xmlDoc->SetAttribute(objectkey,entry);
  } else {
    error = XML_ERR_CATEGORY_NODE;
  }
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string("\n SetObject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::SetObjectAttribute(int objectID,keywordType keyword, bool entry)
{
  xmlHierarchy * objectkey = GetObjectHierarchy(objectID);
  xmlErrorVal error;
  if (objectkey) {
    objectkey->Append(subjectXMLkeywords[keyword]);
    error = m_xmlDoc->SetAttribute(objectkey,entry);
  } else {
    error = XML_ERR_CATEGORY_NODE;
  }
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string("\n SetObject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::SetObjectAttribute(int objectID,keywordType keyword, int  entry)
{
   xmlHierarchy * objectkey = GetObjectHierarchy(objectID);
  xmlErrorVal error;
  if (objectkey) {
    objectkey->Append(subjectXMLkeywords[keyword]);
    error = m_xmlDoc->SetAttribute(objectkey,entry);
  } else {
    error = XML_ERR_CATEGORY_NODE;
  }
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string("\n SetObject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::SetObjectAttribute(int objectID,keywordType keyword, string  entry)
{
  xmlHierarchy * objectkey = GetObjectHierarchy(objectID);
  xmlErrorVal error;
  if (objectkey) {
    objectkey->Append(subjectXMLkeywords[keyword]);
    error = m_xmlDoc->SetAttribute(objectkey,entry);
  } else {
    error = XML_ERR_CATEGORY_NODE;
  }
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string("\n SetObject ") + string(subjectXMLkeywords[keyword]));
  } 
}


// SubjectInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::QueryObjectAttribute(int objectID,keywordType keyword, double & entry)
{
  xmlHierarchy * objectkey = GetObjectHierarchy(objectID);
  xmlErrorVal error;
  if (objectkey) {
    objectkey->Append(subjectXMLkeywords[keyword]);
    error = m_xmlDoc->GetAttribute(objectkey,entry);
  } else {
    error = XML_ERR_CATEGORY_NODE;
  }
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string("\n QueryObject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::QueryObjectAttribute(int objectID,keywordType keyword, bool & entry)
{
  xmlHierarchy * objectkey = GetObjectHierarchy(objectID);
  xmlErrorVal error;
  if (objectkey) {
    objectkey->Append(subjectXMLkeywords[keyword]);
    error = m_xmlDoc->GetAttribute(objectkey,entry);
  } else {
    error = XML_ERR_CATEGORY_NODE;
  }
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string("\n QueryObject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::QueryObjectAttribute(int objectID,keywordType keyword, int & entry)
{
  xmlHierarchy * objectkey = GetObjectHierarchy(objectID);
  xmlErrorVal error;
  if (objectkey) {
    objectkey->Append(subjectXMLkeywords[keyword]);
    error = m_xmlDoc->GetAttribute(objectkey,entry);
  } else {
    error = XML_ERR_CATEGORY_NODE;
  }
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string("\n QueryObject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::QueryObjectAttribute(int objectID,keywordType keyword, string & entry)
{
  xmlHierarchy * objectkey = GetObjectHierarchy(objectID);
  xmlErrorVal error;
  if (objectkey) {
    objectkey->Append(subjectXMLkeywords[keyword]);
    error = m_xmlDoc->GetAttribute(objectkey,entry);
  } else {
    error = XML_ERR_CATEGORY_NODE;
  }
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string("\n QueryObject ") + string(subjectXMLkeywords[keyword]));
  } 
}


// SubjectInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void  SubjectInfo::SetSubjectAttribute(keywordType keyword, double entry)
{
  m_rootNode->Append(subjectXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n SetSubject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::SetSubjectAttribute(keywordType keyword, bool entry)
{
  m_rootNode->Append(subjectXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n SetSubject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::SetSubjectAttribute(keywordType keyword, int  entry)
{
  m_rootNode->Append(subjectXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n SetSubject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::SetSubjectAttribute(keywordType keyword, string  entry)
{
  m_rootNode->Append(subjectXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n SetSubject ") + string(subjectXMLkeywords[keyword]));
  } 
}


// SubjectInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::QuerySubjectAttribute(keywordType keyword, double & entry)
{
  m_rootNode->Append(subjectXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n QuerySubject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::QuerySubjectAttribute(keywordType keyword, bool & entry)
{
  m_rootNode->Append(subjectXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n QuerySubject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::QuerySubjectAttribute(keywordType keyword, int & entry)
{
  m_rootNode->Append(subjectXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n QuerySubject ") + string(subjectXMLkeywords[keyword]));
  } 
}

// SubjectInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void SubjectInfo::QuerySubjectAttribute(keywordType keyword, string & entry)
{
  m_rootNode->Append(subjectXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n QuerySubject ") + string(subjectXMLkeywords[keyword]));
  } 
}


// ======================================================================================
// ======================================================================================
// ======================================================================================
// protected methods
// ======================================================================================
// ======================================================================================
// ======================================================================================

// SubjectInfo::GetRootCategoryNode
// --------------------------
/*!  Returns a copy of the hierarchy that points to the category node that forms the 
  root node of the  Subject*/
xmlHierarchy * SubjectInfo::GetRootCategoryNode()
{
  xmlHierarchy * nodePath  = new xmlHierarchy(); 
  m_xmlDoc->GetRootCategoryNode(nodePath);
  nodePath->Append(subjectXMLkeywords[SubjectRoot]);
  
  return nodePath;
}

// SubjectInfo::CreateBasicEntries
// --------------------------
/*!  Create the basic entries needed for every subject*/
void SubjectInfo::CreateBasicEntries()
{
  xmlHierarchy * nodePath = GetRootCategoryNode();
  m_xmlDoc->CreateCategoryNode(nodePath);

  nodePath->Append(subjectXMLkeywords[SubjectVersionName]);
  m_xmlDoc->SetAttribute(nodePath,string(subjectXMLkeywords[SubjectVersionID]));
  
  nodePath->ReplaceLast(subjectXMLkeywords[SubjectID]);
  int randomNumber = rand(); 
  m_xmlDoc->SetAttribute(nodePath,randomNumber);

  nodePath->ReplaceLast(subjectXMLkeywords[SubjectName]);
  string subjectName = m_filename;
  m_xmlDoc->SetAttribute(nodePath,subjectName);
  
  // set number of populations to 0
  unsigned int numObjects = 0;
  nodePath->ReplaceLast(subjectXMLkeywords[SubjectNumObjects]);
  m_xmlDoc->SetAttribute(nodePath,numObjects);

  delete nodePath;
}
 
// SubjectInfo::GetObjectHierarchy
// --------------------------
/*! returns the xml hierarchy  for the object with ID objectID
 */
xmlHierarchy * SubjectInfo::GetObjectHierarchy(int objectID)
{
  static xmlHierarchy * path = NULL;
  
  if (path) delete path; 
  path = NULL;

  unsigned int numObjects = GetNumberOfObjects();
  for (unsigned int i = 0; i < numObjects; i ++) {
    int id;
    const string nodeString = GetObjectCategoryKey(i);
    
    path = GetRootCategoryNode();
    path->Append(nodeString);
    path->Append(subjectXMLkeywords[ObjectID]);
    m_xmlDoc->GetAttribute(path,id);
    
    if (id == objectID) {
      path->RemoveLast();
      return path;
    }
    delete path;
    path = NULL;
  }
  return NULL;

}

// SubjectInfo::GetSubjectCategoryKey
// --------------------------
/*! Get the XML keyword for the Object Category 
*/
const std::string SubjectInfo::GetObjectCategoryKey(int number)
{
  static string keyString;
  char keyStr[50];
  sprintf(keyStr,"%s_%d",subjectXMLkeywords[Object],number);
  keyString = string(keyStr);

  return keyString;
}

// ======================================================================================
// ======================================================================================
// ======================================================================================
// private methods
// ======================================================================================
// ======================================================================================
// ======================================================================================


// SubjectInfo::CheckDocValidity
// --------------------------
/*! Check validity of Subject file */
bool SubjectInfo::CheckDocValidity()
{
  bool valid = true;
  xmlErrorVal error;

  // Is this a Subject file
  xmlHierarchy * nodePath = GetRootCategoryNode();
  bool hasSubjectRoot = m_xmlDoc->IsCategoryNode(nodePath);
  if (!hasSubjectRoot) {
    cout << "No subject root entry" << endl;
    valid = false;
  }
  // check version? Not yet necessary, maybe in future

  // check if there is an id
  nodePath->Append(subjectXMLkeywords[SubjectID]);
  int subjectID;
  error = m_xmlDoc->GetAttribute(nodePath,subjectID);
  if (error != XML_NO_ERROR) {
    cout << xmlErrors::getErrorString(error) << endl;
    valid = false;
  }

  // is there a name?
  nodePath->ReplaceLast(subjectXMLkeywords[SubjectName]);
  string subjectName;
  error = m_xmlDoc->GetAttribute(nodePath,subjectName);
  if (error != XML_NO_ERROR) {
    // no name -> create an unknown name entry
    subjectName = string("UNKNOWN");
    m_xmlDoc->SetAttribute(nodePath,subjectName);
  }

  // are there any populations
  nodePath->ReplaceLast(subjectXMLkeywords[SubjectNumObjects]);
  unsigned int numObjects;
  error = m_xmlDoc->GetAttribute(nodePath,numObjects);
  if (error != XML_NO_ERROR) {
    numObjects = 0;
    m_xmlDoc->SetAttribute(nodePath,numObjects);
  }

  delete nodePath;
  return valid;
}
