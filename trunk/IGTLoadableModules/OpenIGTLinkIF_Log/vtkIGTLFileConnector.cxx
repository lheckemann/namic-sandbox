/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.
  
  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $
  
==========================================================================*/

#include "vtkObjectFactory.h"

#include <vtksys/SystemTools.hxx>

#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include "vtkImageData.h"

#include "igtlOSUtil.h"
#include "igtlMessageBase.h"
#include "igtlMessageHeader.h"

#include "vtkIGTLCircularBuffer.h"
#include "vtkIGTLFileConnector.h"


vtkStandardNewMacro(vtkIGTLFileConnector);
vtkCxxRevisionMacro(vtkIGTLFileConnector, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
vtkIGTLFileConnector::vtkIGTLFileConnector()
{
  this->filename  = "";
  this->Mutex = vtkMutexLock::New();
  this->Interval = 0;
  this->SpeedFactor = 1;
}

////---------------------------------------------------------------------------
vtkIGTLFileConnector::~vtkIGTLFileConnector()
{
  if (this->Mutex)
    {
    this->Mutex->Delete();
    }
}

//---------------------------------------------------------------------------
void vtkIGTLFileConnector::PrintSelf(ostream& os, vtkIndent indent)
{
}

//---------------------------------------------------------------------------
void* vtkIGTLFileConnector::StartThread()
{
  this->SetState(STATE_WAIT_CONNECTION);
  
  //Create time stamp objects ts1, ts2;
  ts1 = igtl::TimeStamp::New();
  ts2 = igtl::TimeStamp::New();
  //Create a pointer for the OpenIGTLink header
  headerMsg = igtl::MessageHeader::New();
  
  //number of OpenIGTLink messages read
  numMes = 0;
  
  // Communication to data file
  while (!this->GetConnectorStopFlag())
    {
    //vtkErrorMacro("vtkOpenIGTLinkIFLogic::ThreadFunction(): alive.");
    this->Mutex->Lock();
    this->WaitForConnection();
    this->Mutex->Unlock();
    if (this->inputFile.is_open())
      {
      this->SetState(STATE_CONNECTED);
      //vtkErrorMacro("vtkOpenIGTLinkIFLogic::ThreadFunction(): Client Connected.");
      this->ReceiveController();
      this->SetState(STATE_WAIT_CONNECTION);
      }
    }

  //Close file if open
  if (this->inputFile.is_open())
    {
    this->inputFile.close();
    }

  return NULL;
}

//---------------------------------------------------------------------------
int vtkIGTLFileConnector::WaitForConnection()
{
  while (!this->GetConnectorStopFlag())
    {
    if (this->GetType() == TYPE_FILE)
      {
      //Connect to file with data
      this->inputFile.open(filename.c_str(), std::fstream::binary| std::fstream::out);
      if (this->inputFile.fail())    //file not opened
        {
        std::cerr << "Cannot open file" << std::endl;
        break;
        }
      else
        {
        std::cerr << "File " << filename << " opened correctly." << std::endl;
        return 1;
        }
      }
    else
      {
      this->SetConnectorStopFlag(true);
      }
    }

  //Close file if open
  if (this->inputFile.is_open())
    {
    this->inputFile.close();
    }

  //return NULL;
  return 0;
}

//---------------------------------------------------------------------------
int vtkIGTLFileConnector::ReceiveData(void* data, int size, int readFully /*=1*/)
{
  //Check file is open
  if (!this->inputFile.is_open())
    {
    return 0;
    }

  // Receive data from file
  inputFile.read((char*)data, size);
  
  if (inputFile.gcount() == IGTL_HEADER_SIZE) //header message received
    {
    //Calculate the interval between two successive OpenIGTLink messages
    Interval = MessageInterval(data, size);\
    //Pause between time stamps
    igtl::Sleep(Interval/SpeedFactor);
    }
  
  //Return total bytes read
  return inputFile.gcount();
}

//---------------------------------------------------------------------------
int vtkIGTLFileConnector::SendData(int size, unsigned char* data)
{
  
//  if (this->Socket.IsNull())
//    {
//    return 0;
//    }
//  
//  // check if connection is alive
//  if (!this->Socket->GetConnected())
//    {
//    return 0;
//    }
//
//  return this->Socket->Send(data, size);  // return 1 on success, otherwise 0.
  return 0; 

}


//---------------------------------------------------------------------------
int vtkIGTLFileConnector::MessageInterval(void* data, int size)
{
  numMes++;
  std::cerr << "Number of message: " << numMes << std::endl;
  
  // Extract Header
  headerMsg->InitPack();
  memcpy(headerMsg->GetPackPointer(), data, size);
  headerMsg->Unpack();
  
  //Save the timestamp
  if (numMes % 2 == 1)
    headerMsg->GetTimeStamp(ts1);
  else
    headerMsg->GetTimeStamp(ts2);
  
  //Calculate the time interval between two succesive OpenIGTLink messages
  if (numMes == 1)
    {
    Interval = 0;
    }
  else
    {
    Interval = abs(((int)ts1->GetSecond()-(int)ts2->GetSecond())*1000 + ((int)ts1->GetNanosecond()-(int)ts2->GetNanosecond())/1000000);
    std::cerr << "Interval: " << Interval << " ms" << std::endl;
    }
 
  return Interval;
}
