/*=========================================================================

  Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   GenerateCLP
  Module:    $RCSfile: vtkSlicerBase.h,v $
  Date:      $Date: 2006/01/06 17:56:51 $
  Version:   $Revision: 1.6 $

=========================================================================*/

/* Generate command line processing code from an xml description */

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "expat.h"
#include <string>
#include <vector>

#include "GenerateCLP.h"

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

class ParserState
{
public:
  std::vector<CommandLineArg> m_AllArgs;
  std::string m_LastTag;
  std::string m_Description;
  CommandLineArg *m_Current;
  bool m_Debug;
  ParserState():m_Debug(false){};
};
  
void GeneratePre(std::ofstream &, int, char *[]);
void GeneratePost(std::ofstream &);
void GenerateXML(std::ofstream &, std::string);
void GenerateTCLAP(std::ofstream &, ParserState &);
void GenerateEchoArgs(std::ofstream &, ParserState &);

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
  ParserState *ps = reinterpret_cast<ParserState *>(userData);
  CommandLineArg *arg = ps->m_Current;
  if (ps->m_Debug) std::cout << name << std::endl;

  ps->m_LastTag.clear();

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
  else if (strcmp(name, "image") == 0)
    {
    arg = new CommandLineArg;
    arg->m_Type = "std::string";
    }
  ps->m_Current = arg;
}

static void
endElement(void *userData, const char *name)
{
  ParserState *ps = reinterpret_cast<ParserState *>(userData);
  CommandLineArg *arg = ps->m_Current;

  if (strcmp(name, "integer") == 0)
    {
    ps->m_AllArgs.push_back(*arg);
    ps->m_Current = 0;
    }
  else if (strcmp(name, "float") == 0)
    {
    ps->m_AllArgs.push_back(*arg);
    ps->m_Current = 0;
    }
  else if (strcmp(name, "double") == 0)
    {
    ps->m_AllArgs.push_back(*arg);
    ps->m_Current = 0;
    }
  else if (strcmp(name, "string") == 0)
    {
    ps->m_AllArgs.push_back(*arg);
    ps->m_Current = 0;
    }
  else if (strcmp(name, "boolean") == 0)
    {
    ps->m_AllArgs.push_back(*arg);
    ps->m_Current = 0;
    }
  else if (strcmp(name, "filename") == 0)
    {
    ps->m_AllArgs.push_back(*arg);
    ps->m_Current = 0;
    }
  else if (strcmp(name, "image") == 0)
    {
    ps->m_AllArgs.push_back(*arg);
    ps->m_Current = 0;
    }
  else if (strcmp(name, "integer-vector") == 0)
    {
    ps->m_AllArgs.push_back(*arg);
    ps->m_Current = 0;
    }
  else if (strcmp(name, "float-vector") == 0)
    {
    ps->m_AllArgs.push_back(*arg);
    ps->m_Current = 0;
    }
  else if (strcmp(name, "double-vector") == 0)
    {
    ps->m_AllArgs.push_back(*arg);
    ps->m_Current = 0;
    }
  else if (strcmp(name, "flag") == 0)
    {
    arg->m_ShortFlag = ps->m_LastTag;
    trimLeading(arg->m_ShortFlag);
    trimTrailing(arg->m_ShortFlag);
    }
  else if (strcmp(name, "longflag") == 0)
    {
    arg->m_LongFlag = ps->m_LastTag;
    trimLeading(arg->m_LongFlag);
    trimTrailing(arg->m_LongFlag);
    if (arg->m_Variable.empty())
      {
      arg->m_Variable = std::string(ps->m_LastTag);
      trimLeading(arg->m_Variable);
      trimTrailing(arg->m_Variable);
      trimLeading(arg->m_Variable,"-");
      }
    }
  else if (strcmp(name, "name") == 0)
    {
    if (ps->m_Debug) std::cout << "--------------------" << ps->m_LastTag << std::endl;
    arg->m_Variable = std::string(ps->m_LastTag);
    trimLeading(arg->m_Variable);
    trimTrailing(arg->m_Variable);
    }
  else if (strcmp(name, "description") == 0)
    {
    if (arg)
      {
      arg->m_Description = ps->m_LastTag;
      trimLeading(arg->m_Description);
      trimTrailing(arg->m_Description);
      }
    else
      {
      if (ps->m_Description.empty())
        {
        ps->m_Description = ps->m_LastTag;
        trimLeading(ps->m_Description);
        trimTrailing(ps->m_Description);
        }
      }
    }
  else if (strcmp(name, "default") == 0)
    {
    arg->m_Default = ps->m_LastTag;
    trimLeading(arg->m_Default);
    trimTrailing(arg->m_Default);
    }
}

