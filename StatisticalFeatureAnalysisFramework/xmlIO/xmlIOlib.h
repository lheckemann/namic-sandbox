

/*!        \file xmlIOlib.h
        \author Martin Styner
        
        \brief provides routine for creating dynamically xmlIO  objects without knowing which XML implementation is used
        
        simple example:
        xmlIOlib * xmlLib = new xmlIOlib();
        xmlIO * xmlDoc = xmlLib->createXmlIO();
        xmlErrorVal openErrVal = xmlDoc->SetFilename("myXML.xml");
        if (openErrVal) cout << *(xmlErrors::getErrorString(openErrVal)) << endl;
        xmlDoc->Print();
        xmlDoc->Close(); // all temporary data is freed, as well as DOM
*/

#ifndef __xmlIOlib_H
#define __xmlIOlib_H
 
//
// Includes
//
#include <iostream>
#include <string>
 
#include "xmlIObase.h"
#include "xmlIOxml2.h"
 
// Class xmlIOlib
// --------------
/*!        \brief provides routine for creating dynamically xmlIO objects using libxml2without knowing which XML implementation is used
        
        \verbatim
        simple example:
        xmlIO * xmlDoc = xmlLib::createXmlIO();
        xmlErrorVal openErrVal = xmlDoc->SetFilename("myXML.xml");
        if (openErrVal != XML__NO_ERROR) cout << *(xmlErrors::getErrorString(openErrVal)) << endl;
        xmlDoc->Print();
        xmlDoc->Close(); // all temporary data is freed, as well as DOM
        \endverbatim
*/

class xmlIOlib
{
public:
        
        //! create a new xmlIO object
        static  xmlIO * createXmlIO();
        
private:

        
};

#endif // __xmlIOlib_H


