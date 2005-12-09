/*!     \file StudyInfo.cxx
        \author Martin Styner
        
        \brief C++ source for reading/writing xmlIO based files describing statistical studies of
     populations consisting each of a set of objects with features
        
        
*/

#include "StudyInfo.h"
//
// Includes
//
#include <iostream>
#include <fstream>
#include <time.h>

using std::string;
using std::cout;
using std::endl;

const int numStudyXMLKeywords = 10;
const char * studyXMLkeywords[numStudyXMLKeywords] = {
  "StudyVersion","0.1", "Study","StudyID","StudyName",
  "StudyNumPopulations", "StudyPopulation", 
  "StudyPopNameCACHE", "StudyPopIDCACHE", "StudyPopRef"
};

// StudyInfo::GetKeywordString
// --------------------------
/*! Returns the string associated with the keyword
 */
std::string StudyInfo::GetKeywordString(keywordType keyword)
{
  return string(studyXMLkeywords[keyword]);
}


/*! Initialization
*/
void StudyInfo::Initialize()
{
  int randomNumber = rand(); 
  char randomName[30];
  sprintf(randomName,"Study%010d",randomNumber);
  SetFilenameNoLoad(randomName);
}

/*! constructor
*/
StudyInfo::StudyInfo()
{
  srand(time(NULL));
  m_xmlDoc = xmlIOlib::createXmlIO();
  int randomNumber = rand(); 
  char randomName[30];
  sprintf(randomName,"Study%010d",randomNumber);
  SetFilenameNoLoad(randomName);
  m_rootNode = GetRootCategoryNode();
  m_populationList.clear();
}

/*!  deconstructor
*/
StudyInfo::~StudyInfo()
{
  m_xmlDoc->Clean();
  for (unsigned int i = 0; i < m_populationList.size(); i ++) {
    m_populationList[i] = NULL;
  }
  m_populationList.clear();
  delete m_xmlDoc;
}

// StudyInfo::Clean
// --------------------------
/*! frees all data and associates a new, clean, fresh Study with 
  the current filename (no update is peformed)
  */
void StudyInfo::Clean()
{
  for (unsigned int i = 0; i < m_populationList.size(); i ++) {
    (m_populationList[i])->Clean();
  }
  xmlErrorVal error = m_xmlDoc->Clean();
  if (error < XML_NO_ERROR)  
    {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
    }
  
}

// StudyInfo::SetFilename
// --------------------------
/*!         Sets a new filename, and deletes current Study representation
                does NOT write old Study to disk, use 'Update' prior to this call  to do so
        Both read and write access are checked
        if supplied filename exists then the contents are read and parsed
        if supplied filename does not exist, then nothing is read
        If file does not exist or no write access is established then a error/warning is returned
 */         
void StudyInfo::SetFilename(std::string filename) 
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
      m_rootNode->Append(studyXMLkeywords[StudyNumPopulations]);
      unsigned int num_pops = 0;
      m_xmlDoc->GetAttribute(m_rootNode, num_pops);
      m_rootNode->RemoveLast();
      
      for (unsigned int i = 0; i < num_pops; i ++) {
       string reference;
       const string nodeString = GetPopulationCategoryKey(i);
 
       xmlHierarchy * nodePath = GetRootCategoryNode();
       nodePath->Append(nodeString);
       nodePath->Append(studyXMLkeywords[StudyPopRef]);
       m_xmlDoc->GetAttribute(nodePath,reference);
       cout << "loading referenced population " << reference << endl;
       AddPopulation(reference);
 
       delete nodePath;
      }
      SyncPopCache();
    }
  } else {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  }
  
}


// StudyInfo::SetFilenameNoLoad
// --------------------------
/*! Sets a new filename, and deletes current DOM representation
  does NOT write old XML tree to disk, use 'Update' prior to this call  to do so
  Both read and write access are checked, nothing is being read even if file exists
  if no write access is established then a error/warning is returned
 */         
void  StudyInfo::SetFilenameNoLoad(std::string filename)
{ 
  m_filename = filename;
  xmlErrorVal error = m_xmlDoc->SetFilenameNoLoad(filename);
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
  CreateBasicEntries();
}
        
// StudyInfo::IsWriteable
// --------------------------
/*!          Returns write access availability (checks physically on file system)        
        Updates the internal access variables for read and write access
 */         
bool StudyInfo::IsWriteable() 
{ 
  return m_xmlDoc->IsWriteable();
}

// StudyInfo::IsValid
// --------------------------
/*!         Returns whether the current XML file is valid
 */         
