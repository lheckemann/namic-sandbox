#ifndef txtIO_hpp_
#define txtIO_hpp_


#include <iostream>
#include <fstream>

//boost
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "boost/shared_ptr.hpp"

#include <cassert>

#include <csignal>

#include "txtIO.h"


namespace newProstate
{

  /*============================================================
   * readTextLineToListOfString   
   */
  template<typename TNull>
  std::vector< std::string > readTextLineToListOfString(const char* textFileName)
  {
    /* The file MUST end with an empty line, then each line will be
       stored as an element in the returned vector object. */


    // Here speed and space is not a big issue, so just let it copy and return stuff.
    std::vector< std::string > listOfStrings;

    std::ifstream f(textFileName);
    std::string thisLine;

    if (f.good())
      {
        while( std::getline(f, thisLine) )
          {
            listOfStrings.push_back(thisLine);
          }
      }
    else
      {
        std::cerr<<"Error: can not open file:"<<textFileName<<std::endl;
        raise(SIGABRT);
      }

    f.close();

    return listOfStrings;
  }



}//douher


#endif
