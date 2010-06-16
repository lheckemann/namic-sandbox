#ifndef txtIO_h_
#define txtIO_h_

#include <string>
#include <vector>



namespace douher
{
//  template<typename TNull>
  std::vector< std::string > readTextLineToListOfString(const char* textFileName);


  template<typename TData>
  void readNumbersToVector(const char* textFileName, std::vector< TData >& v);


  /* If TData is floating numbers, this works find. But there seems to
     be some problem for types such as long. Becareful. Or, may read
     in as float and then cat to TData one by one. */
  template<typename TData>
  std::vector< TData >
  readNumbersToVector(const char* textFileName);



  template<typename TData>
  void saveVector(const char* textFileName, const std::vector< TData >& v);




} //douher

#include "txtIO.hpp"

#endif