bool StudyInfo::IsValid()
{
  bool valid = m_xmlDoc->IsValid();
  if (valid && !CheckDocValidity()) {
    valid = false;
  }
  for (unsigned int i = 0; i < m_populationList.size(); i ++) {
    if (valid && ! (m_populationList[i])->IsValid()) {
      valid = false;
    }
  }

  return valid;
}

// StudyInfo::UpdateFile
// --------------------------
/*!          Writes internal DOM to file. Returns error if no write access.
 */         
void StudyInfo::UpdateFile() 
{ 
  m_rootNode->Append(studyXMLkeywords[StudyNumPopulations]);
  m_xmlDoc->SetAttribute(m_rootNode,m_populationList.size());
  m_rootNode->RemoveLast();

  for (unsigned int i = 0; i < m_populationList.size(); i ++) {
    (m_populationList[i])->UpdateFile();
  }
  SyncPopCache();
  xmlErrorVal error = m_xmlDoc->Update();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
}

// StudyInfo::Print
// --------------------------
/*!        Print the current study on the standard out
*/
void StudyInfo::Print()
{
  m_xmlDoc->Print();
  for (unsigned int i = 0; i < m_populationList.size(); i ++) {
    std::cout << "Population : " << i + 1 << ". entry" ; 
    (m_populationList[i])->Print();
  }
}

// StudyInfo::PrintSelf
// --------------------------
/*!        Print the current study on the given stream
*/
void StudyInfo::PrintSelf(std::ostream& os, itk::Indent indent) const
{  
  Superclass::PrintSelf(os,indent);   
  os << indent << "Use Print for printing the whole DOM onto cout " << std::endl;

}

// StudyInfo::GetUserAttribute
// --------------------------
/*!        General read access with deserialize function.  
        Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
        hierarchy node. retvalue is unchanged if the keyword is not found and a warning is returned.
*/ 
void StudyInfo::GetUserAttribute(xmlHierarchy * hierarchy, void * retvalue,  DeSerializeFunc UserFunc) 
{ 
  xmlErrorVal error = m_xmlDoc->GetUserAttribute(hierarchy,retvalue,UserFunc);
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
}
                             

// StudyInfo::SetUserAttribute
// --------------------------
/*!  General write access with serialize function. 
        Finds keyword in DOM tree and sets attribute to retvalue.
        Creates a new keyword/attribute entry if the keyword is not found
        Returns warning if no write access.
*/
void StudyInfo::SetUserAttribute(xmlHierarchy * hierarchy, void * setvalue,  SerializeFunc UserFunc) 
{ 
  xmlErrorVal error = m_xmlDoc->SetUserAttribute(hierarchy,setvalue,UserFunc);
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
}

// StudyInfo::AddPopulation
// --------------------------
/*! Add a new population to the study. Load the population if the file exists.
  returns pointer to created population 
*/
PopulationInfo::Pointer StudyInfo::AddPopulation(std::string filename)
{
  PopulationInfo::Pointer  xmlDoc = PopulationInfo::New();
  xmlDoc->SetFilename(filename);

  const string keystring = GetPopulationCategoryKey(m_populationList.size());
  m_populationList.push_back(xmlDoc);
  m_rootNode->Append(keystring);
  xmlErrorVal  error = m_xmlDoc->CreateCategoryNode(m_rootNode);
  m_rootNode->RemoveLast();
  if (error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
  SyncPopCache();
  return xmlDoc;
}

// StudyInfo::AddPopulationNoLoad
// --------------------------
/*! Add a new population to the study. Do not load the population file, even when it is exists
  returns pointer to created population 
*/ 
PopulationInfo::Pointer StudyInfo::AddPopulationNoLoad(std::string filename)
{
  PopulationInfo::Pointer  xmlDoc = PopulationInfo::New();
  xmlDoc->SetFilenameNoLoad(filename);
  
  const string keystring = GetPopulationCategoryKey(m_populationList.size());
  m_populationList.push_back(xmlDoc);
  m_rootNode->Append(keystring);
  xmlErrorVal error = m_xmlDoc->CreateCategoryNode(m_rootNode);
  m_rootNode->RemoveLast();
  if (error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error));
  } 
  SyncPopCache();
  
  return xmlDoc;
}

// StudyInfo::AddPopulation
// --------------------------
/*! Add a new population without specifying a filename, returns pointer to created population 
*/   
PopulationInfo::Pointer StudyInfo::AddPopulation()
{
  PopulationInfo::Pointer  xmlDoc = PopulationInfo::New();
  m_populationList.push_back(xmlDoc);
  SyncPopCache();

  return xmlDoc;
}


