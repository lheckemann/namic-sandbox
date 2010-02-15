#ifndef txtIO_hpp_
#define txtIO_hpp_


#include <iostream>
#include <fstream>

//boost
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "txtIO.h"


namespace douher
{

  /*============================================================
   * readTextLineToListOfString   
   */
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



  /*============================================================
   * readNumbersToVector
   */
  template<typename TData>
  void readNumbersToVector(const char* textFileName, std::vector< TData >& v)
  {
    /* 
       The txt file should only contain numbers and deliminitor (white
       space, enter, tab, , ;)

       procedure:

       1. read whole file to a string
       2. tokenize the string by ";, "
       3. lexical_cast each token (string) to number



       "The file MUST end with an empty line, then each line will be
       stored as an element in the returned vector object. "  ---With
       the newer implementation using boost::lexical_cast, this does
       not seem to be a problem. But for debugging hint, I'll leave
       the above warning there. 20090914
    */


    // v will be cleared !!!
    v.clear();

    // 1.
    std::ifstream f(textFileName);
    std::string wholeFile;
    std::string thisLine;

    if (f.good())
      {
        while(std::getline(f, thisLine))
          {
            wholeFile += thisLine;
            wholeFile += ' ';
          }
      }
    else
    {
      std::cerr<<"can not open file:"<<textFileName<<std::endl;
      raise(SIGABRT);
    }

    //debug//
    //    std::cout<<wholeFile<<std::endl;
    //DEBUG//

    // 2.
    std::vector< std::string > tokenList;
    boost::split(tokenList, wholeFile, boost::is_any_of(";,\n\t "), boost::token_compress_on);


    // 3
    unsigned long n = tokenList.size();
    // cout<<"Have "<<n<<" tokens.\n";
    for (unsigned long i = 0; i < n; ++i)
      {
        if (!tokenList[i].size())
          {
            continue;
          }

        try
          {
            //debug//
            //            std::cout<<i<<"\t"<<tokenList[i]<<std::endl;
            //DEBUG//

            v.push_back(boost::lexical_cast<TData>(tokenList[i]));
          }
        catch(boost::bad_lexical_cast &)
          {
            std::cerr<<"error casting\n";
            exit(-1);
          }
      }


    f.close();

    return;
  }



  template<typename TData>
  std::vector< TData >
  readNumbersToVector(const char* textFileName)
  {
    /* 
       See comments in:

       template<typename TData>
       void readNumbersToVector(const char* textFileName, std::vector< TData >& v)

       for detail   
    */

    std::vector< TData > v;

    // 1.
    std::ifstream f(textFileName);
    std::string wholeFile;
    std::string thisLine;

    if (f.good())
      {
        while(std::getline(f, thisLine))
          {
            wholeFile += thisLine;
            wholeFile += ' ';
          }
      }
    else
    {
      std::cerr<<"can not open file:"<<textFileName<<std::endl;
      raise(SIGABRT);
    }

    //debug//
    //    std::cout<<wholeFile<<std::endl;
    //DEBUG//

    // 2.
    std::vector< std::string > tokenList;
    boost::split(tokenList, wholeFile, boost::is_any_of(";,\n\t "), boost::token_compress_on);


    // 3
    unsigned long n = tokenList.size();
    // cout<<"Have "<<n<<" tokens.\n";
    for (unsigned long i = 0; i < n; ++i)
      {
        if (!tokenList[i].size())
          {
            continue;
          }

        try
          {
            //debug//
            //            std::cout<<i<<"\t"<<tokenList[i]<<std::endl;
            //DEBUG//

            v.push_back(boost::lexical_cast<TData>(tokenList[i]));
          }
        catch(boost::bad_lexical_cast &)
          {
            std::cerr<<"error casting\n";
            exit(-1);
          }
      }


    f.close();

    return v;
  }


  /*================================================================================*/
  template<typename TData>
  void saveVector(const char* textFileName, const std::vector< TData >& v)
  {
    if (v.empty())
      {
        std::cerr<<"Error: input vector is empty.\n";
        raise(SIGABRT);
      }

    std::ofstream f(textFileName);

    unsigned long n = v.size();

    if (f.good())
      {
        for (unsigned long i = 0; i < n; ++i)
          {
            f << v[i] <<"  ";
          }
        f<<std::endl;
      }
    else
    {
      std::cerr<<"can not open file: "<<textFileName<<std::endl;
      raise(SIGABRT);
    }


    f.close();
    
    return;
  }



}//douher


#endif
