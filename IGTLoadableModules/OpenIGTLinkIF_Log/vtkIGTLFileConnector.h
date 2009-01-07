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

#include "igtlOSUtil.h"
#include "igtlMessageBase.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"

class vtkMutexLock;


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
  vtkGetMacro( Interval, int );
  vtkSetMacro( Interval, int );
  vtkGetMacro( SpeedFactor, int );
  vtkSetMacro( SpeedFactor, int );
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
  int MessageInterval(void*, int);
  
 private:
  int Interval;
  int SpeedFactor;
  //BTX
  //Create two time stamp objects
  igtl::TimeStamp::Pointer ts1;
  igtl::TimeStamp::Pointer ts2;
  //Create a pointer to the header of the OpenIGTLink message
  igtl::MessageHeader::Pointer headerMsg;
  int numMes;     //number of OpenIGTLink messages read
  //ETX
  
  //----------------------------------------------------------------
  // Thread and File Manager
  //----------------------------------------------------------------
 
 protected:
  virtual void* StartThread();
  vtkMutexLock* Mutex;
 
 private:
  //BTX
  std::string filename;
  ifstream inputFile;
  //ETX
};

#endif // __vtkIGTLFileConnector_h
