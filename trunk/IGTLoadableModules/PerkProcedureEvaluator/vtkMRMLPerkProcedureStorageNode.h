
#ifndef __vtkMRMLPerkProcedureStorageNode_h
#define __vtkMRMLPerkProcedureStorageNode_h

#ifdef WIN32
#include "vtkPerkProcedureEvaluatorWin32Header.h"
#endif


#include "vtkMRML.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLStorageNode.h"



class
VTK_PerkProcedureEvaluator_EXPORT
vtkMRMLPerkProcedureStorageNode
: public vtkMRMLStorageNode
{
  public:
  static vtkMRMLPerkProcedureStorageNode *New();
  vtkTypeMacro( vtkMRMLPerkProcedureStorageNode, vtkMRMLStorageNode );
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual vtkMRMLNode* CreateNodeInstance();

  virtual void ReadXMLAttributes( const char** atts );

  virtual int ReadData( vtkMRMLNode *refNode );
  virtual int WriteData( vtkMRMLNode *refNode );
  
  virtual void WriteXML( ostream& of, int indent );

  /// Description:
  /// Set dependencies between this node and the parent node
  /// when parsing XML file
  virtual void ProcessParentNode( vtkMRMLNode *parentNode );

  virtual void Copy( vtkMRMLNode *node );
  
  /// 
  /// Get node XML tag name (like Storage, Model)
  virtual const char* GetNodeTagName()  {
    return "PerkProcedureStorage";
  };

  /// 
  /// Check to see if this storage node can handle the file type in the input
  /// string. If input string is null, check URI, then check FileName. 
  /// Subclasses should implement this method.
  virtual int SupportedFileType( const char *fileName );

  /// 
  /// Initialize all the supported write file types
  virtual void InitializeSupportedWriteFileTypes();

  /// 
  /// Return a default file extension for writing
  virtual const char* GetDefaultWriteFileExtension()
    {
    return "xml";
    };
  
  
protected:


  vtkMRMLPerkProcedureStorageNode();
  ~vtkMRMLPerkProcedureStorageNode();
  vtkMRMLPerkProcedureStorageNode( const vtkMRMLPerkProcedureStorageNode& );
  void operator=( const vtkMRMLPerkProcedureStorageNode& );
  
};

#endif
