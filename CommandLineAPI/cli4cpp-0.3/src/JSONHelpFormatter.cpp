/*
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cli/JSONHelpFormatter.h>

namespace CLI {

JSONHelpFormatter::JSONHelpFormatter() {}
JSONHelpFormatter::~JSONHelpFormatter() {}

  void JSONHelpFormatter::printJSONHelp ( const char* algclass,
                                      const char* name,
                                      const char* description,
                                      const char* executable,
                                      const Options& options )
  {
    std::cout << "/* JSON version 1.0 */" << std::endl;
    std::cout << "{" << std::endl;
    std::cout << "\t\"class\" : \"" << algclass << "\"," << std::endl;
    std::cout << "\t\"name\" : \"" << name << "\"," << std::endl;
    std::cout << "\t\"info\" : \"" << description << "\"," << std::endl;
    std::cout << "\t\"executable\" : \"" << executable << "\"," << std::endl;
    std::cout << "\t\"parameters\" : {" << std::endl;

    std::vector<std::string> invocationStrings;
    for(Options::Vector::const_iterator iterator = options().begin();
        iterator != options().end(); iterator++)
      {
      std::ostringstream invocation;
      invocation << "\t\t\"" << (*iterator).getLongOpt() << "\" : {\n ";
      invocation << "\t\t\t\"flag\" : \"" << (*iterator).getOpt()[0] << "\",\n";
      invocation << "\t\t\t\"name\" : \"" << (*iterator).getLongOpt() << "\",\n";
      invocation << "\t\t\t},\n";
      std::cout << invocation.str();
      }
    std::cout << "\t\t}\n}" << std::endl;
  }

}

