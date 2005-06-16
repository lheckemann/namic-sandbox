/*!     \file SubjectInfo.h
        \author Martin Styner
        
        \brief This class provides functionality for reading/writing
        xmlIO based files describing subjects  with objects/features
        
    
*/

#ifndef __SubjectInfo_H
#define __SubjectInfo_H
 
//
// Includes
//
#include <iostream>
#include <string>
 
#include "itkDataObject.h"
#include "itkProcessObject.h"

#include "xmlIOlib.h"

// Class SubjectInfo
// --------------
/*!     \brief  This class provides functionality for reading/writing
        xmlIO based files describing subjects with features/objects
    
        \verbatim
        simple example:
        SubjectInfo::Pointer  xmlDoc = SubjectInfo::New();
   try {
     m_xmlDoc->SetFilename("myStudy.xml");
   } catch (itk::ExceptionObject e)  {
          e.Print(std::cout) ;
          exit(-1) ;
        } 
        m_xmlDoc->Print();
        \endverbatim
*/

/** \brief Base exception class for subject conflicts. */
class SubjectInfoException : public itk::ExceptionObject
{
public:
  /** Run-time information. */
  itkTypeMacro(SubjectInfoException , ExceptionObject );
 
  /** Constructor. */
  SubjectInfoException(const char *file, unsigned int line,
                           const char * message = "Error in IO") :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
 
  /** Constructor. */
  SubjectInfoException(const std::string &file, unsigned int line,
                           const char* message = "Error in IO") :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
  SubjectInfoException(const std::string &file, unsigned int line,
                           const std::string message) :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
  SubjectInfoException(const char * file, unsigned int line,
                           const std::string message) :
    ExceptionObject(file, line)
  {
    SetDescription(message);
  }
};
                                                                                             
               
class SubjectInfo : public itk::DataObject
{
public:
  
  typedef SubjectInfo          Self;
  typedef DataObject  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  itkNewMacro(Self);
  itkTypeMacro(SubjectInfo, DataObject);
 
  typedef enum {
    SubjectVersionName = 0,
    SubjectVersionID, SubjectRoot,
    
    SubjectID, SubjectName,
    SubjectAgeVal,SubjectAgeDesc,
    SubjectGenderVal,SubjectGenderDesc,
    SubjectRaceVal,SubjectRaceDesc,
    SubjectGroupVal,SubjectGroupDesc,
    SubjectIQVal,SubjectIQDesc,
    SubjectHeightVal,SubjectHeightDesc,
    SubjectWeightVal,SubjectWeightDesc,
    SubjectAcquisitionDateVal,SubjectAcquisitionDateDesc,
    SubjectMedicationVal,SubjectMedicationDesc,

    SubjectNumObjects,
    Object, ObjectName,ObjectID,
    ObjectBinImgSegRef,
    ObjectOrigSurfSegRef,
    ObjectRigidAlignSurfRef,
    ObjectScaleAlignSurfRef,
    ObjectOrigSPHARMRef,
    ObjectRigidAlignSPHARMRef,
    ObjectScaleAlignSPHARMRef,
    ObjectDistanceMapRef
    
  } keywordType;
  
  //Returns the string associated with the keyword
  static std::string GetKeywordString(keywordType keyword);
  
  typedef std::vector<int> ObjectIDListType;
  
  void Initialize();
  
  //! frees all data and associates a new, clean, fresh Subject with 
  //!  the current filename (no update is peformed)
  virtual void Clean();
  
  //! Writes internal DOM to file. Returns error if no write access.
  virtual void UpdateFile();
  
  //! Print the current Subject description on the standard out
  virtual void Print();
  
  //! Sets the filename, write access is checked
  //! if supplied filename exists then the contents are read and parsed, 
  //!   the current Subject is replaced
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

  //! Sets the the studyID, keyword PopulationSubjectID
  virtual void SetSubjectID(int studyID);
  //! Returns the the studyID, keyword PopulationSubjectID
  virtual int GetSubjectID();
  //! Sets the the studyName, keyword PopulationSubjectName
  virtual void SetSubjectName(std::string name);
  //! Returns the the studyName, keyword PopulationSubjectName
  virtual std::string GetSubjectName();
  //! Returns the current filename of the study xml file
  virtual const std::string GetFileName() { return m_filename; };
  
