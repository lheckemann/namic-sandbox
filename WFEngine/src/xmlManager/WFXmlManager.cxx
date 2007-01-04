#include "WFXmlManager.h"

#include "DOMTreeErrorReporter.h"
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include "WFXmlWriter.h"

using namespace std;

using namespace WFEngine::nmWFXmlManager;

XERCES_CPP_NAMESPACE_USE;

WFXmlManager::WFXmlManager()
{
 gDoNamespaces          = false;
 gDoSchema              = false;
 gSchemaFullChecking    = false;
 gDoCreate              = false;
 
 char* goutputfile      = 0;
 //options for DOMLSSerializer's features
 XMLCh* gOutputEncoding = 0;
 
 gSplitCdataSections    = true;
 gDiscardDefaultContent = true;
 gUseFilter;//             = false;
 gFormatPrettyPrint     = true;
 gWhitespaceInElementContent = false;
 gWriteBOM              = false;
 gValScheme = XercesDOMParser::Val_Auto;
}

WFXmlManager::~WFXmlManager()
{
}

//template<class Child>
//WFXmlManager* WFXmlManager::New()
//{
// WFXmlManager* myWFXmlMgr = new WFXmlManager;
// myWFXmlMgr->m_wfXmlMgr = myWFXmlMgr;
// return myWFXmlMgr;
//}

int WFXmlManager::initializeXerces(std::string xmlFileName)
{
 int retVal = 0;
 //Initialize the XML4C2 system
    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch(const XMLException &toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
             << "  Exception message:"
             << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
        retVal = 1;
        return retVal;
    }
    
    XercesDOMParser *parser = new XercesDOMParser;
    parser->setValidationScheme(gValScheme);
    parser->setDoNamespaces(gDoNamespaces);
    parser->setDoSchema(gDoSchema);
    parser->setValidationSchemaFullChecking(gSchemaFullChecking);
    parser->setCreateEntityReferenceNodes(gDoCreate);

    DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
    parser->setErrorHandler(errReporter);

    //
    //  Parse the XML file, catching any XML exceptions that might propogate
    //  out of it.
    //
    try
    {
        parser->parse(xmlFileName.c_str());
    }
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        retVal = 1;
    }
    catch (const XMLException& e)
    {
        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n   Message: "
             << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        retVal = 1;
    }

    catch (const DOMException& e)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];

        XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << xmlFileName << "'\n"
             << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

        if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
             XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;

        retVal = 1;
    }

    catch (...)
    {
        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
        retVal = 1;
    }
    
    if(retVal != 1)
    {
     this->xmlDoc = parser->getDocument();
     cout<<XMLString::transcode(this->xmlDoc->getDocumentElement()->getTagName())<<std::endl;
     this->goutputfile = (char*)xmlFileName.c_str();
    }
    std::cout<<parser->getErrorCount()<<std::endl;
    //std::cout<<this->xmlParser->getErrorCount()<<std::endl;
    return retVal;
}

void WFXmlManager::setXercesParsingOptions(bool doNamespaces,
  bool doSchema,
  bool schemaFullChecking,
  bool doCreate,
  bool splitCdataSections,
  bool discardDefaultContent,
  bool useFilter,
  bool formatPrettyPrint,
  bool writeBOM,
  int validateScheme)
{
 this->gDoNamespaces = doNamespaces;
 this->gDoSchema = doSchema;
 this->gSchemaFullChecking = schemaFullChecking;
 this->gDoCreate = doCreate;
 this->gSplitCdataSections = splitCdataSections;
 this->gDiscardDefaultContent = discardDefaultContent;
 this->gUseFilter = useFilter;
 this->gFormatPrettyPrint = formatPrettyPrint;
 this->gWriteBOM = writeBOM;
 
 switch (validateScheme)
 {
 case 0:
  this->gValScheme = XercesDOMParser::Val_Auto;
  break;
 case 1:
  this->gValScheme = XercesDOMParser::Val_Never;
  break;
 case 2:
  this->gValScheme = XercesDOMParser::Val_Always;
  break;
 }
}

int WFXmlManager::saveXmlFile()
{
  int retval;
  WFXmlWriter *myFilter = 0;

    try
    {
        // get a serializer, an instance of DOMLSSerializer
        XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
        DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(tempStr);
        DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
        DOMLSOutput       *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();

        // set user specified output encoding
        theOutputDesc->setEncoding(gOutputEncoding);

        // plug in user's own filter
        if (gUseFilter)
        {
            // even we say to show attribute, but the DOMLSSerializer
            // will not show attribute nodes to the filter as
            // the specs explicitly says that DOMLSSerializer shall
            // NOT show attributes to DOMLSSerializerFilter.
            //
            // so DOMNodeFilter::SHOW_ATTRIBUTE has no effect.
            // same DOMNodeFilter::SHOW_DOCUMENT_TYPE, no effect.
            //
            myFilter = new WFXmlWriter(DOMNodeFilter::SHOW_ELEMENT   |
                                          DOMNodeFilter::SHOW_ATTRIBUTE |
                                          DOMNodeFilter::SHOW_DOCUMENT_TYPE);
            theSerializer->setFilter(myFilter);
        }

        // plug in user's own error handler
        DOMConfiguration* serializerConfig=theSerializer->getDomConfig();
        
        // set feature if the serializer supports the feature/mode
        if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTSplitCdataSections, gSplitCdataSections))
            serializerConfig->setParameter(XMLUni::fgDOMWRTSplitCdataSections, gSplitCdataSections);

        if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTDiscardDefaultContent, gDiscardDefaultContent))
            serializerConfig->setParameter(XMLUni::fgDOMWRTDiscardDefaultContent, gDiscardDefaultContent);

        if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, gFormatPrettyPrint))
            serializerConfig->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, gFormatPrettyPrint);

        if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTBOM, gWriteBOM))
            serializerConfig->setParameter(XMLUni::fgDOMWRTBOM, gWriteBOM);

        if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTWhitespaceInElementContent , gWhitespaceInElementContent))
            serializerConfig->setParameter(XMLUni::fgDOMWRTWhitespaceInElementContent , gWhitespaceInElementContent);
        //
        // Plug in a format target to receive the resultant
        // XML stream from the serializer.
        //
        // StdOutFormatTarget prints the resultant XML stream
        // to stdout once it receives any thing from the serializer.
        //
        XMLFormatTarget *myFormTarget;
        myFormTarget=new LocalFileFormatTarget(goutputfile);
        theOutputDesc->setByteStream(myFormTarget);

        //
        // do the serialization through DOMLSSerializer::write();
        //
        theSerializer->write(this->xmlDoc, theOutputDesc);

        theOutputDesc->release();
        theSerializer->release();

        //
        // Filter, formatTarget and error handler
        // are NOT owned by the serializer.
        //
        delete myFormTarget;

        if (gUseFilter)
            delete myFilter;

    }
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        retval = 5;
    }
    catch (XMLException& e)
    {
        XERCES_STD_QUALIFIER cerr << "An error occurred during creation of output transcoder. Msg is:"
            << XERCES_STD_QUALIFIER endl
            << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        retval = 4;
    }
}
