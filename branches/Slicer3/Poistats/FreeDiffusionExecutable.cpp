#include <iostream>
#include <iterator>

#include <fstream>

#include "FreeDiffusionExecutable.h"

FreeDiffusionExecutable::FreeDiffusionExecutable( int inArgs, char ** iaArgs ) {
  m_Parser = new CommandParser( inArgs, iaArgs );
}

FreeDiffusionExecutable::~FreeDiffusionExecutable() {
  delete m_Parser;
  m_Parser = NULL;
  
// TODO: delete arguments  
}

void
FreeDiffusionExecutable::SetArgument( std::vector< Argument* > *iArguments, 
  std::string iFlag,
  std::string iShortDescription,
  std::string iLongDescription,
  std::string iExample,
  std::string iErrorMessage ) {

  Argument *argument = new Argument();
  argument->flag = iFlag;
  argument->shortDescription = iShortDescription;
  argument->longDescription = iLongDescription;
  argument->example = iExample;
  argument->errorMessage = iErrorMessage;

  iArguments->insert( iArguments->end(), argument );
} 

void 
FreeDiffusionExecutable::SetNextRequiredArgument( std::string iFlag, 
  std::string iShortDescription, 
  std::string iLongDescription,
  std::string iExample,
  std::string iErrorMessage ) {

  SetArgument( &m_RequiredArguments, iFlag, iShortDescription, iLongDescription,
    iExample, iErrorMessage );

}

void
FreeDiffusionExecutable::SetNextOptionalArgument( std::string iFlag,
  std::string iShortDescription,
  std::string iLongDescription ) {

  SetArgument( &m_OptionalArguments, iFlag, iShortDescription, iLongDescription,
    "", "" );

}

void
FreeDiffusionExecutable::SetName( std::string iName, 
                                  std::string iDescription ) {
  m_Name = iName;
  m_Description = iDescription;
}

std::string
FreeDiffusionExecutable::GetName() {
  return m_Name;
}

std::string
FreeDiffusionExecutable::GetSynposis() {
  std::string synopsis = m_Name;
  
  for( std::vector< Argument* >::const_iterator arguments = m_RequiredArguments.begin(); 
    arguments != m_RequiredArguments.end(); arguments++ ) {

    Argument *argument = *arguments;
    synopsis = synopsis + " " + argument->flag + 
               " <" + argument->shortDescription + ">";
  }  
  
  synopsis = synopsis + " [<options>]";
  
  return synopsis;
}

void
FreeDiffusionExecutable::SetOutput( std::string iOutput ) {
  m_Output = iOutput;
}

std::string
FreeDiffusionExecutable::GetExampleUsage() {
  std::string example = m_Name;
  
  for( std::vector< Argument* >::const_iterator arguments = m_RequiredArguments.begin(); 
    arguments != m_RequiredArguments.end(); arguments++ ) {

    Argument *argument = *arguments;
    example = example + " " + argument->flag + " " + argument->example;
  }  
  
  return example;
}

void
FreeDiffusionExecutable::SetChildren( std::string iChildren ) {
  m_Children = iChildren;
}

void
FreeDiffusionExecutable::SetAuthor( std::string iAuthor ) {
  m_Author = iAuthor;
}

void
FreeDiffusionExecutable::SetVersion( std::string iVersion ) {
  m_Version = iVersion;
}

std::string
FreeDiffusionExecutable::GetVersion() {
  return m_Version;
}

void
FreeDiffusionExecutable::SetBugEmail( std::string iEmail ) {
  m_BugEmail = iEmail;
}

void
FreeDiffusionExecutable::PrintArgumentsHelp( 
  std::vector< Argument* > iArguments ) {
  
  for( std::vector< Argument* >::const_iterator arguments = iArguments.begin(); 
    arguments != iArguments.end(); arguments++ ) {

    Argument *argument = *arguments;
    std::cout << "     " << argument->flag << " <" << argument->shortDescription << ">" << std::endl;

    if( argument->longDescription != "" ) {
      std::cout << "         " << argument->longDescription << std::endl;
    }

    std::cout << std::endl;
  }  
}

void
FreeDiffusionExecutable::PrintHelp() {
  std::string spaces = "\t";
  
  std::cout << "Name\n" << std::endl;
  std::cout << spaces << m_Name << " - " << m_Description << std::endl;

  std::cout << std::endl;

  std::cout << "Synopsis" << std::endl;
  std::cout << spaces << GetSynposis() << std::endl;
 
  std::cout << std::endl;
 
  std::cout << "Required Arguments" << std::endl;
  PrintArgumentsHelp( m_RequiredArguments );

  std::cout << "Optional Arguments" << std::endl;
  PrintArgumentsHelp( m_OptionalArguments );
  
  std::cout << "Output" << std::endl;
  std::cout << spaces << m_Output << std::endl;  
  
  std::cout << std::endl;
  
  std::cout << "Example" << std::endl;
  std::cout << spaces << GetExampleUsage() << std::endl;

  std::cout << std::endl;

  std::cout << "Children" << std::endl;
  std::cout << spaces << m_Children << std::endl;

  std::cout << std::endl;
  
  std::cout << "Version" << std::endl;
  std::cout << spaces << m_Version << std::endl;

  std::cout << std::endl;

  std::cout << "Author" << std::endl;
  std::cout << spaces << "Written by " << m_Author << std::endl;

  std::cout << std::endl;

  std::cout << "Reporting Bugs" << std::endl;
  std::cout << spaces << "Report bugs to <" << m_BugEmail << ">" << std::endl;

  std::cout << std::endl;
}

void 
FreeDiffusionExecutable::PrintError( std::string iError ) {
  std::cout << "ERROR: " << iError << std::endl;
}

void 
FreeDiffusionExecutable::PrintErrorArgument( std::string iArgument, 
    std::string iDetailedMessage ) {
    
  std::string errorMessage = iArgument + " argument missing";

  PrintError( errorMessage );
  PrintError( iDetailedMessage );
}

std::string*
FreeDiffusionExecutable::GetRequiredArguments() {
  std::string *requiredArgumentValues = 
    new std::string[ m_RequiredArguments.size() ];
  
  int i=0;
  for( std::vector< Argument* >::const_iterator arguments = m_RequiredArguments.begin(); 
       arguments != m_RequiredArguments.end(); i++, arguments++ ) {

    Argument *argument = *arguments;
    char *argumentStr = m_Parser->GetArgument( argument->flag.c_str() );

    if( argumentStr == NULL ) {
      PrintErrorArgument( argument->flag, argument->errorMessage );
      // TODO: throw an error
      throw "invalid";
    }
    
    requiredArgumentValues[i] = argumentStr;
  }
  
  return requiredArgumentValues;
}

void 
FreeDiffusionExecutable::WriteData( const std::string fileName, 
  const double *data, const int nData) {

  std::cerr << "writing: " << fileName << std::endl;
  
  std::ofstream output( fileName.c_str() );
  
  for( int cData=0; cData<nData; cData++ ) {
    output << data[ cData ] << std::endl;
  }
  
  output.close();
    
}

void 
FreeDiffusionExecutable::WriteData( const std::string fileName, 
  double **dataArray, const int nRows, const int nCols ) {

  std::cerr << "writing: " << fileName << std::endl;
  
  std::ofstream output( fileName.c_str() );
  
  for( int cRow=0; cRow<nRows; cRow++ ) {
    for( int cCol=0; cCol<nCols; cCol++ ) {
      output << dataArray[ cRow ][ cCol ] << "   ";
    }    
    output << std::endl;
  }
  
  output.close();
    
}