// StudyInfo::GetPopulations
// --------------------------
/*! Returns a list of all populations in the study
*/
const StudyInfo::PopulationListType StudyInfo::GetPopulations()
{
  return m_populationList;
}

// StudyInfo::GetNumberOfPopulations
// --------------------------
/*! Returns the number of population associated with this study
*/
unsigned int StudyInfo::GetNumberOfPopulations()
{
  return m_populationList.size();
}

// StudyInfo::SetStudyID
// --------------------------
/*! Sets the the studyID, keyword StudyID
*/
void StudyInfo::SetStudyID( int studyID)
{
  SetStudyAttribute(StudyID,studyID);
}

// StudyInfo::GetStudyID
// --------------------------
/*! Returns the the studyID, keyword StudyID
*/
int StudyInfo::GetStudyID()
{
  int studyID;
  QueryStudyAttribute(StudyID,studyID);
  return studyID;
}

// StudyInfo::SetStudyName
// --------------------------
/*! Sets the the studyName, keyword StudyName
*/
void StudyInfo::SetStudyName( std::string studyName)
{
  SetStudyAttribute(StudyName,studyName);
}

// StudyInfo::GetStudyName
// --------------------------
/*! Returns the the studyName, keyword StudyName
*/
std::string StudyInfo::GetStudyName()
{
  std::string studyName;
  QueryStudyAttribute(StudyName,studyName);
  return studyName;
}

// StudyInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void StudyInfo::SetStudyAttribute(keywordType keyword, double entry)
{
  m_rootNode->Append(studyXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n SetStudy ") + string(studyXMLkeywords[keyword]));
  } 
}

// StudyInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void StudyInfo::SetStudyAttribute(keywordType keyword, bool entry)
{
  m_rootNode->Append(studyXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n SetStudy ") + string(studyXMLkeywords[keyword]));
  } 
}

// StudyInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void StudyInfo::SetStudyAttribute(keywordType keyword, int  entry)
{
  m_rootNode->Append(studyXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n SetStudy ") + string(studyXMLkeywords[keyword]));
  } 
}

// StudyInfo::Set
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void StudyInfo::SetStudyAttribute(keywordType keyword, string  entry)
{
  m_rootNode->Append(studyXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->SetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n SetStudy ") + string(studyXMLkeywords[keyword]));
  } 
}


// StudyInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void StudyInfo::QueryStudyAttribute(keywordType keyword, double & entry)
{
  m_rootNode->Append(studyXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error) + 
            string(" \n QueryStudy ") + string(studyXMLkeywords[keyword]));
  } 
}

// StudyInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void StudyInfo::QueryStudyAttribute(keywordType keyword, bool & entry)
{
  m_rootNode->Append(studyXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n QueryStudy ") + string(studyXMLkeywords[keyword]));
  } 
}

// StudyInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void StudyInfo::QueryStudyAttribute(keywordType keyword, int & entry)
{
  m_rootNode->Append(studyXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n QueryStudy ") + string(studyXMLkeywords[keyword]));
  } 
}

// StudyInfo::Query
// --------------------------
/*! Returns the queried entry if it exists, no type checking of the answer is performed */
void StudyInfo::QueryStudyAttribute(keywordType keyword, string & entry)
{
  m_rootNode->Append(studyXMLkeywords[keyword]);
  xmlErrorVal error = m_xmlDoc->GetAttribute(m_rootNode,entry);
  m_rootNode->RemoveLast();
  if ( error < XML_NO_ERROR) {
    throw SubjectInfoException(__FILE__, __LINE__, xmlErrors::getErrorString(error)+ 
            string(" \n QueryStudy ") + string(studyXMLkeywords[keyword]));
  } 
}


// ======================================================================================
// ======================================================================================
// ======================================================================================
// protected methods
// ======================================================================================
// ======================================================================================
// ======================================================================================

// StudyInfo::GetRootCategoryNode
// --------------------------
/*!  Returns a copy of the hierarchy that points to the category node that forms the 
  root node of the Stat Study*/
xmlHierarchy * StudyInfo::GetRootCategoryNode()
{
  xmlHierarchy * nodePath  = new xmlHierarchy(); 
  m_xmlDoc->GetRootCategoryNode(nodePath);
  nodePath->Append(studyXMLkeywords[StudyRoot]);
  
  return nodePath;
}

