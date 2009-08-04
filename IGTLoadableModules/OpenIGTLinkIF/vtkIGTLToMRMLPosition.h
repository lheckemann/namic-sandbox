/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLPosition.h $
  Date:      $Date: 2009-01-05 13:28:20 -0500 (Mon, 05 Jan 2009) $
  Version:   $Revision: 8267 $

==========================================================================*/

#ifndef __vtkIGTLToMRMLPosition_h
#define __vtkIGTLToMRMLPosition_h

#include "vtkObject.h"
#include "vtkOpenIGTLinkIFWin32Header.h" 
#include "vtkMRMLNode.h"
#include "vtkIGTLToMRMLBase.h"

#include "igtlPositionMessage.h"

class VTK_OPENIGTLINKIF_EXPORT vtkIGTLToMRMLPosition : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLPosition *New();
  vtkTypeRevisionMacro(vtkIGTLToMRMLPosition,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "POSITION"; };
  virtual const char*  GetMRMLName() { return "LinearTransform"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  //BTX
  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  //ETX
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);


 protected:
  vtkIGTLToMRMLPosition();
  ~vtkIGTLToMRMLPosition();

 protected:
  //BTX
  igtl::PositionMessage::Pointer OutPositionMsg;
  //ETX
  
};


#endif //__vtkIGTLToMRMLPosition_h
