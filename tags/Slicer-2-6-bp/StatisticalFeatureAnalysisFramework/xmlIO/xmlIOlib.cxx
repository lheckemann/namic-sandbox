
/*!        \file xmlIOlib.C
        \author Martin Styner
        
        \brief C++ source for XML io library interface, only general functions
        
        Full description.
*/

//
// Includes
//
#include <iostream>
#include <fstream>
#include <dirent.h>

#include "xmlIOlib.h"

static const int debug = 1;

// xmlIOlib::createXmlIO
// --------------------------
/*!  create a new xmlIO object
*/        
xmlIO * xmlIOlib::createXmlIO()
{
  return new xmlIOxml2();
}


