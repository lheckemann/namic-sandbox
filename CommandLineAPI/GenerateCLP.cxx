/* This is simple demonstration of how to use expat. This program
   reads an XML document from standard input and writes a line with
   the name of each element to standard output indenting child
   elements by one tab stop more than their parent element.
   It must be used with Expat compiled for UTF-8 output.
*/

#include <stdlib.h>
#include <fstream>
#include "expat.h"
#include <string>
#include <vector>

#include "GenerateCLP.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif


class CommandLineArg
{
public:
  std::string m_Variable;
  std::string m_Container;
  std::string m_ShortFlag;
  std::string m_LongFlag;
  std::string m_Description;
  std::string m_Label;
  std::string m_Default;
  std::string m_Type;
  std::string m_StringToType;
  std::string m_Channel;
  unsigned int m_Index;
  CommandLineArg()
  {
  }
  bool NeedsTemp()
  {
    return (m_Type == "std::vector<int>" ||
            m_Type == "std::vector<float>" ||
            m_Type == "std::vector<double>");
  }
  bool NeedsQuotes()
  {
    return (m_Type == "std::vector<int>" ||
            m_Type == "std::vector<float>" ||
            m_Type == "std::vector<double>" ||
            m_Type == "std::string");
  }
};

static CommandLineArg *current;

static std::string lastTag;
static std::string description;
static std::vector<CommandLineArg> allArgs;
static bool debug = false;

void GenerateTCLAP(std::ofstream &);
void GenerateXML(std::ofstream &, std::string);

void
trimLeading(std::string& s, char* extraneousChars = " \t\n")
{
s = s.substr(s.find_first_not_of(extraneousChars));
}

void
trimTrailing(std::string& s, char* extraneousChars = " \t\n")
{
s = s.substr(0, s.find_last_not_of(extraneousChars)+1);
}

static void
startElement(void *userData, const char *name, const char **atts)
{
  CommandLineArg *arg = current;
  if (debug) std::cout << name << std::endl;

  lastTag.clear();

  if (strcmp(name, "integer") == 0)
    {
    arg = new CommandLineArg;
    arg->m_Type = "int";
    }
  else if (strcmp(name, "float") == 0)
    {
    arg = new CommandLineArg;
    arg->m_Type = "float";
    }
  else if (strcmp(name, "double") == 0)
    {
    arg = new CommandLineArg;
    arg->m_Type = "double";
    }
  else if (strcmp(name, "string") == 0)
    {
    arg = new CommandLineArg;
    arg->m_Type = "std::string";
    }
  else if (strcmp(name, "boolean") == 0)
    {
    arg = new CommandLineArg;
    arg->m_Type = "bool";
    }
  else if (strcmp(name, "integer-vector") == 0)
    {
    arg = new CommandLineArg;
    arg->m_Type = "std::vector<int>";
    arg->m_StringToType = "atoi";
    }
  else if (strcmp(name, "float-vector") == 0)
    {
    arg = new CommandLineArg;
    arg->m_Type = "std::vector<float>";
    arg->m_StringToType = "atof";
    }
  else if (strcmp(name, "double-vector") == 0)
    {
    arg = new CommandLineArg;
    arg->m_Type = "std::vector<double>";
    arg->m_StringToType = "atof";
    }
  else if (strcmp(name, "filename") == 0)
    {
    arg = new CommandLineArg;
    arg->m_Type = "std::string";
    }
  current = arg;
}

static void
endElement(void *c, const char *name)
{
  CommandLineArg *arg = current;

  if (strcmp(name, "integer") == 0)
    {
    allArgs.push_back(*arg);
    current = 0;
    }
  else if (strcmp(name, "float") == 0)
    {
    allArgs.push_back(*arg);
    current = 0;
    }
  else if (strcmp(name, "double") == 0)
    {
    allArgs.push_back(*arg);
    current = 0;
    }
  else if (strcmp(name, "string") == 0)
    {
    allArgs.push_back(*arg);
    current = 0;
    }
  else if (strcmp(name, "boolean") == 0)
    {
    allArgs.push_back(*arg);
    current = 0;
    }
  else if (strcmp(name, "filename") == 0)
    {
    allArgs.push_back(*arg);
    current = 0;
    }
  else if (strcmp(name, "integer-vector") == 0)
    {
    allArgs.push_back(*arg);
    current = 0;
    }
  else if (strcmp(name, "float-vector") == 0)
    {
    allArgs.push_back(*arg);
    current = 0;
    }
  else if (strcmp(name, "double-vector") == 0)
    {
    allArgs.push_back(*arg);
    current = 0;
    }
  else if (strcmp(name, "flag") == 0)
    {
    arg->m_ShortFlag = lastTag;
    trimLeading(arg->m_ShortFlag);
    trimTrailing(arg->m_ShortFlag);
    }
  else if (strcmp(name, "longflag") == 0)
    {
    arg->m_LongFlag = lastTag;
    trimLeading(arg->m_LongFlag);
    trimTrailing(arg->m_LongFlag);
    if (arg->m_Variable.empty())
      {
      arg->m_Variable = std::string(lastTag);
      trimLeading(arg->m_Variable);
      trimTrailing(arg->m_Variable);
      trimLeading(arg->m_Variable,"-");
      }
    }
  else if (strcmp(name, "name") == 0)
    {
    if (debug) std::cout << "--------------------" << lastTag << std::endl;
    arg->m_Variable = std::string(lastTag);
    trimLeading(arg->m_Variable);
    trimTrailing(arg->m_Variable);
    }
  else if (strcmp(name, "description") == 0)
    {
    if (arg)
      {
      arg->m_Description = lastTag;
      trimLeading(arg->m_Description);
      trimTrailing(arg->m_Description);
      }
    else
      {
      if (description.empty())
        {
        description = lastTag;
        trimLeading(description);
        trimTrailing(description);
        }
      }
    }
  else if (strcmp(name, "default") == 0)
    {
    arg->m_Default = lastTag;
    trimLeading(arg->m_Default);
    trimTrailing(arg->m_Default);
    }
}

