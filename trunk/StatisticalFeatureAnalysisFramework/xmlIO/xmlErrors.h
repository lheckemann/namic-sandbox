
/*!        \file xmlErrors.h
        \author Martin Styner
        \brief Definition of all errorcodes associated with the XML interface
        
        Definition of all errorcodes associated with the XML interface. 
        All errorcode < 0 are strict errors demanding actions on the part of the user
        All errocodes > 0 are warnings, i.e. the user can choose to disregard these errors
*/

#ifndef __xmlErrors_H
#define __xmlErrors_H

#include <string>

typedef enum xmlErrorValEnum {
//no error
XML_NO_ERROR = 0,
// warnings are positive integers or > XML_NO_ERROR
XML_NO_VALIDATION = 1,
XML_NO_WRITE_ACCESS =  2,
XML_WARN_CATEGORY_NODE = 3,
//errors are negative integers or < XML_NO_ERROR
XML_CANNOT_OPEN_FILE  = -1,
XML_CANNOT_PARSE_FILE  = -2,
XML_UNSPEC_SAVE_ERROR  = -3,
XML_NO_SUCH_HIERARCHY  = -4,
XML_EMPTY_SUBNODE_POINTER  = -5,
XML_ERR_ATTRIBUTE_NODE  = -6,
XML_ERR_UNSPEC_NODE  = -7,
XML_ERR_CATEGORY_NODE  = -8,
XML_UNSPEC_ERROR  = -9,
XML_NO_FILENAME  = -10,
XML_GET_ERROR  = -11,
XML_SET_ERROR  = -12
} xmlErrorVal;

// Class xmlErrors
// --------------
/*!        \brief Definition of all errorcodes associated with the XML interface
        
        Definition of all errorcodes associated with the XML interface. 
        All errorcodes < XML_NO_ERROR are strict errors demanding actions on the part of the user
        All errorcodes > XML_NO_ERROR are warnings, i.e. the user can choose to disregard these errors

        
*/
class xmlErrors
{
public:
        
        xmlErrors() { };
        
        ~xmlErrors() { };
        
        //! returns string associated with given error code, caller should free string after use
        static std::string getErrorString(xmlErrorVal error_code) {
                switch (error_code) {
                case  XML_NO_ERROR:
                          return ( std::string("Hey there is no error here!"));
                        break;
                  case XML_NO_VALIDATION:
                          return ( std::string("XML file is not a valid instance of this DTD!"));
                    break;
                case XML_NO_WRITE_ACCESS:
                          return ( std::string("Specified file has no write access."));
                    break;
            case XML_WARN_CATEGORY_NODE:
                          return ( std::string("Hierarchy points to existing category node."));
                    break;
                    
                  
                  case XML_CANNOT_OPEN_FILE:
                          return ( std::string("Cannot open XML file."));
                    break;
                  case XML_CANNOT_PARSE_FILE:
                          return ( std::string("Cannot parse XML file."));
                    break;
                case XML_UNSPEC_SAVE_ERROR:
                          return ( std::string("Cannot write XML file (despite write access). Disk full?"));
                    break;
                case XML_NO_SUCH_HIERARCHY:
                          return ( std::string("Cannot find such a hierarchy in the file."));
                    break;
            case XML_EMPTY_SUBNODE_POINTER:
                          return ( std::string("Null pointer as subnode/hierarchy parameter."));
                    break;
            case XML_ERR_UNSPEC_NODE:
                          return ( std::string("Hierarchy points to unspecified node type."));
                    break;
            case XML_ERR_ATTRIBUTE_NODE:
                          return ( std::string("Hierarchy points to existing attribute node."));
                    break;
            case XML_ERR_CATEGORY_NODE:
                          return ( std::string("Hierarchy points to existing category node."));
                    break;
            case XML_UNSPEC_ERROR:
                          return ( std::string("Unspecified error."));
                    break;
            case XML_NO_FILENAME:
                          return ( std::string("No filename specified -> no save/load possible."));
                    break;
            case XML_GET_ERROR:
                          return ( std::string("Error while getting value."));
                    break;
            case XML_SET_ERROR:
                          return ( std::string("Error while setting value."));
                    break;
            
                  default:
                          return ( std::string("Such an error code does not exist!"));
                          // should not come here
                }
                return ( std::string("Oops, what is this ?"));
        }
};

#endif // _xmlErrors_H