// StudyInfo::CreateBasicEntries
// --------------------------
/*!  Create the basic entries needed for every study*/
void StudyInfo::CreateBasicEntries()
{
  xmlHierarchy * nodePath = GetRootCategoryNode();
  m_xmlDoc->CreateCategoryNode(nodePath);

  nodePath->Append(studyXMLkeywords[StudyVersionName]);
  m_xmlDoc->SetAttribute(nodePath,string(studyXMLkeywords[StudyVersionID]));
  
  nodePath->ReplaceLast(studyXMLkeywords[StudyID]);
  int randomNumber = rand(); 
  m_xmlDoc->SetAttribute(nodePath,randomNumber);

  nodePath->ReplaceLast(studyXMLkeywords[StudyName]);
  string studyName = m_filename;
  m_xmlDoc->SetAttribute(nodePath,studyName);

  // set number of populations to 0
  m_populationList.clear();
  nodePath->ReplaceLast(studyXMLkeywords[StudyNumPopulations]);
  m_xmlDoc->SetAttribute(nodePath,m_populationList.size());
  
  delete nodePath;
}

// StudyInfo::SyncPopCache
// --------------------------
/*!  Synchronize the cached entries of each population associated with this study
 */
void StudyInfo::SyncPopCache()
{
  xmlHierarchy * nodePath = GetRootCategoryNode();
  nodePath->Append(studyXMLkeywords[StudyNumPopulations]);
  m_xmlDoc->SetAttribute(nodePath,m_populationList.size());
  delete nodePath;

  for (unsigned int i = 0; i < m_populationList.size(); i ++) {
    const string reference = (m_populationList[i])->GetFileName();
    const string nodeString = GetPopulationCategoryKey(i);

    nodePath = GetRootCategoryNode();
    nodePath->Append(nodeString);
    nodePath->Append(studyXMLkeywords[StudyPopRef]);
    m_xmlDoc->SetAttribute(nodePath,reference);
  
    int popID = (m_populationList[i])->GetPopulationID();
    nodePath->ReplaceLast(studyXMLkeywords[StudyPopIDCACHE]);
    m_xmlDoc->SetAttribute(nodePath,popID);

    const string popName  = (m_populationList[i])->GetPopulationName();
    nodePath->ReplaceLast(studyXMLkeywords[StudyPopNameCACHE]);
    m_xmlDoc->SetAttribute(nodePath,popName);

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

// StudyInfo::GetPopulationCategoryKey
// --------------------------
/*! Get the XML keyword for the Population Category 
*/
const std::string StudyInfo::GetPopulationCategoryKey(int number)
{
  static string keyString;
  char keyStr[50];
  sprintf(keyStr,"%s_%d",studyXMLkeywords[StudyPopulation],number);
  keyString = string(keyStr);

  return keyString;
}

// StudyInfo::CheckDocValidity
// --------------------------
/*! Check validity of Study file */
bool StudyInfo::CheckDocValidity()
{
  bool valid = true;
  xmlErrorVal error;

  // Is this a Study file
  xmlHierarchy * nodePath = GetRootCategoryNode();
  bool hasStudyRoot = m_xmlDoc->IsCategoryNode(nodePath);
  if (!hasStudyRoot) {
    cout << "No study root entry" << endl;
    valid = false;
  }
  // check version? Not yet necessary, maybe in future

  // check if there is an id
  nodePath->Append(studyXMLkeywords[StudyID]);
  int studyID;
  error = m_xmlDoc->GetAttribute(nodePath,studyID);
  if (error != XML_NO_ERROR) {
    cout << xmlErrors::getErrorString(error) << endl;
    valid = false;
  }

  // is there a name?
  nodePath->ReplaceLast(studyXMLkeywords[StudyName]);
  string studyName;
  error = m_xmlDoc->GetAttribute(nodePath,studyName);
  if (error != XML_NO_ERROR) {
    // no name -> create an unknown name entry
    studyName = string("UNKNOWN");
    m_xmlDoc->SetAttribute(nodePath,studyName);
  }

  // are there any populations
  nodePath->ReplaceLast(studyXMLkeywords[StudyNumPopulations]);
  int numPopulations;
  error = m_xmlDoc->GetAttribute(nodePath,numPopulations);
  if (error != XML_NO_ERROR) {
    // no pops -> create an empty numberPop entry
    m_populationList.clear();
    m_xmlDoc->SetAttribute(nodePath,m_populationList.size());
  }

  delete nodePath;
  return valid;
}
