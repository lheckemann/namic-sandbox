#ifndef WFENGINE_H_
#define WFENGINE_H_

#include "WFEngineOptions.h"
#include "WFServerConnection.h"

#include <string>

namespace WFEngine
{
 class WFBaseEngine
 {
 public:
  static WFBaseEngine* New(const std::string wfConfigFile, bool showEditGUI);
 protected:
 
  WFBaseEngine();
  virtual ~WFBaseEngine();
 
  nmWFEngineOptions::WFEngineOptions * InitializeWFEOpts(std::string wfConfigFile, bool showEditGUI);
  void InitializeWFFactoryClasses();
  void InitializeWFInterfaces();
 
 private:
  //static string wfConfigFile;
  nmWFEngineOptions::WFEngineOptions *m_wfeOpts;
 
  static void recvClientData(int socket, char* buffer);
  static void recvOptionsData(int socket, char* buffer);
  void sendClientData(int socket, std::string sendData);
  void sendOptionsData(int socket, std::string sendData);
//  nmWFFactory::WFFactory *m_wfeFactory;
  
//  nmWFClientInterface::WFClientInterface *m_wfeCLI;
  
  static WFBaseEngine *m_wfeBE;
  
  void mainInterfaceLoop();
  
  //ConnectionHandler is the main Handler for all server connections
  nmWFServerConnection::WFServerConnection *m_wfeSC;
 };
}

#endif /*WFENGINE_H_*/
