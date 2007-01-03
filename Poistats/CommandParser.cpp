#include <string>
#include <iostream>
#include <sstream>
#include "CommandParser.h"

CommandParser::CommandParser( int inArgs, char ** iaArgs ) {
  m_nArgs = inArgs;
  m_aArgs = iaArgs;
}

CommandParser::~CommandParser() {

}
  
int
CommandParser::GetNumberOfArguments() {
  return m_nArgs;
}

char **
CommandParser::GetArguments() {
  return m_aArgs;
}

/**
 * Return the position in the array of arguments of the flag.  Returns 
 * FLAG_NOT_FOUND if the flag isn't found.
 */
int 
CommandParser::GetPosition( const char *iFlag ) {
  std::string flag = std::string( iFlag );

  bool isFound = false;
  int position = -1;

  for( int cArgs=0; cArgs<m_nArgs && isFound == false; cArgs++ ) {
    if( flag == m_aArgs[ cArgs ] ) {
      position = cArgs;
    }
  }
  
  return position;
}

char *
CommandParser::GetArgument( const char *iFlag ) {
  char *argument = NULL;
  
  int position = GetPosition( iFlag );
  if( position != FLAG_NOT_FOUND ) {
    if( position+1 < m_nArgs ) {
      argument = m_aArgs[ position + 1 ];
    }
  }
  
  return argument;  
}

int 
CommandParser::GetArgumentInt( const char *iFlag ) {
  char *argumentStr = GetArgument( iFlag );
  
  int argument = -1;
  
  if( argumentStr != NULL ) {
    argument = atoi( argumentStr );
  }
  
  return argument;  
}

std::vector< int > 
CommandParser::GetArgumentIntVector( const char *iFlag ) {

  char* argument = this->GetArgument( iFlag );  
  std::vector< int > argumentVector;
  
  if( argument != NULL ) {
    std::string arguments( argument );
    
    std::stringstream tokenizer( arguments, std::stringstream::in | std::stringstream::out );
    std::string token;
      
    while ( tokenizer >> token) {
      int value = atoi( token.c_str() );
      
      argumentVector.push_back( value );    
    }
  }
  
  return argumentVector;

}

double
CommandParser::GetArgumentDouble( const char *iFlag ) {
  char *argumentStr = GetArgument( iFlag );
  
  double argument = -1;
  
  if( argumentStr != NULL ) {
    argument = atof( argumentStr );
  }
  
  return argument;  
}