static void
charData(void *userData, const char *s, int len)
{
  if (len)
    {
    std::string str(s,len);
    lastTag += str;
    if (debug) std::cout << "|" << str << "|" << std::endl;
    }
}

int
main(int argc, char *argv[])
{
#include "GenerateCLP.hxx"

  XML_Parser parser = XML_ParserCreate(NULL);
  int done;
  int depth = 0;
  current = 0;

  XML_SetUserData(parser, static_cast<void *>(current));
  XML_SetElementHandler(parser, startElement, endElement);
  XML_SetCharacterDataHandler(parser, charData);
  std::ifstream fin(InputXML.c_str(),std::ios::in);
  if (fin.fail())
    {
    std::cerr << argv[0] << ": Cannot open " << InputXML << " for input" << std::endl;
    perror(argv[0]);
    return EXIT_FAILURE;
    }

  // Get the length of the file
  fin.seekg (0, std::ios::end);
  size_t len = fin.tellg();
  fin.seekg (0, std::ios::beg);
  char * XML = new char[len];
  do {
  fin.read (XML, len);
    done = true;
    if (XML_Parse(parser, XML, len, done) == 0) {
      fprintf(stderr,
              "%s at line %" XML_FMT_INT_MOD "u\n",
              XML_ErrorString(XML_GetErrorCode(parser)),
              XML_GetCurrentLineNumber(parser));
      return 1;
    }
  } while (!done);
  fin.close();
  XML_ParserFree(parser);

// Print each command line arg
  std::cerr << "Found " << allArgs.size() << " command line arguments" << std::endl;
// Do the hard stuff
  std::ofstream sout(OutputCxx.c_str(),std::ios::out);
  GenerateXML(sout, InputXML);
  GenerateTCLAP(sout);

  return (EXIT_SUCCESS);
}

void GenerateXML(std::ofstream &sout, std::string XMLFile)
{
  char linec[2048];
  std::ifstream fin(XMLFile.c_str(),std::ios::in);

  // Generate special section to produce xml description
  sout << "  if (argc >= 2 && (strcmp(argv[1],\"--xml\") == 0))" << std::endl;
  sout << "    {" << std::endl;

  while (!fin.eof())
    {
    fin.getline (linec, 2048);
    // replace quotes with escaped quotes
    std::string line(linec);
    std::string cleanLine;
    for (size_t j = 0; j < line.length(); j++)
      {
      if (line[j] == '\"')
        {
        cleanLine += "\\\"";
        }
      else
        {
        cleanLine += line[j];
        }
      }
    sout << "std::cout << \"" << cleanLine << "\" << std::endl;" << std::endl;
    }
  sout << "    return EXIT_SUCCESS;" << std::endl;
  sout << "    }" << std::endl;

}

