/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkIGTLConnector_h
#define __vtkIGTLConnector_h

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <set>

#include "vtkObject.h"
#include "vtkOpenIGTLinkIFWin32Header.h" 

class vtkMultiThreader;
class vtkMutexLock;

class vtkImageData;
class vtkMatrix4x4;

class vtkIGTLCircularBuffer;
class vtkMRMLNode;

class VTK_OPENIGTLINKIF_EXPORT vtkIGTLConnector : public vtkObject
{
 public:  
  //----------------------------------------------------------------
  // Constants Definitions
  //----------------------------------------------------------------

  //BTX
  static const int TYPE_NOT_DEFINED = 0;
  
  enum {
    STATE_OFF,
    STATE_WAIT_CONNECTION,
    STATE_CONNECTED,
    NUM_STATE
  };

  enum {
    IO_UNSPECIFIED = 0x00,
    IO_INCOMING   = 0x01,
    IO_OUTGOING   = 0x02,
  };
  //ETX
  
  //BTX
  typedef struct {
    std::string   name;
    std::string   type;
    int           io;
    //vtkMRMLNode*  node;
  } DeviceInfoType;

  typedef std::map<int, DeviceInfoType> DeviceInfoMapType;   // Device list:  index is referred as
                                                              // a device id in the connector.
  typedef std::set<int> DeviceIDSetType;
  //ETX

  //----------------------------------------------------------------
  // VTK functions and macros
  //----------------------------------------------------------------

 public:
  
  static vtkIGTLConnector *New();
  vtkTypeRevisionMacro(vtkIGTLConnector,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetMacro( Type, int );
  vtkSetMacro( Type, int );
  vtkGetMacro( State, int );
  vtkSetMacro( State, int );
  vtkGetMacro( ConnectorStopFlag, int );
  vtkSetMacro( ConnectorStopFlag, int );
  vtkSetMacro( RestrictDeviceName, int );
  vtkGetMacro( RestrictDeviceName, int );

  //----------------------------------------------------------------
  // Data access functions
  //----------------------------------------------------------------
  
  //BTX
  void SetName (const char* str) { this->Name = str; }
  void SetName (std::string str) { this->Name = str; }
  const char* GetName() { return this->Name.c_str(); }
  
  //----------------------------------------------------------------
  // Save to File Functions
  //----------------------------------------------------------------
  
  void PrepareToLogData();
  void LogData(char*, int);
  void StopLoggingData();
  void GenerateUniqueFilename(std::string&, std::string, std::string);
  bool logData;
  std::string filename;
  ofstream outputFile;
  std::map<std::string,std::string> logMap;
  std::map<std::string,std::string>::iterator iter;
  //ETX

  //----------------------------------------------------------------
  // Constructor and Destructor
  //----------------------------------------------------------------

protected:
  vtkIGTLConnector();
  ~vtkIGTLConnector();

public:

  //----------------------------------------------------------------
  // Thread Control
  //----------------------------------------------------------------

  int Start();
  int Stop();
  static void* ThreadFunction(void* ptr);

  //----------------------------------------------------------------
  // OpenIGTLink Message handlers
  //----------------------------------------------------------------
  
  virtual void* StartThread() { return NULL; }
  virtual int WaitForConnection() {return 0; }
  int ReceiveController();
  virtual int ReceiveData(void*, int, int skipFully=1) { return 0; }
  virtual int SendData(int size, unsigned char* data) { return 0;}
  virtual int Skip(int length, int skipFully=1);

  //----------------------------------------------------------------
  // Circular Buffer
  //----------------------------------------------------------------

  //BTX
  typedef std::vector<std::string> NameListType;
  int GetUpdatedBuffersList(NameListType& nameList);
  vtkIGTLCircularBuffer* GetCircularBuffer(std::string& key);
  //ETX

  //----------------------------------------------------------------
  // Device Lists
  //----------------------------------------------------------------

  int GetDeviceID(const char* deviceName, const char* deviceType);
  int RegisterNewDevice(const char* deviceName, const char* deviceType, int io=IO_UNSPECIFIED);
  int UnregisterDevice(const char* deviceName, const char* deviceType, int io=IO_UNSPECIFIED);
  int UnregisterDevice(int id);
  int RegisterDeviceIO(int id, int io);

  //BTX
  DeviceInfoType*     GetDeviceInfo(int id);
  DeviceInfoMapType*  GetDeviceInfoList()    { return &(this->DeviceInfoList);        };
  DeviceIDSetType*    GetIncomingDevice()    { return &(this->IncomingDeviceIDSet);   }
  DeviceIDSetType*    GetOutgoingDevice()    { return &(this->OutgoingDeviceIDSet);   }
  DeviceIDSetType*    GetUnspecifiedDevice() { return &(this->UnspecifiedDeviceIDSet);}
  //ETX

 private:
  //----------------------------------------------------------------
  // Connector configuration 
  //----------------------------------------------------------------
  //BTX
  std::string Name;
  //ETX
  int Type;
  int State;

  //----------------------------------------------------------------
  // Thread and Socket
  //----------------------------------------------------------------

  vtkMultiThreader* Thread;

  int               ThreadID;
  int               ConnectorStopFlag;

  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------

  //BTX
  typedef std::map<std::string, vtkIGTLCircularBuffer*> CircularBufferMap;
  CircularBufferMap Buffer;
  //ETX

  vtkMutexLock* CircularBufferMutex;
  
  int     RestrictDeviceName;  // Flag to restrict incoming and outgoing data by device names
  //BTX
  // -- Device Name (same as MRML node) and data type (data type string defined in OpenIGTLink)
  int                LastID;
  DeviceInfoMapType DeviceInfoList;

  DeviceIDSetType   IncomingDeviceIDSet;
  DeviceIDSetType   OutgoingDeviceIDSet;
  DeviceIDSetType   UnspecifiedDeviceIDSet;

  //ETX

};

#endif // __vtkIGTLConnector_h
