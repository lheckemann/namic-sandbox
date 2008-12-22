/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkIGTLServerClientConnector_h
#define __vtkIGTLServerClientConnector_h

#include <string>
#include <map>
#include <vector>
#include <set>

#include "vtkObject.h"
#include "vtkOpenIGTLinkIFWin32Header.h" 
#include "igtlServerSocket.h"
#include "igtlClientSocket.h"
#include "vtkIGTLConnector.h"

class vtkMultiThreader;
class vtkMutexLock;

class vtkImageData;
class vtkMatrix4x4;

class vtkIGTLCircularBuffer;
class vtkMRMLNode;

class VTK_OPENIGTLINKIF_EXPORT vtkIGTLServerClientConnector : public vtkIGTLConnector
{

 public:  
  //----------------------------------------------------------------
  // Constants Definitions
  //----------------------------------------------------------------
 
  //BTX
  static const int TYPE_SERVER = 1;
  static const int TYPE_CLIENT = 2;
  //ETX
  
  //----------------------------------------------------------------
  // VTK Functions and Macros
  //----------------------------------------------------------------
  
  static vtkIGTLServerClientConnector* New();
  vtkTypeRevisionMacro(vtkIGTLServerClientConnector,vtkIGTLConnector);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  vtkGetMacro( ServerPort, int );
  vtkSetMacro( ServerPort, int );

  //BTX
  void SetServerHostname(const char* str) { this->ServerHostname = str; }
  void SetServerHostname(std::string str) { this->ServerHostname = str; }
  const char* GetServerHostname() { return this->ServerHostname.c_str(); }

  //----------------------------------------------------------------
  // Connector configuration
  //----------------------------------------------------------------

  int SetTypeServer(int port);
  int SetTypeClient(char* hostname, int port);
  //BTX
  int SetTypeClient(std::string hostname, int port);
  //ETX

  
  //----------------------------------------------------------------
  // Constructor and Destructor
  //----------------------------------------------------------------

protected:
  vtkIGTLServerClientConnector();
  ~vtkIGTLServerClientConnector();

  //----------------------------------------------------------------
  // OpenIGTLink Message handlers
  //----------------------------------------------------------------
  
  virtual int WaitForConnection();
  virtual int ReceiveData(void*, int, int readFully =1);
  virtual int SendData(int size, unsigned char* data);

 private:
  //----------------------------------------------------------------
  // Thread and Socket
  //----------------------------------------------------------------
  
  virtual void* StartThread();
  vtkMutexLock* Mutex;
  //BTX
  igtl::ServerSocket::Pointer  ServerSocket;
  igtl::ClientSocket::Pointer  Socket;
  //ETX
  int               ServerPort;
  //BTX
  std::string       ServerHostname;
  //ETX

  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------

};

#endif // __vtkIGTLServerClientConnector_h