static void
charData(void *userData, const char *s, int len)
{
  ParserState *ps = reinterpret_cast<ParserState *>(userData);
  if (len)
    {
    std::string str(s,len);
    ps->m_LastTag += str;
    if (ps->m_Debug) std::cout << "|" << str << "|" << std::endl;
    }
}

int
main(int argc, char *argv[])
{
#include "GenerateCLP.clp"

  ParserState parserState;

  XML_Parser parser = XML_ParserCreate(NULL);
  int done;
  int depth = 0;
  parserState.m_Current = 0;

  XML_SetUserData(parser, static_cast<void *>(&parserState));
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
              "%s at line %d\n",
              XML_ErrorString(XML_GetErrorCode(parser)),
              XML_GetCurrentLineNumber(parser));
      return 1;
    }
  } while (!done);
  fin.close();
  XML_ParserFree(parser);

// Print each command line arg
  std::cerr << "Found " << parserState.m_AllArgs.size() << " command line arguments" << std::endl;
// Do the hard stuff
  std::ofstream sout(OutputCxx.c_str(),std::ios::out);
  GeneratePre(sout, argc, argv);
  GenerateXML(sout, InputXML);
  GenerateTCLAP(sout, parserState);
  GenerateEchoArgs(sout, parserState);
  GeneratePost(sout);

  return (EXIT_SUCCESS);
}

void GeneratePre(std::ofstream &sout, int argc, char *argv[])
{
  sout << "// This file was automatically generated by:" << std::endl;
  sout << "// ";
  for (unsigned int i = 0; i < argc; i++)
    {
    sout << " " << argv[i];
    }
  sout << std::endl;
  sout << "//" << std::endl;
  sout << "#include <stdio.h>" << std::endl;
  sout << "#include <string.h>" << std::endl;
  sout << "#include <stdlib.h>" << std::endl;
  sout << "" << std::endl;
  sout << "#include <iostream>" << std::endl;
  sout << "#include \"tclap/CmdLine.h\"" << std::endl;
  sout << "#include <itksys/ios/sstream>" << std::endl;
  sout << "" << std::endl;
  sout << "void" << std::endl;
  sout << "splitString (std::string &text," << std::endl;
  sout << "             std::string &separators," << std::endl;
  sout << "             std::vector<std::string> &words)" << std::endl;
  sout << "{" << std::endl;
  sout << "  int n = text.length();" << std::endl;
  sout << "  int start, stop;" << std::endl;
  sout << "  start = text.find_first_not_of(separators);" << std::endl;
  sout << "  while ((start >= 0) && (start < n))" << std::endl;
  sout << "    {" << std::endl;
  sout << "    stop = text.find_first_of(separators, start);" << std::endl;
  sout << "    if ((stop < 0) || (stop > n)) stop = n;" << std::endl;
  sout << "    words.push_back(text.substr(start, stop - start));" << std::endl;
  sout << "    start = text.find_first_not_of(separators, stop+1);" << 
    std::endl;
  sout << "    }" << std::endl;
  sout << "}" << std::endl;

}

void GenerateXML(std::ofstream &sout, std::string XMLFile)
{
  std::string EOL(" \\");
  char linec[2048];
  std::ifstream fin(XMLFile.c_str(),std::ios::in);

  sout << "#define GENERATE_XML \\" << std::endl;
  // Generate special section to produce xml description
  sout << "  if (argc >= 2 && (strcmp(argv[1],\"--xml\") == 0))" << EOL << std::endl;
  sout << "    {" << EOL << std::endl;

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
    sout << "std::cout << \"" << cleanLine << "\" << std::endl;" << EOL << std::endl;
    }
  sout << "    return EXIT_SUCCESS;" << EOL << std::endl;
  sout << "    }" << std::endl;

}

