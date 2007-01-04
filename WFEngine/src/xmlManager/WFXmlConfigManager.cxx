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

int WFXmlConfigManager::loadConfigFile(std::string &xmlFileName)
{
 return this->loadXmlFile(xmlFileName);
}

int WFXmlConfigManager::loadXmlFile(std::string &xmlFileName)
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

std::vector<WFXmlConfigManager::myAttrMap> WFXmlConfigManager::getAttributesOfChilds(DOMNodeList *nodeList)
{
  std::vector<WFXmlConfigManager::myAttrMap> myChildList;
  for(int i = 0; i < nodeList->getLength(); i++)
  {
      if (nodeList->item(i)->getNodeType() == 1)
      {
       myChildList.push_back(this->getAllAttributesFromElement((DOMElement*)(nodeList->item(i))));
      }
  }
  
  return myChildList;
}

WFXmlConfigManager::myAttrMap WFXmlConfigManager::getAllAttributesFromElement(DOMElement *curElem)
{
 WFXmlConfigManager::myAttrMap curAttrMap;
 DOMNamedNodeMap *attrMap = curElem->getAttributes();
 for(int i = 0; i < attrMap->getLength(); i++)
 {
  std::cout<<XMLString::transcode(attrMap->item(i)->getNodeName())<<"='"<<XMLString::transcode(attrMap->item(i)->getNodeValue())<<"'"<<std::endl;
  curAttrMap.insert(std::make_pair(XMLString::transcode(attrMap->item(i)->getNodeName()), XMLString::transcode(attrMap->item(i)->getNodeValue())));
 }
 return curAttrMap;
}

std::vector<WFXmlConfigManager::myAttrMap> WFXmlConfigManager::getAllKnownWorkflows()
{
  std::vector<myAttrMap> myKnownWFList;
  std::string parentTagName = "knownWorkflows";
  std::string childTagName = "workflow";
  myKnownWFList = this->getAttributesOfChilds(this->getAllChildesByName(parentTagName, childTagName));
  return myKnownWFList;
}

DOMNode *WFXmlConfigManager::getParentNodeByName(std::string &parentTagName)
{
  DOMNodeList *myKnownWFNL = this->xmlDoc->getDocumentElement()->getElementsByTagName(XMLString::transcode(parentTagName.c_str()));
  std::cout<<myKnownWFNL->getLength()<<" <"<<parentTagName<<"> Length"<<std::endl;
   
  if(myKnownWFNL->getLength() > 0)
    return myKnownWFNL->item(0);
  else
    return NULL;
}

DOMNodeList *WFXmlConfigManager::getAllChildesByName(std::string &parentTagName, std::string &childTagName)
{
  DOMNode *myKnownWFNode = this->getParentNodeByName(parentTagName);
  //check if the node type is an ElementNode
  if (myKnownWFNode != NULL && myKnownWFNode->getNodeType() == 1)
  {
   DOMElement *curElem = (DOMElement*)myKnownWFNode;
   return curElem->getElementsByTagName(XMLString::transcode(childTagName.c_str()));
  }
  else return NULL;
}

void WFXmlConfigManager::removeAllNodesWithAttribute(std::string &parentTagName, std::string &childTagName, std::string &attribName, std::string &attribValue)
{
  DOMNodeList *myKnownWFNLs = this->getAllChildesByName(parentTagName, childTagName);
  for(int i = 0; i < myKnownWFNLs->getLength(); i++)
  {
      if (myKnownWFNLs->item(i)->getNodeType() == 1)
      {
        DOMElement *curElem = (DOMElement*)(myKnownWFNLs->item(i));
        DOMNamedNodeMap *attrMap = curElem->getAttributes();
        std::string curAttrValue = XMLString::transcode(attrMap->getNamedItem(XMLString::transcode(attribName.c_str()))->getNodeValue());
        std::cout<<curAttrValue<<"="<<attribValue<<std::endl;
        if(curAttrValue == attribValue)
        {
          std::cout<<"found"<<std::endl;
          curElem->getParentNode()->removeChild(curElem);
          
        }
      }
  }
}

std::vector<WFXmlConfigManager::myAttrMap> WFXmlConfigManager::getAllLookUpPaths()
{
  std::vector<myAttrMap> myLookUpPaths;
  std::string parentTagName = "lookUpPaths";
  std::string childTagName = "path";
  myLookUpPaths = this->getAttributesOfChilds(this->getAllChildesByName(parentTagName, childTagName));
  return myLookUpPaths;
}

void WFXmlConfigManager::removeKnownWorkflow(std::string &fileName)
{
  std::string parentTagName = "knownWorkflows";
  std::string childTagName = "workflow";
  std::string attribName = "fileName";
    
  this->removeAllNodesWithAttribute(parentTagName,childTagName, attribName, fileName);
}

void WFXmlConfigManager::removeLookUpPath(std::string &path)
{
  std::string parentTagName = "lookUpPaths";
  std::string childTagName = "path";
  std::string attribName = "url";
    
  this->removeAllNodesWithAttribute(parentTagName,childTagName, attribName, path);
}

void WFXmlConfigManager::addKnownWorkflow(std::string &fileName, bool visible, bool enabled)
{
  std::string parentTagName = "knownWorkflows";
  DOMNode *myKnownWFNode = this->getParentNodeByName(parentTagName);
  
  if(myKnownWFNode != NULL && myKnownWFNode->getNodeType() == 1)
  {
    DOMElement *newKWF = xmlDoc->createElement(XMLString::transcode("workflow"));
    newKWF->setAttribute(XMLString::transcode("fileName"), XMLString::transcode(fileName.c_str()));
    newKWF->setAttribute(XMLString::transcode("visible"), XMLString::transcode(visible?"true":"false"));
    newKWF->setAttribute(XMLString::transcode("enabled"), XMLString::transcode(enabled?"true":"false"));
    myKnownWFNode->appendChild(newKWF);
  }
}
