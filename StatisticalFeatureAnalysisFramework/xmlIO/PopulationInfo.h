/*!     \file PopulationInfo.h
        \author Martin Styner
        
        \brief This class provides functionality for reading/writing
        xmlIO based files describing populations consisting of a set of subjects with objects/features
        
        Any referenced subjects are read in as well and can be accessed.
    
*/

#ifndef __PopulationInfo_H
#define __PopulationInfo_H
 
//
// Includes
//
#include <iostream>
#include <string>
 
#include "itkDataObject.h"
#include "itkProcessObject.h"
#include "itkExceptionObject.h"


#include "xmlIOlib.h"
#include "SubjectInfo.h"

// Class PopulationInfo
// --------------
/*!     \brief  This class provides functionality for reading/writing
        xmlIO based files describing populations consisting each of a set of objects with features
        
        Any referenced object describtions are read in as well and can be accessed.
    
        \verbatim
        simple example:
        PopulationInfo::Pointer  xmlDoc = PopulationInfo::New();
   try {
     m_xmlDoc->SetFilename("myStudy.xml");
   } catch (itk::ExceptionObject e)  {
          e.Print(std::cout) ;
          exit(-1) ;
        } 
        m_xmlDoc->Print();
        \endverbatim
*/
 
/** \brief Base exception class for population conflicts. */
class PopulationInfoException : public itk::ExceptionObject
{
public:
  /** Run-time information. */
  itkTypeMacro(PopulationInfoException , ExceptionObject );
 
  /** Constructor. */
  PopulationInfoException(const char *file, unsigned int line,
                           const char * message = "Error in IO") :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
 
  /** Constructor. */
  PopulationInfoException(const std::string &file, unsigned int line,
                           const char* message = "Error in IO") :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
    
  PopulationInfoException(const std::string &file, unsigned int line,
                           const std::string message) :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
  PopulationInfoException(const char * file, unsigned int line,
                           const std::string message) :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
};
                                                                                             
                       

class PopulationInfo : public itk::DataObject
{
public:
  
  typedef PopulationInfo          Self;
  typedef DataObject  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  itkNewMacro(Self);
  itkTypeMacro(PopulationInfo, DataObject);
 
  typedef enum {
    PopulationVersionName = 0,
    PopulationVersionID, PopulationRoot,
    PopulationID, PopulationName,
    PopulationNumSubjects, PopulationSubject,
    PopulationSubjectNameCACHE, PopulationSubjectIDCACHE, PopulationSubjectRef
  } keywordType;

  //Returns the string associated with the keyword
  static std::string GetKeywordString(keywordType keyword);
  
  typedef std::vector<SubjectInfo::Pointer> SubjectListType;
  
  void Initialize();
  
  //! frees all data and associates a new, clean, fresh Population with 
  //!  the current filename (no update is peformed)
  virtual void Clean();
  
  //! Writes internal DOM to file. Returns error if no write access.
  virtual void UpdateFile();
  
  //! Print the current Population description on the standard out
  virtual void Print();
  
  //! Sets the filename, write access is checked
  //! if supplied filename exists then the contents are read and parsed, 
  //!   the current Population is replaced
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

  //! Sets the the populationID, keyword PopulationID
  virtual void SetPopulationID(int popID);
  //! Returns the the populationID, keyword PopulationID
  virtual int GetPopulationID();
  //! Sets the the populationName, keyword PopulationName
  virtual void SetPopulationName(std::string name);
  //! Returns the the populationName, keyword PopulationName
  virtual std::string GetPopulationName();
  //! Returns the current filename of the population xml file
  virtual const std::string GetFileName() { return m_filename; };
  
  //! Returns the queried entry if it exists, no type checking of the answer is performed
  virtual void SetPopulationAttribute(keywordType keyword, double entry);
  virtual void SetPopulationAttribute(keywordType keyword, bool entry);
  virtual void SetPopulationAttribute(keywordType keyword, int entry);
  virtual void SetPopulationAttribute(keywordType keyword, std::string entry);
  
  //! Returns the queried entry if it exists, no type checking of the answer is performed
  virtual void QueryPopulationAttribute(keywordType keyword, double & entry);
  virtual void QueryPopulationAttribute(keywordType keyword, bool & entry);
  virtual void QueryPopulationAttribute(keywordType keyword, int & entry);
  virtual void QueryPopulationAttribute(keywordType keyword, std::string & entry);
  
  //! Returns the number of subject associated with this population
  virtual unsigned int GetNumberOfSubjects();
  //! Returns a list of all subjects in the population , the subjectlist should be seen rather as an
  //  unordered set of populations. The order is not guaranteed!
  virtual const SubjectListType GetSubjects(); 
  //! Add a new subject to the population, Load subject if file exists, returns pointer to created subject  
  virtual SubjectInfo::Pointer AddSubject(std::string filename);
  //! Add a new subjectto the population, Do not load the subjectfile, returns pointer to created subject 
  virtual SubjectInfo::Pointer AddSubjectNoLoad(std::string filename);
  //! Add a new subject without specifying a filename, returns pointer to created subject   
  virtual SubjectInfo::Pointer AddSubject();
  
  //! enables the use of generic attribute nodes, SerializeFunc has to generate the string
  //! serial_string that is the serialized representation of the object referenced by setval
  typedef  xmlIO::SerializeFunc SerializeFunc; 
  //! enables the use of generic attribute nodes, DeSerializeFunc has to generate the object
  //! getvalue that is the deserialized representation of the string serial_string
  typedef  xmlIO::DeSerializeFunc DeSerializeFunc; 
  
  //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
  //!hierrarchy node. USE WITH CAUTION!
  virtual void GetUserAttribute(xmlHierarchy * hierarchy, void * retvalue, DeSerializeFunc UserFunc);
  
  //!Finds keyword in Population doc  and sets attribute to retvalue. Returns error
  // if no write access. USE WITH CAUTION!
  virtual void SetUserAttribute(xmlHierarchy * hierarchy, void * setvalue,  SerializeFunc UserFunc);

  
 protected:
  
  //!Print onto the specified stream
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;
  
  PopulationInfo();
  virtual ~PopulationInfo();
  
  //!Returns a copy of the hierarchy that points to the category node that forms the root node of the Population
  xmlHierarchy * GetRootCategoryNode();
  
  //! Create the basic entries that are necessary for a population file
  virtual void CreateBasicEntries();
  
  //! Synchronize the cached entries of each subject associated with this population
  virtual void SyncSubjectCache();

  //! returns the xml keyword for the number-th subject
  static const std::string GetSubjectCategoryKey(int number);
  
 private:

  PopulationInfo(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  bool CheckDocValidity();
  
  SubjectListType m_subjectList;
  std::string    m_filename;
  
  xmlIO        * m_xmlDoc;
  xmlHierarchy * m_rootNode;
};

#endif // __PopulationInfo_H


