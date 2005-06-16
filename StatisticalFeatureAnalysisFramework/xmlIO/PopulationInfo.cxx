/*!     \file PopulationInfo.cxx
        \author Martin Styner
        
        \brief C++ source for reading/writing xmlIO based files describing statistical studies of
     populations consisting each of a set of objects with features
        
        
*/

#include "PopulationInfo.h"
//
// Includes
//
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::endl;

const int  numPopXMLkeywords = 10;
const char * popXMLkeywords[numPopXMLkeywords] = {
  "PopulationVersion","0.1", "Population","PopulationID","PopulationName",
  "PopulationNumSubjects", "PopulationSubject", 
  "PopulationSubjectNameCACHE", "PopulationSubjectIDCACHE", "PopulationSubjectRef"
};

// PopulationInfo::GetKeywordString
// --------------------------
/*! Returns the string associated with the keyword
 */
std::string PopulationInfo::GetKeywordString(keywordType keyword)
{
  return string(popXMLkeywords[keyword]);
}

/*!  Initialization
*/
void PopulationInfo::Initialize()
{
  int randomNumber = rand(); 
  char randomName[30];
  sprintf(randomName,"Population%010d",randomNumber);
  SetFilenameNoLoad(randomName);
}

/*!  constructor
*/
PopulationInfo::PopulationInfo()
{
  m_xmlDoc = xmlIOlib::createXmlIO();
  int randomNumber = rand(); 
  char randomName[30];
  sprintf(randomName,"Population%010d",randomNumber);
  SetFilenameNoLoad(randomName);
  m_rootNode = GetRootCategoryNode();
  m_subjectList.clear();
}

/*!  deconstructor
*/
PopulationInfo::~PopulationInfo()
{
  m_xmlDoc->Clean();
  for (unsigned int i = 0; i < m_subjectList.size(); i ++) {
    m_subjectList[i] = NULL;
  }
  m_subjectList.clear();
  delete m_xmlDoc;
}

// PopulationInfo::Clean
// --------------------------
/*! frees all data and associates a new, clean, fresh Population with 
  the current filename (no update is peformed)
  */
void PopulationInfo::Clean()
{
  for (unsigned int i = 0; i < m_subjectList.size(); i ++) {
    (m_subjectList[i])->Clean();
  }
  xmlErrorVal error = m_xmlDoc->Clean();
  if (error < XML_NO_ERROR)  
    {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
    }
}

// PopulationInfo::SetFilename
// --------------------------
/*!     Sets a new filename, and deletes current Population representation
        does NOT write old Population to disk, use 'Update' prior to this call  to do so
        Both read and write access are checked
        if supplied filename exists then the contents are read and parsed
        if supplied filename does not exist, then nothing is read
        If file does not exist or no write access is established then a error/warning is returned
 */         
void PopulationInfo::SetFilename(std::string filename) 
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
      m_rootNode->Append(popXMLkeywords[PopulationNumSubjects]);
      unsigned int num_subj = 0;
      m_xmlDoc->GetAttribute(m_rootNode, num_subj);
      m_rootNode->RemoveLast();
      
      for (unsigned int i = 0; i < num_subj; i ++) {
       string reference;
       const string nodeString = GetSubjectCategoryKey(i);
 
       xmlHierarchy * nodePath = GetRootCategoryNode();
       nodePath->Append(nodeString);
       nodePath->Append(popXMLkeywords[PopulationSubjectRef]);
       m_xmlDoc->GetAttribute(nodePath,reference);
       cout << "loading referenced subject " << reference << endl;
       AddSubject(reference);
 
       delete nodePath;
      }
      SyncSubjectCache();
    }
  } else {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  }
}


// PopulationInfo::SetFilenameNoLoad
// --------------------------
/*!         Sets a new filename, and deletes current DOM representation
  does NOT write old XML tree to disk, use 'Update' prior to this call  to do so
  Both read and write access are checked, nothing is being read even if file exists
  if no write access is established then a error/warning is returned
 */         
void  PopulationInfo::SetFilenameNoLoad(std::string filename)
{ 
  m_filename = filename;
  xmlErrorVal error = m_xmlDoc->SetFilenameNoLoad(filename);
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
  CreateBasicEntries();
}
        
// PopulationInfo::IsWriteable
// --------------------------
/*!          Returns write access availability (checks physically on file system)        
        Updates the internal access variables for read and write access
 */         
