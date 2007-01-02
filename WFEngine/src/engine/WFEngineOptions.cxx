#include "WFEngineOptions.h"
#include <iostream>
//#include "GUI/WFConfigEditor.h"

using namespace std;

using namespace WFEngine::nmWFEngineOptions;

WFEngineOptions::WFEngineOptions()
{
 this->m_configIsActive = false;
}

WFEngineOptions::~WFEngineOptions()
{
}

WFEngineOptions* WFEngineOptions::New()
{
 WFEngineOptions *wfeOpts = new WFEngineOptions;

 cout<<"WFConfigFile needs to be load!"<<endl;
 //if(!wfeOpts->loadConfigFile(wfConfigFile)) showEditGUI = true;
 
 //if(showEditGUI)
 //{
  //create Config GUI for WFEngine
//  const WFConfigEditor *wfConfigGUI = WFConfigEditor::New(wfeOpts);
 //}
 return wfeOpts;
}

int WFEngineOptions::loadConfigFile(string wfConfigFile)
{
 int retVal = 0;
 using namespace nmWFXmlConfigManager;
 this->m_WFCfgXmlManager = WFXmlConfigManager::New();
 retVal = this->m_WFCfgXmlManager->loadConfigFile(wfConfigFile);
 if (retVal != 1)
 {
  //load Config into interal data structure
//  retVal = this->m_WFCfgXmlManager
  //get all known Workflows
  std::vector<WFXmlConfigManager::myAttrMap> knownWFs;
  knownWFs = this->m_WFCfgXmlManager->getAllKnownWorkflows();
  std::cout<<knownWFs.size()<<std::endl;
 }
}

void WFEngineOptions::SetConfigFile(string wfConfigFile)
{
 if(this->loadConfigFile(wfConfigFile))
 {
  this->m_wfConfigFile = wfConfigFile;
  this->m_configIsActive = true;
 }
}

string WFEngineOptions::GetConfigFile()
{
 return this->m_wfConfigFile;
}

void WFEngineOptions::SetShowEditor(bool showEditGUI)
{
 
}
