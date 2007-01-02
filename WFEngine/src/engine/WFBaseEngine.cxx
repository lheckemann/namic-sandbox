#include "WFBaseEngine.h"

#include <iostream>
#include <fstream>

#include "WFSocketCollection.h"

#define WFCLIENT_INTF 1
#define WFOPTIONS_INTF 2

using namespace std;

using namespace WFEngine;
//{

WFBaseEngine* WFBaseEngine::m_wfeBE = 0;

WFBaseEngine::WFBaseEngine()
{
 
 //initialize static variables
 //using namespace WFFactoryNameSpace;
 //WFFactory *WFFactory::m_wfFactory;
}

WFBaseEngine::~WFBaseEngine()
{
}

WFBaseEngine* WFBaseEngine::New(std::string wfConfigFile, bool showEditGUI)
{
 
 //
 bool configExists = false;

 if(wfConfigFile == "")
 {
  fstream fs_op("wfConfig.xml",ios::in);
  if(!fs_op)
  {
   cout<<"wfConfig.xml not found!"<<endl;
  }
  else
  {
   cout<<"wfConfig.xml found!"<<endl;
   wfConfigFile = "wfConfig.xml";
   configExists = true;
  }
  fs_op.close();
 }
 else
 {
  cout<<wfConfigFile<<endl;
  
  fstream fs_op(wfConfigFile.c_str(), ios::in);
  if(!fs_op)
  {
   cout<<wfConfigFile<<" could not be found!"<<endl;
  }
  else configExists = true;
  
  fs_op.close();
 }
 
 m_wfeBE = new WFBaseEngine();
 
 if(!configExists)
 {
  const string emptyStr = "";
  m_wfeBE->m_wfeOpts = m_wfeBE->InitializeWFEOpts(emptyStr, true);
 }
 else
 {
  m_wfeBE->m_wfeOpts = m_wfeBE->InitializeWFEOpts(wfConfigFile, showEditGUI);
 }
 
// wfBE->InitializeWFFactoryClasses();
 
 m_wfeBE->mainInterfaceLoop();
 
 return m_wfeBE;
}

nmWFEngineOptions::WFEngineOptions *WFBaseEngine::InitializeWFEOpts(string wfConfigFile, bool showEditGUI)
{
 using namespace nmWFEngineOptions;
 WFEngineOptions *wfeOpts = WFEngineOptions::New();
 wfeOpts->SetConfigFile(wfConfigFile);
 wfeOpts->SetShowEditor(showEditGUI);
}

//void WFBaseEngine::InitializeWFFactoryClasses()
//{
// using namespace nmWFFactory;
// using namespace nmWFSocketHandler;
// using namespace nmWFClientInterface;
// 
// this->m_wfeFactory = WFFactory::GetInstance();
//
// WFSocketHandler wfSH;
// WFClientInterface wfCI;
// m_wfeFactory->registerType(wfSH, wfCI);
// //m_Factory->registerTypes();
//}

void WFBaseEngine::mainInterfaceLoop()
{
 using namespace nmWFServerConnection;
 
 WFEngine::nmWFSocketCollection::WFSocketCollection sc;
 
 //add Workflow Connection
 WFServerConnection *m_wfeSC = WFServerConnection::New();
 
 m_wfeSC->setPort(6867);
 m_wfeSC->bindAndListen();
 
 sc.addSocket(m_wfeSC);
 
 //add Options Connection
 m_wfeSC = WFServerConnection::New();
 
 m_wfeSC->setPort(6868);
 m_wfeSC->setMaxConnections(2);
 m_wfeSC->bindAndListen();
 
 sc.addSocket(m_wfeSC);
 
 
 //setup Client Connections
 //this->m_wfeCH->addConnection(6867,5, &WFBaseEngine::recvClientData);
 
// //setup Option Conection
// this->m_wfeCH->addConnection(6868,1, &WFBaseEngine::recvOptionsData);
// 
////setup Option Conection
// this->m_wfeCH->addConnection(6869,1, &WFBaseEngine::recvOptionsData);
 
 sc.setTimeOut(1);
 while(true)
 {
  sc.selectOnSockets();
 }
// m_wfeCLI = WFClientInterface::New(6867, 5, &WFBaseEngine::recvClientData);
 
 
}

void WFBaseEngine::recvClientData(int socket, char* buffer)
{
 if(buffer == NULL) return;
 
 std::cout<<"recvClientData:"<<std::endl;
 std::cout<<buffer<<std::endl;
 
 //int length = sizeof("acknowledgment")+1;
 //char sendData[length];
 std::string sendData = "acknowledgment";
 m_wfeBE->sendClientData(socket, sendData.c_str());
}

void WFBaseEngine::recvOptionsData(int socket, char* buffer)
{
 if(buffer == NULL) return;
 
 std::cout<<"recvClientData:"<<std::endl;
 std::cout<<buffer<<std::endl;
 
 //int length = sizeof("acknowledgment")+1;
 //char sendData[length];
 std::string sendData = "acknowledgment";
 m_wfeBE->sendClientData(socket, sendData.c_str());
}

void WFBaseEngine::sendClientData(int socket, std::string buffer)
{
 this->m_wfeSC->sendDataToConnection(socket, buffer);
}

//}