void GenerateTCLAP(std::ofstream &sout)
{

  // Add a swtich argument to produce xml output
  CommandLineArg xmlSwitch;
  xmlSwitch.m_Type = "bool";
  xmlSwitch.m_Variable = "xmlSwitch";
  xmlSwitch.m_LongFlag = "--xml";
  xmlSwitch.m_Description = "Produce xml description of command line arguments";
  xmlSwitch.m_Default = "false";
  allArgs.push_back (xmlSwitch);
  
  // First pass generates argument declarations
  for (unsigned int i = 0; i < allArgs.size(); i++)
    {
    if (allArgs[i].NeedsQuotes())
      {
      sout << "    "
           << "std::string"
           << " "
           << allArgs[i].m_Variable;
      if (allArgs[i].NeedsTemp())
        {
        sout << "Temp";
        }
      if (allArgs[i].m_Default.empty())
        {    
        sout << ";"
             << std::endl;
        }
      else
        {
        sout << " = "
             << "\""
             << allArgs[i].m_Default
             << "\""
             << ";"
             << std::endl;
        }
      if (allArgs[i].NeedsTemp())
        {
        sout << "    "
             << allArgs[i].m_Type
             << " "
             << allArgs[i].m_Variable
             << ";"
             << std::endl;
        }
      }
    else
      {
      sout << "    "
           << allArgs[i].m_Type
           << " "
           << allArgs[i].m_Variable;
      if (allArgs[i].m_Default.empty())
        {    
        sout << ";"
             << std::endl;
        }
      else
        {
        sout << " = "
             << allArgs[i].m_Default
             << ";"
             << std::endl;
        }
      }
    }

  sout << "try" << std::endl;
  sout << "  {" << std::endl;
  sout << "    TCLAP::CmdLine commandLine (" << std::endl;
  sout << "      argv[0]," << std::endl;
  sout << "      " << "\"" << description << "\"," << std::endl;
  sout << "      " << "\"$Revision: $\" );" << std::endl << std::endl;
  sout << "    itk::OStringStream msg;" << std::endl;

  // Second pass generates argument declarations
  for (unsigned int i = 0; i < allArgs.size(); i++)
    {
    sout << "    msg.str(\"\");";
    sout << "msg << "
         << "\""
         << allArgs[i].m_Description;
    if (allArgs[i].m_Default.empty())
      {
      sout << "\";";
      }
    else
      {
      sout << " (default: \" "
           << "<< "
           << allArgs[i].m_Variable;
      if (allArgs[i].NeedsTemp())
        {
        sout << "Temp";
        }
      sout << " << "
           << "\")"
           << "\";"
           << std::endl;
      }

    if (allArgs[i].m_Type == "bool")
      {
      sout << "    TCLAP::SwitchArg "
           << allArgs[i].m_Variable
           << "Arg" << "(\""
           << allArgs[i].m_ShortFlag.replace(0,1,"")
           << "\", \"" 
           << allArgs[i].m_LongFlag.replace(0,2,"")
           << "\", msg.str(), "
           << allArgs[i].m_Default.empty()
           << ", "
           << "commandLine);"
           << std::endl << std::endl;
      }
    else
      {
      if (allArgs[i].m_ShortFlag.empty() && allArgs[i].m_LongFlag.empty())
        {
        sout << "    TCLAP::UnlabeledValueArg<";
        sout << allArgs[i].m_Type;
        sout << "> "
             << allArgs[i].m_Variable
             << "Arg" << "(\""
             << allArgs[i].m_Variable
             << "\", msg.str(), "
             << allArgs[i].m_Default.empty()
             << ", "
             << allArgs[i].m_Variable;
        sout << ", "
             << "\""
             << allArgs[i].m_Type
             << "\""
             << ", "
             << "commandLine);"
             << std::endl << std::endl;
        }
      else
        {
        sout << "    TCLAP::ValueArg<";
        if (allArgs[i].NeedsTemp())
          {
          sout << "std::string";
          }
        else
          {
          sout << allArgs[i].m_Type;
          }
        sout << "> "
             << allArgs[i].m_Variable
             << "Arg" << "(\""
             << allArgs[i].m_ShortFlag.replace(0,1,"")
             << "\", \"" 
             << allArgs[i].m_LongFlag.replace(0,2,"")
             << "\", msg.str(), "
             << allArgs[i].m_Default.empty()
             << ", "
             << allArgs[i].m_Variable;
        if (allArgs[i].NeedsTemp())
          {
          sout << "Temp";
          }
        sout << ", "
             << "\""
             << allArgs[i].m_Type
             << "\""
             << ", "
             << "commandLine);"
             << std::endl << std::endl;
        }
      }
    }
  sout << "    commandLine.parse ( argc, (char**) argv );" << std::endl;
  
  // Third pass generates access to arguments
  for (unsigned int i = 0; i < allArgs.size(); i++)
    {
    sout << "    "
         << allArgs[i].m_Variable;
    if (allArgs[i].NeedsTemp())
      {
      sout << "Temp";
      }
    sout << " = "
         << allArgs[i].m_Variable
         << "Arg.getValue();"
         << std::endl;
    }

// Finally, for any arrays, split the strings into words
  for (unsigned int i = 0; i < allArgs.size(); i++)
    {
    if (allArgs[i].NeedsTemp())
      {
      sout << "      {" << std::endl;
      sout << "      std::vector<std::string> words;"
           << std::endl;
      sout << "      std::string sep(\",\");"
           << std::endl;
      sout << "      splitString(" 
           << allArgs[i].m_Variable
           << "Temp"
           << ", "
           << "sep, "
           << "words);"
           << std::endl;
      sout << "      for (unsigned int j = 0; j < words.size(); j++)"
           << std::endl;
      sout << "        {"
           << std::endl;
      sout << "        " 
           << allArgs[i].m_Variable << ".push_back("
           << allArgs[i].m_StringToType
           << "(words[j].c_str()));"
           << std::endl;
      sout << "        }"
           << std::endl;
      sout << "      }"
           << std::endl;
      }
    }

  // Wrapup the block and generate the catch block
  sout << "  }" << std::endl;
  sout << "catch ( TCLAP::ArgException e )" << std::endl;
  sout << "  {" << std::endl;
  sout << "  std::cerr << \"error: \" << e.error() << \" for arg \" << e.argId() << std::endl;" << std::endl;
  sout << "  exit ( EXIT_FAILURE );" << std::endl;
  sout << "    }" << std::endl;
}