void GenerateEchoArgs(std::ofstream &sout, ParserState &ps)
{
  std::string EOL(" \\");
  sout << "#define GENERATE_ECHOARGS \\" << std::endl;

  sout << "if (echoSwitch)" << EOL << std::endl;
  sout << "{" << EOL << std::endl;
  sout << "std::cout << \"Command Line Arguments\" << std::endl;" << EOL << std::endl;
  for (unsigned int i = 0; i < ps.m_AllArgs.size(); i++)
    {
    if (ps.m_AllArgs[i].NeedsTemp())
      {
      sout << "std::cout << "
           << "\"    "
           << ps.m_AllArgs[i].m_Variable
           << ": \";"
           << EOL << std::endl;
      sout << "for (unsigned int _i =0; _i < "
           << ps.m_AllArgs[i].m_Variable
           << ".size(); _i++)"
           << EOL << std::endl;
      sout << "{"
           << EOL << std::endl;
      sout << "std::cout << "
           << ps.m_AllArgs[i].m_Variable
           << "[_i]"
           << " << \", \";"
           << EOL << std::endl;
      sout << "}"
           << EOL << std::endl;
      sout << "std::cout <<std::endl;"
           << EOL << std::endl;

      }
    else
      {
      sout << "std::cout << "
           << "\"    "
           << ps.m_AllArgs[i].m_Variable
           << ": \" << "
           << ps.m_AllArgs[i].m_Variable
           << " << std::endl;"
           << EOL << std::endl;
      }
    }
  sout << "}" << std::endl;
}