bool PopulationInfo::IsWriteable() 
{ 
  return m_xmlDoc->IsWriteable();
}

// PopulationInfo::IsValid
// --------------------------
/*!         Returns whether the current XML file is valid
 */         
bool PopulationInfo::IsValid()
{
  bool valid = m_xmlDoc->IsValid();
  if (valid && !CheckDocValidity()) {
    valid = false;
  }
  for (unsigned int i = 0; i < m_subjectList.size(); i ++) {
    if (valid && ! (m_subjectList[i])->IsValid()) {
      valid = false;
    }
  }

  return valid;
}

// PopulationInfo::UpdateFile
// --------------------------
/*!          Writes internal DOM to file. Returns error if no write access.
 */         
void PopulationInfo::UpdateFile()
{ 
  xmlHierarchy * nodePath = GetRootCategoryNode();
  nodePath->Append(popXMLkeywords[PopulationNumSubjects]);
  m_xmlDoc->SetAttribute(nodePath,m_subjectList.size());
  delete nodePath;

  for (unsigned int i = 0; i < m_subjectList.size(); i ++) {
    (m_subjectList[i])->UpdateFile();
  }
  SyncSubjectCache();
  xmlErrorVal error = m_xmlDoc->Update();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
}

// PopulationInfo::Print
// --------------------------
/*!        Print the current population on the standard out
*/
void PopulationInfo::Print()
{
  m_xmlDoc->Print();
  for (unsigned int i = 0; i < m_subjectList.size(); i ++) {
    std::cout << "Subject : " << i + 1 << ". entry" ; 
    (m_subjectList[i])->Print();
  }
}

// PopulationInfo::PrintSelf
// --------------------------
/*!        Print the current pop on the given stream
*/
void PopulationInfo::PrintSelf(std::ostream& os, itk::Indent indent) const
{  
  Superclass::PrintSelf(os,indent);   
  os << indent << "Use Print for printing the whole DOM onto cout " << std::endl;

}

