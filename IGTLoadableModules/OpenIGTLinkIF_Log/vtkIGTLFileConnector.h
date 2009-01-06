/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

//Things to add

#ifndef __vtkIGTLFileConnector_h
#define __vtkIGTLFileConnector_h

#include <string>
#include <map>
#include <vector>
#include <fstream>

#include "vtkObject.h"
#include "vtkOpenIGTLinkIFWin32Header.h" 
#include "vtkIGTLConnector.h"


class VTK_OPENIGTLINKIF_EXPORT vtkIGTLFileConnector : public vtkIGTLConnector
{
 public:

  static vtkIGTLFileConnector *New();
  vtkTypeRevisionMacro(vtkIGTLFileConnector, vtkIGTLConnector);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //BTX
  static const int TYPE_FILE = 3;

  void SetFilename(std::string fn){ filename = fn; }
  void GetFilename(std::string& fn) { fn = filename; } 
  const char* GetFilename() {return filename.c_str(); }
  //ETX

  //----------------------------------------------------------------
  // Constructor and Destructor
  //----------------------------------------------------------------
 
 protected:
  vtkIGTLFileConnector();
  ~vtkIGTLFileConnector();
  
  //----------------------------------------------------------------
  // OpenIGTLink Message handlers
  //----------------------------------------------------------------
  
  virtual int WaitForConnection();
  virtual int ReceiveData(void*, int, int readFully =1);
  virtual int SendData(int size, unsigned char* data);
  
  //----------------------------------------------------------------
  // Thread and Socket
  //----------------------------------------------------------------
  
  virtual void* StartThread();
 
 private:
  //BTX
  std::string filename;
  //ETX
};

#endif // __vtkIGTLFileConnector_h
