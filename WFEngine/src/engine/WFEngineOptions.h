#ifndef WFENGINEOPTIONS_H_
#define WFENGINEOPTIONS_H_

#include "WFXmlConfigManager.h"

#include <string>
#include <list>
#include <map>

namespace WFEngine
{
 
 namespace nmWFEngineOptions
 {
  class WFEngineOptions
  {
  public:
   typedef struct wfDescription
   {
    std::string fileName;
    bool visible;
    bool embedded;
   } wfDesc;
   
   //static WFEngineOptions *New(const string wfConfigFile, bool showEditGUI);
   static WFEngineOptions *New();
 
   void SetConfigFile(std::string wfConfigFile);
   std::string GetConfigFile();
 
   void SetShowEditor(bool);
  
  protected:
   WFEngineOptions();
   virtual ~WFEngineOptions();
 
  private:
   WFEngine::nmWFXmlConfigManager::WFXmlConfigManager *m_WFCfgXmlManager;
 
   wfDescription m_wfDesc;
 
   std::map<int, struct wfDescription> m_knownWFs;
 
   int loadConfigFile(std::string wfConfigFile);
 
   bool m_configIsActive;
   std::string m_wfConfigFile;
   
   void recvOptionsData(char* buffer);
   
  };
 }

}
#endif /*WFENGINEOPTIONS_H_*/