void GenerateTCLAP(std::ofstream &sout, ParserState &ps)
{

  std::string EOL(" \\");
  sout << "#define GENERATE_TCLAP \\" << std::endl;

  // Add a switch argument to echo command line arguments
  CommandLineArg echoSwitch;
  echoSwitch.m_Type = "bool";
  echoSwitch.m_Variable = "echoSwitch";
  echoSwitch.m_LongFlag = "--echo";
  echoSwitch.m_Description = "Echo the command line arguments";
  echoSwitch.m_Default = "false";
  ps.m_AllArgs.push_back (echoSwitch);

  // Add a switch argument to produce xml output
  CommandLineArg xmlSwitch;
  xmlSwitch.m_Type = "bool";
  xmlSwitch.m_Variable = "xmlSwitch";
  xmlSwitch.m_LongFlag = "--xml";
  xmlSwitch.m_Description = "Produce xml description of command line arguments";
  xmlSwitch.m_Default = "false";
  ps.m_AllArgs.push_back (xmlSwitch);
  
  // First pass generates argument declarations
  for (unsigned int i = 0; i < ps.m_AllArgs.size(); i++)
    {
    if (ps.m_AllArgs[i].NeedsQuotes())
      {
      sout << "    "
           << "std::string"
           << " "
           << ps.m_AllArgs[i].m_Variable;
      if (ps.m_AllArgs[i].NeedsTemp())
        {
        sout << "Temp";
        }
      if (ps.m_AllArgs[i].m_Default.empty())
        {    
        sout << ";"
             << EOL << std::endl;
        }
      else
        {
        sout << " = "
             << "\""
             << ps.m_AllArgs[i].m_Default
             << "\""
             << ";"
             << EOL << std::endl;
        }
      if (ps.m_AllArgs[i].NeedsTemp())
        {
        sout << "    "
             << ps.m_AllArgs[i].m_Type
             << " "
             << ps.m_AllArgs[i].m_Variable
             << ";"
             << EOL << std::endl;
        }
      }
    else
      {
      sout << "    "
           << ps.m_AllArgs[i].m_Type
           << " "
           << ps.m_AllArgs[i].m_Variable;
      if (ps.m_AllArgs[i].m_Default.empty())
        {    
        sout << ";"
             << EOL << std::endl;
        }
      else
        {
        sout << " = "
             << ps.m_AllArgs[i].m_Default
             << ";"
             << EOL << std::endl;
        }
      }
    }

  sout << "try" << EOL << std::endl;
  sout << "  {" << EOL << std::endl;
  sout << "    TCLAP::CmdLine commandLine (" << EOL << std::endl;
  sout << "      argv[0]," << EOL << std::endl;
  sout << "      " << "\"" << ps.m_Description << "\"," << EOL << std::endl;
  sout << "      " << "\"$Revision: $\" );" << EOL << std::endl << EOL << std::endl;
  sout << "      itksys_ios::ostringstream msg;" << EOL << std::endl;

  // Second pass generates argument declarations
  for (unsigned int i = 0; i < ps.m_AllArgs.size(); i++)
    {
    sout << "    msg.str(\"\");";
    sout << "msg << "
         << "\""
         << ps.m_AllArgs[i].m_Description;
    if (ps.m_AllArgs[i].m_Default.empty())
      {
      sout << "\";";
      }
    else
      {
      sout << " (default: \" "
           << "<< "
           << ps.m_AllArgs[i].m_Variable;
      if (ps.m_AllArgs[i].NeedsTemp())
        {
        sout << "Temp";
        }
      sout << " << "
           << "\")"
           << "\";"
           << EOL << std::endl;
      }

    if (ps.m_AllArgs[i].m_Type == "bool")
      {
      sout << "    TCLAP::SwitchArg "
           << ps.m_AllArgs[i].m_Variable
           << "Arg" << "(\""
           << ps.m_AllArgs[i].m_ShortFlag.replace(0,1,"")
           << "\", \"" 
           << ps.m_AllArgs[i].m_LongFlag.replace(0,2,"")
           << "\", msg.str(), "
           << ps.m_AllArgs[i].m_Default.empty()
           << ", "
           << "commandLine);"
           << EOL << std::endl << EOL << std::endl;
      }
    else
      {
      if (ps.m_AllArgs[i].m_ShortFlag.empty() && ps.m_AllArgs[i].m_LongFlag.empty())
        {
        sout << "    TCLAP::UnlabeledValueArg<";
        sout << ps.m_AllArgs[i].m_Type;
        sout << "> "
             << ps.m_AllArgs[i].m_Variable
             << "Arg" << "(\""
             << ps.m_AllArgs[i].m_Variable
             << "\", msg.str(), "
             << ps.m_AllArgs[i].m_Default.empty()
             << ", "
             << ps.m_AllArgs[i].m_Variable;
        sout << ", "
             << "\""
             << ps.m_AllArgs[i].m_Type
             << "\""
             << ", "
             << "commandLine);"
             << EOL << std::endl << EOL << std::endl;
        }
      else
        {
        sout << "    TCLAP::ValueArg<";
        if (ps.m_AllArgs[i].NeedsTemp())
          {
          sout << "std::string";
          }
        else
          {
          sout << ps.m_AllArgs[i].m_Type;
          }
        sout << "> "
             << ps.m_AllArgs[i].m_Variable
             << "Arg" << "(\""
             << ps.m_AllArgs[i].m_ShortFlag.replace(0,1,"")
             << "\", \"" 
             << ps.m_AllArgs[i].m_LongFlag.replace(0,2,"")
             << "\", msg.str(), "
             << ps.m_AllArgs[i].m_Default.empty()
             << ", "
             << ps.m_AllArgs[i].m_Variable;
        if (ps.m_AllArgs[i].NeedsTemp())
          {
          sout << "Temp";
          }
        sout << ", "
             << "\""
             << ps.m_AllArgs[i].m_Type
             << "\""
             << ", "
             << "commandLine);"
             << EOL << std::endl << EOL << std::endl;
        }
      }
    }
  sout << "    commandLine.parse ( argc, (char**) argv );" << EOL << std::endl;
  
  // Third pass generates access to arguments
  for (unsigned int i = 0; i < ps.m_AllArgs.size(); i++)
    {
    sout << "    "
         << ps.m_AllArgs[i].m_Variable;
    if (ps.m_AllArgs[i].NeedsTemp())
      {
      sout << "Temp";
      }
    sout << " = "
         << ps.m_AllArgs[i].m_Variable
         << "Arg.getValue();"
         << EOL << std::endl;
    }

// Finally, for any arrays, split the strings into words
  for (unsigned int i = 0; i < ps.m_AllArgs.size(); i++)
    {
    if (ps.m_AllArgs[i].NeedsTemp())
      {
      sout << "      {" << EOL << std::endl;
      sout << "      std::vector<std::string> words;"
           << EOL << std::endl;
      sout << "      std::string sep(\",\");"
           << EOL << std::endl;
      sout << "      splitString(" 
           << ps.m_AllArgs[i].m_Variable
           << "Temp"
           << ", "
           << "sep, "
           << "words);"
           << EOL << std::endl;
      sout << "      for (unsigned int j = 0; j < words.size(); j++)"
           << EOL << std::endl;
      sout << "        {"
           << EOL << std::endl;
      sout << "        " 
           << ps.m_AllArgs[i].m_Variable << ".push_back("
           << ps.m_AllArgs[i].m_StringToType
           << "(words[j].c_str()));"
           << EOL << std::endl;
      sout << "        }"
           << EOL << std::endl;
      sout << "      }"
           << EOL << std::endl;
      }
    }

  // Wrapup the block and generate the catch block
  sout << "  }" << EOL << std::endl;
  sout << "catch ( TCLAP::ArgException e )" << EOL << std::endl;
  sout << "  {" << EOL << std::endl;
  sout << "  std::cerr << \"error: \" << e.error() << \" for arg \" << e.argId() << std::endl;" << EOL << std::endl;
  sout << "  exit ( EXIT_FAILURE );" << EOL << std::endl;
  sout << "    }" << std::endl;
}

void GeneratePost(std::ofstream &sout)
{
  sout << "#define PARSE_ARGS GENERATE_XML;GENERATE_TCLAP;GENERATE_ECHOARGS" << std::endl;
}
