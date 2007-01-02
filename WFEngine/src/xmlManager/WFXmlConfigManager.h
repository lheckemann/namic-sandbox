#ifndef WFXMLCONFIGMANAGER_H_
#define WFXMLCONFIGMANAGER_H_

#include "WFXmlManager.h"

#include <list>
#include <vector>
#include <string>
#include <map>

using namespace WFEngine::nmWFXmlManager;

namespace WFEngine
{
 namespace nmWFXmlConfigManager
 {
  class WFXmlConfigManager : public WFXmlManager
  {
  public:
   static WFXmlConfigManager* New();
   int loadConfigFile(std::string xmlFileName);
   int parseConfig(std::string parentTag);
   
//   typedef std::list< std::vector<std::string,std::string> > tagAttrList;
   typedef std::map<std::string,std::string> myAttrMap;
   
   std::vector<myAttrMap> getAllKnownWorkflows();
   
      
  private:
   int loadXmlFile(std::string xmlFileName);
   WFXmlConfigManager();
   virtual ~WFXmlConfigManager();
   
   typedef WFXmlManager super;
   
   myAttrMap getAllAttributesFromTag(DOMElement* curElem);
   void getParseAllChildesByName(std::string, std::string, std::vector<myAttrMap>&);
  };
 }
}

#endif /*WFXMLCONFIGMANAGER_H_*/
