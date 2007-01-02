#include "WFXmlConfigManager.h"

#include <iostream>

using namespace WFEngine::nmWFXmlConfigManager;
//need to recognize the XERCESC types
XERCES_CPP_NAMESPACE_USE;


WFXmlConfigManager::WFXmlConfigManager()
{
}

WFXmlConfigManager::~WFXmlConfigManager()
{
}

int WFXmlConfigManager::loadConfigFile(std::string xmlFileName)
{
 return this->loadXmlFile(xmlFileName);
}

int WFXmlConfigManager::loadXmlFile(std::string xmlFileName)
{
 int retval = 0;
 retval = initializeXerces(xmlFileName);
 if(retval != 1)
 {
  //DOMNode *myXMLConfigFile = this->xmlParser->getDocument();
  DOMElement *myRootElem = this->xmlDoc->getDocumentElement();
  //  std::cout<<myRootElem->getTagName()<<std::endl;
 }
 return retval;
}

WFXmlConfigManager* WFXmlConfigManager::New()
{
 return new WFXmlConfigManager();
}

WFXmlConfigManager::myAttrMap WFXmlConfigManager::getAllAttributesFromTag(DOMElement *curElem)
{
 WFXmlConfigManager::myAttrMap curAttrMap;
 DOMNamedNodeMap *attrMap = curElem->getAttributes();
 for(int i = 0; i < attrMap->getLength(); i++)
 {
  std::cout<<XMLString::transcode(attrMap->item(i)->getNodeName())<<"='"<<XMLString::transcode(attrMap->item(i)->getNodeValue())<<"'"<<std::endl;
 }
 return curAttrMap;
}

std::vector<WFXmlConfigManager::myAttrMap> WFXmlConfigManager::getAllKnownWorkflows()
{
 std::vector<myAttrMap> myKnownWFList;
 this->getParseAllChildesByName("knownWorkflows", "workflow", myKnownWFList);
 return myKnownWFList;
}

void WFXmlConfigManager::getParseAllChildesByName(std::string parentTagName, std::string childTagName, std::vector<WFXmlConfigManager::myAttrMap> &myChildList)
{
 DOMNodeList *myKnownWFNL = this->xmlDoc->getDocumentElement()->getElementsByTagName(XMLString::transcode(parentTagName.c_str()));
 std::cout<<myKnownWFNL->getLength()<<" <"<<parentTagName<<"> Length"<<std::endl;
 
 if(myKnownWFNL->getLength() > 0)
 {
  DOMNode *myKnownWFNode = myKnownWFNL->item(0);
  //check if the node type is an ElementNode
  if (myKnownWFNode->getNodeType() == 1)
  {
   DOMElement *curElem = (DOMElement*)myKnownWFNode;
   DOMNodeList *childNodes = curElem->getElementsByTagName(XMLString::transcode(childTagName.c_str()));
   for(int i = 0; i < childNodes->getLength(); i++)
   {
    if (childNodes->item(i)->getNodeType() == 1)
    {
     myChildList.push_back(this->getAllAttributesFromTag((DOMElement*)(childNodes->item(i))));
    }
   }
  }
 }
}