// PopulationInfo::GetUserAttribute
// --------------------------
/*!        General read access with deserialize function.  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/ 
void PopulationInfo::GetUserAttribute(xmlHierarchy * hierarchy, void * retvalue,  DeSerializeFunc UserFunc) 
{ 
  xmlErrorVal error = m_xmlDoc->GetUserAttribute(hierarchy,retvalue,UserFunc);
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
}
                             

// PopulationInfo::SetUserAttribute
// --------------------------
/*!  General write access with serialize function. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
void PopulationInfo::SetUserAttribute(xmlHierarchy * hierarchy, void * setvalue,  SerializeFunc UserFunc) 
{ 
  xmlErrorVal error = m_xmlDoc->SetUserAttribute(hierarchy,setvalue,UserFunc);
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
}


// PopulationInfo::AddSubject
// --------------------------
/*! Add a new population to the pop. Load the population if the file exists 
*/
SubjectInfo::Pointer PopulationInfo::AddSubject(std::string filename)
{
  SubjectInfo::Pointer  xmlDoc = SubjectInfo::New();
  xmlDoc->SetFilename(filename);

  const string keystring = GetSubjectCategoryKey(m_subjectList.size());
  m_subjectList.push_back(xmlDoc);
  m_rootNode->Append(keystring);
  xmlErrorVal error = m_xmlDoc->CreateCategoryNode(m_rootNode);
  m_rootNode->RemoveLast();
  if (error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
  SyncSubjectCache();

  return xmlDoc;
}

// PopulationInfo::AddSubjectNoLoad
// --------------------------
/*! Add a new population to the pop. Do not load the population file, even when it is exists
*/ 
SubjectInfo::Pointer PopulationInfo::AddSubjectNoLoad(std::string filename)
{
  SubjectInfo::Pointer  xmlDoc = SubjectInfo::New();
  xmlDoc->SetFilenameNoLoad(filename);
  
  const string keystring = GetSubjectCategoryKey(m_subjectList.size());
  m_subjectList.push_back(xmlDoc);
  m_rootNode->Append(keystring);
  xmlErrorVal error = m_xmlDoc->CreateCategoryNode(m_rootNode);
  m_rootNode->RemoveLast();
  if (error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
  SyncSubjectCache();
  
  return xmlDoc;
}

// PopulationInfo::AddSubject
// --------------------------
/*! Add a new population without specifying a filename
*/   
SubjectInfo::Pointer PopulationInfo::AddSubject()
{
  SubjectInfo::Pointer  xmlDoc = SubjectInfo::New();
  m_subjectList.push_back(xmlDoc);
  SyncSubjectCache();

  return xmlDoc;
}


// PopulationInfo::GetSubjects
// --------------------------
/*! Returns a list of all populations in the pop
*/
const PopulationInfo::SubjectListType PopulationInfo::GetSubjects()
{
  return m_subjectList;
}

// PopulationInfo::GetNumberOfSubjects
// --------------------------
/*! Returns the number of population associated with this pop
*/
unsigned int PopulationInfo::GetNumberOfSubjects()
{
  return m_subjectList.size();
}

// PopulationInfo::SetPopulationID
// --------------------------
/*! Sets the the populationID, keyword PopulationID
*/
void PopulationInfo::SetPopulationID( int populationID)
{
  SetPopulationAttribute(PopulationID,populationID);
}

// PopulationInfo::GetPopulationID
// --------------------------
/*! Returns the the populationID, keyword PopulationID
*/
int PopulationInfo::GetPopulationID()
{
  int populationID;
  QueryPopulationAttribute(PopulationID,populationID);
  return populationID;
}

// PopulationInfo::SetPopulationName
// --------------------------
/*! Sets the the popName, keyword PopulationName
*/
void PopulationInfo::SetPopulationName( std::string popName)
{
  SetPopulationAttribute(PopulationName,popName);
}

// PopulationInfo::GetPopulationName
// --------------------------
/*! Returns the the popName, keyword PopulationName
*/
std::string PopulationInfo::GetPopulationName()
{
  std::string popName;
  QueryPopulationAttribute(PopulationName,popName);
  return popName;
}

// PopulationInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void PopulationInfo::SetPopulationAttribute(keywordType keyword, double entry)
{
  m_rootNode->Append(popXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n SetPop ") + string(popXMLkeywords[keyword]));
  } 
}

// PopulationInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void PopulationInfo::SetPopulationAttribute(keywordType keyword, bool entry)
{
  m_rootNode->Append(popXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n SetPop ") + string(popXMLkeywords[keyword]));
  } 
}

// PopulationInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void PopulationInfo::SetPopulationAttribute(keywordType keyword, int  entry)
{
  m_rootNode->Append(popXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n SetPop ") + string(popXMLkeywords[keyword]));
  } 
}

// PopulationInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void PopulationInfo::SetPopulationAttribute(keywordType keyword, string  entry)
{
  m_rootNode->Append(popXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n SetPop ") + string(popXMLkeywords[keyword]));
  } 
}


// PopulationInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void PopulationInfo::QueryPopulationAttribute(keywordType keyword, double & entry)
{
  m_rootNode->Append(popXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n QueryPop ") + string(popXMLkeywords[keyword]));
  } 
}

// PopulationInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void PopulationInfo::QueryPopulationAttribute(keywordType keyword, bool & entry)
{
  m_rootNode->Append(popXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n QueryPop ") + string(popXMLkeywords[keyword]));
  } 
}

// PopulationInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void PopulationInfo::QueryPopulationAttribute(keywordType keyword, int & entry)
{
  m_rootNode->Append(popXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n QueryPop ") + string(popXMLkeywords[keyword]));
  } 
}

// PopulationInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void PopulationInfo::QueryPopulationAttribute(keywordType keyword, string & entry)
{
  m_rootNode->Append(popXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n QueryPop ") + string(popXMLkeywords[keyword]));
  } 
}


// ======================================================================================
// ======================================================================================
// ======================================================================================
// protected methods
// ======================================================================================
// ======================================================================================
// ======================================================================================

// PopulationInfo::GetRootCategoryNode
// --------------------------
/*!  Returns a copy of the hierarchy that points to the category node that forms the 
  root node of the Stat Population*/
xmlHierarchy * PopulationInfo::GetRootCategoryNode()
{
  xmlHierarchy * nodePath  = new xmlHierarchy(); 
  m_xmlDoc->GetRootCategoryNode(nodePath);
  nodePath->Append(popXMLkeywords[PopulationRoot]);
  
  return nodePath;
}

// PopulationInfo::createBasicEntries
// --------------------------
/*!  Create the basic entries needed for every population*/
void PopulationInfo::CreateBasicEntries()
{
  xmlHierarchy * nodePath = GetRootCategoryNode();
  m_xmlDoc->CreateCategoryNode(nodePath);

  nodePath->Append(popXMLkeywords[PopulationVersionName]);
  m_xmlDoc->SetAttribute(nodePath,string(popXMLkeywords[PopulationVersionID]));
  
  nodePath->ReplaceLast(popXMLkeywords[PopulationID]);
  int randomNumber = rand(); 
  m_xmlDoc->SetAttribute(nodePath,randomNumber);

  nodePath->ReplaceLast(popXMLkeywords[PopulationName]);
  string popName = m_filename;
  m_xmlDoc->SetAttribute(nodePath,popName);

  // set number of populations to 0
  m_subjectList.clear();
  nodePath->ReplaceLast(popXMLkeywords[PopulationNumSubjects]);
  m_xmlDoc->SetAttribute(nodePath,m_subjectList.size());

  delete nodePath;
}

// PopulationInfo::SyncSubjectCache
// --------------------------
/*!  Synchronize the cached entries of each subject associated with this pop
 */
void PopulationInfo::SyncSubjectCache()
{
  xmlHierarchy * nodePath = GetRootCategoryNode();
  nodePath->Append(popXMLkeywords[PopulationNumSubjects]);
  m_xmlDoc->SetAttribute(nodePath,m_subjectList.size());
  delete nodePath;

  for (unsigned int i = 0; i < m_subjectList.size(); i ++) {
    const string reference = (m_subjectList[i])->GetFileName();
    const string nodeString = GetSubjectCategoryKey(i);

    nodePath = GetRootCategoryNode();
    nodePath->Append(nodeString);
    nodePath->Append(popXMLkeywords[PopulationSubjectRef]);
    m_xmlDoc->SetAttribute(nodePath,reference);
  
    int subID = (m_subjectList[i])->GetSubjectID();
    nodePath->ReplaceLast(popXMLkeywords[PopulationSubjectIDCACHE]);
    m_xmlDoc->SetAttribute(nodePath,subID);

    const string subName  = (m_subjectList[i])->GetSubjectName();
    nodePath->ReplaceLast(popXMLkeywords[PopulationSubjectNameCACHE]);
    m_xmlDoc->SetAttribute(nodePath,subName);

    delete nodePath;  
  }
}

// ======================================================================================
// ======================================================================================
// ======================================================================================
// private methods
// ======================================================================================
// ======================================================================================
// ======================================================================================

// PopulationInfo::GetSubjectCategoryKey
// --------------------------
/*! Get the XML keyword for the Subject Category 
*/
const std::string PopulationInfo::GetSubjectCategoryKey(int number)
{
  static string keyString;
  char keyStr[50];
  sprintf(keyStr,"%s_%d",popXMLkeywords[PopulationSubject],number);
  keyString = string(keyStr);

  return keyString;
}

// PopulationInfo::CheckDocValidity
// --------------------------
/*! Check validity of Population file */
bool PopulationInfo::CheckDocValidity()
{
  bool valid = true;
  xmlErrorVal error;

  // Is this a Population file
  xmlHierarchy * nodePath = GetRootCategoryNode();
  bool hasPopulationRoot = m_xmlDoc->IsCategoryNode(nodePath);
  if (!hasPopulationRoot) {
    cout << "No population root entry" << endl;
    valid = false;
  }
  // check version? Not yet necessary, maybe in future

  // check if there is an id
  nodePath->Append(popXMLkeywords[PopulationID]);
  int populationID;
  error = m_xmlDoc->GetAttribute(nodePath,populationID);
  if (error != XML_NO_ERROR) {
    cout << xmlErrors::getErrorString(error) << endl;
    valid = false;
  }
  // is there a name?
  nodePath->ReplaceLast(popXMLkeywords[PopulationName]);
  string popName;
  error = m_xmlDoc->GetAttribute(nodePath,popName);
  if (error != XML_NO_ERROR) {
    // no name -> create an unknown name entry
    popName = string("UNKNOWN");
    m_xmlDoc->SetAttribute(nodePath,popName);
  }

  // are there any populations
  nodePath->ReplaceLast(popXMLkeywords[PopulationNumSubjects]);
  unsigned int numPopulations;
  error = m_xmlDoc->GetAttribute(nodePath,numPopulations);
  if (error != XML_NO_ERROR) {
    // no pops -> create an empty numberPop entry
    m_subjectList.clear();
    m_xmlDoc->SetAttribute(nodePath,m_subjectList.size());
  }

  delete nodePath;
  return valid;
}
