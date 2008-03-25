
#include <fstream>
#include <iostream>

void ReplaceCharacters( const std::string& inString, 
                              std::string& outString, 
                        const char targetCharacter, 
                        const char replacementCharacter)
{
  outString.clear();

  for (std::string::const_iterator siter = inString.begin();
        siter != inString.end();
        ++siter)
    {
    if ( targetCharacter == *siter )
      {
      outString.push_back( replacementCharacter );
      }
    else
      {
      outString.push_back( *siter );
      }
    }

}

int main( int argc, char* argv[] )
{
  if (argc < 2)
    {
    return EXIT_SUCCESS;
    }

  std::ofstream outputFile(argv[1]);

  if ( false == outputFile.is_open() )
    {
    std::cerr << "Couldn't open file : " << argv[1] << std::endl;
    return EXIT_FAILURE;
    }

  outputFile << "\\begin{table}" << std::endl;
  outputFile << "\\begin{center}" << std::endl;
  outputFile << "\\begin{tabular}{ | l | c | }" << std::endl;
  outputFile << "\\hline" << std::endl;
  outputFile << "\\multicolumn{2}{|c|}{ " << argv[2] << " } \\\\ \\hline" << std::endl;

  // Row name, row value. Assumes no spaces in either.
  std::string string1;
  std::string string2;
  std::string rstring1;
  std::string rstring2;

  for ( int i = 3; i < argc - 3; i += 2 )
    {
    string1 = argv[ i ];
    string2 = argv[ i+1 ];
    ReplaceCharacters( string1, rstring1, '_', ' ' );
    ReplaceCharacters( string2, rstring2, '_', ' ' );
    outputFile << rstring1.c_str() << " & " << rstring2.c_str() << "\\\\ \\hline" << std::endl;    
    }

  outputFile << "\\end{tabular}" << std::endl;
  outputFile << "\\end{center}" << std::endl;
  outputFile << "\\label{" << argv[argc - 2] << "}" << std::endl;
  outputFile << "\\caption{" << argv[argc - 1] << "}" << std::endl;
  outputFile << "\\end{table}" << std::endl;

  return EXIT_SUCCESS;
}

