/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkIGTLToMRMLBrpRobotCommand_h
#define __vtkIGTLToMRMLBrpRobotCommand_h

#include "vtkObject.h"
#include "vtkOpenIGTLinkIFWin32Header.h" 
#include "vtkMRMLNode.h"
#include "vtkIGTLToMRMLBase.h"

#include "igtlHeaderMessage.h"

class VTK_OPENIGTLINKIF_EXPORT vtkIGTLToMRMLBrpRobotCommand : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLBrpRobotCommand *New();
  vtkTypeRevisionMacro(vtkIGTLToMRMLBrpRobotCommand,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual int          GetConverterType() { return TYPE_MULTI_IGTL_NAMES; };
  virtual const char*  GetIGTLName() { return NULL; };
  virtual const char*  GetMRMLName() { return NULL; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  //BTX
  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  //ETX
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);

 
 protected:
  vtkIGTLToMRMLBrpRobotCommand();
  ~vtkIGTLToMRMLBrpRobotCommand();

 protected:
  //BTX
  std::string ZFrameTransformNodeID;
  igtl::HeaderMessage::Pointer OutgoingMsg;
  //ETX
  
};


#endif //__vtkIGTLToMRMLBrpRobotCommand_h
