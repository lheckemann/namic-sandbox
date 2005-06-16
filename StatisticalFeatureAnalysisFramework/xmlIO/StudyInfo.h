/*!     \file StudyInfo.h
        \author Martin Styner
        
        \brief This class provides functionality for reading/writing
        xmlIO based files describing statistical studies of
        populations consisting each of a set of objects with features
        
        Any referenced populations are read in as well and can be accessed.
    
*/

#ifndef __StudyInfo_H
#define __StudyInfo_H
 
//
// Includes
//
#include <iostream>
#include <vector>
#include <string>

#include "itkDataObject.h"
#include "itkProcessObject.h"

#include "xmlIOlib.h"
#include "PopulationInfo.h"

// Class StudyInfo
// --------------
/*!     \brief  This class provides functionality for reading/writing
        xmlIO based files describing statistical studies of
        populations consisting each of a set of objects with features
        
        Any referenced populations are read in as well and can be accessed.
    
        \verbatim
        simple example:
        StudyInfo::Pointer  xmlDoc = StudyInfo::New();
   try {
     m_xmlDoc->SetFilename("myStudy.xml");
   } catch (itk::ExceptionObject e)  {
          e.Print(std::cout) ;
          exit(-1) ;
        } 
        m_xmlDoc->Print();
        \endverbatim
*/

/** \brief Base exception class for study conflicts. */
class StudyInfoException : public itk::ExceptionObject
{
public:
  /** Run-time information. */
  itkTypeMacro(StudyInfoException , ExceptionObject );
 
  /** Constructor. */
  StudyInfoException(const char *file, unsigned int line,
                           const char * message = "Error in IO") :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
 
  /** Constructor. */
  StudyInfoException(const std::string &file, unsigned int line,
                           const char* message = "Error in IO") :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
  StudyInfoException(const std::string &file, unsigned int line,
                           const std::string message) :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
  StudyInfoException(const char * file, unsigned int line,
                           const std::string message) :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
};
         
class StudyInfo : public itk::DataObject
{
public:
  typedef StudyInfo          Self;
  typedef DataObject  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  itkNewMacro(Self);
  itkTypeMacro(StudyInfo, DataObject);
 
  typedef enum {
    StudyVersionName = 0,
    StudyVersionID, StudyRoot,
    StudyID,StudyName,
    StudyNumPopulations, StudyPopulation,
    StudyPopNameCACHE, StudyPopIDCACHE, StudyPopRef
  } keywordType;

  //Returns the string associated with the keyword
  static std::string GetKeywordString(keywordType keyword);
  
  typedef std::vector<PopulationInfo::Pointer> PopulationListType;
  
  void Initialize();
        
  //! frees all data and associates a new, clean, fresh Study with 
  //!  the current filename (no update is peformed)
  virtual void Clean();
  
  //! Writes internal DOM to file. Returns error if no write access.
  virtual void UpdateFile();
  
  //! Print the current Study description on the standard out
  virtual void Print();
  
  //! Sets the filename, write access is checked
  //! if supplied filename exists then the contents are read and parsed, 
  //!   the current Study is replaced
  //! if supplied filename does not exist, then nothing is read
  //! write access is checked
  virtual void SetFilename(std::string filename);
  
  //! Sets the filename, write access is checked, no loading is performed
  virtual void SetFilenameNoLoad(std::string filename);
  
  //! Returns write access availability (checks physically on file system)        
  // Updates the internal access variables for read and write access
  virtual bool IsWriteable();

  //! Returns whether the current XML file is valid
  virtual bool IsValid();

  //! Sets the the studyID, keyword StudyID
  virtual void SetStudyID(int studyID);
  //! Returns the the studyID, keyword StudyID
  virtual int GetStudyID();
  //! Sets the studyName, keyword StudyName
  virtual void SetStudyName(std::string studyName);
  //! Returns the studyName, keyword StudyName
  virtual std::string GetStudyName();
  //! Returns the current filename of the study xml file
  virtual const std::string GetFileName() { return m_filename; };
  
  //! Returns the queried entry if it exists, no type checking of the answer is performed
  virtual void SetStudyAttribute(keywordType keyword, double entry);
  virtual void SetStudyAttribute(keywordType keyword, bool entry);
  virtual void SetStudyAttribute(keywordType keyword, int entry);
  virtual void SetStudyAttribute(keywordType keyword, std::string entry);
  
  //! Returns the queried entry if it exists, no type checking of the answer is performed
  virtual void QueryStudyAttribute(keywordType keyword, double & entry);
  virtual void QueryStudyAttribute(keywordType keyword, bool & entry);
  virtual void QueryStudyAttribute(keywordType keyword, int & entry);
  virtual void QueryStudyAttribute(keywordType keyword, std::string & entry);

  
  //! Returns the number of population associated with this study
  virtual unsigned int GetNumberOfPopulations();
  //! Returns a list of all populations in the study , the population list should be seen rather as an
  //  unordered set of populations. The order is not guaranteed!
  virtual const PopulationListType GetPopulations(); 
  //! Add a new population to the study, Load the population if  file exists, returns pointer to created population 
  virtual PopulationInfo::Pointer AddPopulation(std::string filename);
  //! Add a new population to the study, Do not load the population file, returns pointer to created population  
  virtual PopulationInfo::Pointer AddPopulationNoLoad(std::string filename);
  //! Add a new population without specifying a filename, returns pointer to created population   
  virtual PopulationInfo::Pointer AddPopulation();
  
  
  //! enables the use of generic attribute nodes, SerializeFunc has to generate the string
  //! serial_string that is the serialized representation of the object referenced by setval
  typedef  xmlIO::SerializeFunc SerializeFunc; 
  //! enables the use of generic attribute nodes, DeSerializeFunc has to generate the object
  //! getvalue that is the deserialized representation of the string serial_string
  typedef  xmlIO::DeSerializeFunc DeSerializeFunc; 
  
  //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
  //!hierrarchy node. USE ONLY IN EXCEPTIONAL CASES!!!
  virtual void GetUserAttribute(xmlHierarchy * hierarchy, void * retvalue, DeSerializeFunc UserFunc);
  
  //!Finds keyword in Study doc  and sets attribute to retvalue. Returns error
  // if no write access. USE ONLY IN EXCEPTIONAL CASES!!!
  virtual void SetUserAttribute(xmlHierarchy * hierarchy, void * setvalue,  SerializeFunc UserFunc);
  
 protected:

  //!Print onto the specified stream
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;
  
  StudyInfo();
  virtual ~StudyInfo();
  
  //!Returns a copy of the hierarchy that points to the category node that forms the root node of the Stat Study
  xmlHierarchy * GetRootCategoryNode();
  
  //! Create the basic entries that are necessary for a study file
  virtual void CreateBasicEntries();
  
  //! Synchronize the cached entries of each population associated with this study
  virtual void SyncPopCache();
  
 private:

  
  StudyInfo(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  static const std::string GetPopulationCategoryKey(int number);
  bool CheckDocValidity();

  PopulationListType m_populationList;
  std::string    m_filename;
  
  xmlIO        * m_xmlDoc;
  xmlHierarchy * m_rootNode;
};

#endif // __StudyInfo_H


