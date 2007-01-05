#ifndef FREE_DIFFUSION_EXECUTABLE_H
#define FREE_DIFFUSION_EXECUTABLE_H

#include <string>
#include <vector>

#include "CommandParser.h"

class FreeDiffusionExecutable {

private:
  struct Argument {
    std::string flag; 
    std::string shortDescription;
    std::string longDescription;
    std::string example;
    std::string errorMessage;
  };

protected:
  std::string m_Name;
  std::string m_Description;
  std::string m_Output;
  std::string m_Children;
  std::string m_Version;
  std::string m_Author;
  std::string m_BugEmail;
  
  std::vector< Argument* > m_RequiredArguments;
  std::vector< Argument* > m_OptionalArguments;
  
  CommandParser *m_Parser;
  
  void SetArgument( std::vector< Argument* > *iArguments,
    std::string iFlag,
    std::string iShortDescription,
    std::string iLongDescription,
    std::string iExample,
    std::string iErrorMessage );
    
  void PrintArgumentsHelp( std::vector< Argument* > iArguments );
  
public:

  FreeDiffusionExecutable( int inArgs, char ** iaArgs );  
  ~FreeDiffusionExecutable();
  
  void PrintHelp();
  
  void SetNextRequiredArgument( std::string iFlag, std::string iShortDescription, 
    std::string iLongDescription, std::string iExample, 
    std::string iErrorMessage );

  void SetNextOptionalArgument( std::string iFlag, std::string iShortDescription,
    std::string iLongDescription );

  void SetName( std::string iName, std::string iDescription );
  std::string GetName();

  std::string GetSynposis();

  void SetOutput( std::string iOutput );
    
  std::string GetExampleUsage();

  void SetChildren( std::string iChildren );

  void SetAuthor( std::string iAuthor );
  
  void SetVersion( std::string iVersion );
  std::string GetVersion();
  
  void SetBugEmail( std::string iEmail );
  
  static void PrintError( std::string iError );

  static void PrintErrorArgument( std::string iArgument, 
    std::string iDetailedMessage );
    
  static void WriteData( const std::string fileName, const double *data, 
    const int nData);

  static void WriteData( const std::string fileName, 
    double **dataArray, const int nRows, const int cCols );

  /**
   * Returns the input arguments for the flags in the same order that the
   * flags were originally input input.
   */
  std::string* GetRequiredArguments();
  
  virtual bool FillArguments() = 0;
  virtual void Run() = 0;
    
};

#endif
