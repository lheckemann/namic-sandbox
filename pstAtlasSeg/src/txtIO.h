#ifndef txtIO_h_
#define txtIO_h_

#include <string>
#include <vector>



namespace newProstate
{
  template<typename TNull>
  std::vector< std::string > readTextLineToListOfString(const char* textFileName);



} //douher

#include "txtIO.hpp"

#endif