  //! Returns the queried entry if it exists, no type checking of the answer is performed
  virtual void SetSubjectAttribute(keywordType keyword, double entry);
  virtual void SetSubjectAttribute(keywordType keyword, bool entry);
  virtual void SetSubjectAttribute(keywordType keyword, int entry);
  virtual void SetSubjectAttribute(keywordType keyword, std::string entry);
  
  //! Returns the queried entry if it exists, no type checking of the answer is performed
  virtual void QuerySubjectAttribute(keywordType keyword, double & entry);
  virtual void QuerySubjectAttribute(keywordType keyword, bool & entry);
  virtual void QuerySubjectAttribute(keywordType keyword, int & entry);
  virtual void QuerySubjectAttribute(keywordType keyword, std::string & entry);
  
  //! Returns the number of objects associated with this subject
  virtual unsigned int GetNumberOfObjects();
  //! Returns a list of all object id's, the objectlist should be seen as an
  //  unordered set of object id's. The order is not guaranteed!
  virtual const ObjectIDListType GetObjectIDlist(); 
  //! Add a new object to this subject, returns the objectid of the newly created object
  //  No test is done to prevent duplicates/uniqueness of ID
  virtual int AddObject(int objectID, std::string objectName);
  virtual int AddObject(int objectID);
  virtual int AddObject(std::string objectName); 
  virtual int AddObject();
  
  //! Returns the queried entry if it exists, no type checking of the answer is performed
  virtual void SetObjectAttribute(int objectID, keywordType keyword, double entry);
  virtual void SetObjectAttribute(int objectID, keywordType keyword, bool entry);
  virtual void SetObjectAttribute(int objectID, keywordType keyword, int entry);
  virtual void SetObjectAttribute(int objectID, keywordType keyword, std::string entry);
  
  //! Returns the queried entry if it exists, no type checking of the answer is performed
  virtual void QueryObjectAttribute(int objectID, keywordType keyword, double & entry);
  virtual void QueryObjectAttribute(int objectID, keywordType keyword, bool & entry);
  virtual void QueryObjectAttribute(int objectID, keywordType keyword, int & entry);
  virtual void QueryObjectAttribute(int objectID, keywordType keyword, std::string & entry);

  
  //! enables the use of generic attribute nodes, SerializeFunc has to generate the string
  //! serial_string that is the serialized representation of the object referenced by setval
  typedef  xmlIO::SerializeFunc SerializeFunc; 
  //! enables the use of generic attribute nodes, DeSerializeFunc has to generate the object
  //! getvalue that is the deserialized representation of the string serial_string
  typedef  xmlIO::DeSerializeFunc DeSerializeFunc; 
  
  //!Finds keyword and returns attribute in retvalue. Notifies if specified keyword leads to a 
  //!hierrarchy node. USE WITH CAUTION!
  virtual  void GetUserAttribute(xmlHierarchy * hierarchy, void * retvalue, DeSerializeFunc UserFunc);
  
  //!Finds keyword in Subject doc  and sets attribute to retvalue. Returns error
  // if no write access. USE WITH CAUTION!
  virtual void SetUserAttribute(xmlHierarchy * hierarchy, void * setvalue,  SerializeFunc UserFunc);
  
 protected:
  
  //!Print onto the specified stream
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;
  
  SubjectInfo();
  virtual ~SubjectInfo();
  
  //!Returns a copy of the hierarchy that points to the category node that forms the root node of the Subject
  xmlHierarchy * GetRootCategoryNode();
  
  //! Create the basic entries that are necessary for a study file
  virtual void CreateBasicEntries();

  //! returns the XML keyword for the number-th object
  static const std::string GetObjectCategoryKey(int number);
  
  //! returns the xml hierarchy  for the object with ID objectID
  xmlHierarchy * GetObjectHierarchy(int objectID);
  
 private:

  SubjectInfo(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  bool CheckDocValidity();
  
  std::string    m_filename;
  
  xmlIO        * m_xmlDoc;
  xmlHierarchy * m_rootNode;
};

#endif // __SubjectInfo_H


