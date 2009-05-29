/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/
#include "vtkObjectFactory.h"
#include "vtkTRProstateBiopsyConfigFile.h"
#include "vtkMatrix4x4.h"
#include "vtkMath.h"
#include "itkPoint.h"

#include <itksys/SystemTools.hxx>

#include "vtkMRMLTRProstateBiopsyModuleNode.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyConfigFile);
vtkCxxRevisionMacro(vtkTRProstateBiopsyConfigFile, "$Revision: 1.0 $");
//------------------------------------------------------------------------------

vtkTRProstateBiopsyConfigFile::vtkTRProstateBiopsyConfigFile()
{
}
  
vtkTRProstateBiopsyConfigFile::~vtkTRProstateBiopsyConfigFile()
{
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyConfigFile::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

bool vtkTRProstateBiopsyConfigFile::ReadConfigFile(vtkMRMLTRProstateBiopsyModuleNode *node)
{
  // read the MR config file

  // assumption
    std::string curDir = itksys::SystemTools::GetCurrentWorkingDirectory();

  if (itksys::SystemTools::FileExists("TRPBConfig.cfg"))
  {
  ifstream file("TRPBConfig.cfg");
  char currentLine[256];  
  char* attName = "";
  char* attValue = "";
  char* pdest;
  int nCharCount = 0;
  unsigned int indexEndOfAttribute = 0;
  unsigned int indexStartOfValue = 0;
  unsigned int indexEndOfValue = 0;

  int paramSetCount = 0;
  while(!file.eof())
     {
     // first get each line,
     // then parse each line on basis of attName, and attValue
     // this can be done as delimiters '='[]' is used to separate out name from value
        file.getline(&currentLine[0], 256, '\n');   
        nCharCount = strlen(currentLine);
        indexEndOfAttribute = strcspn(currentLine,"=");
        if(indexEndOfAttribute >0)
          {
          attName = new char[indexEndOfAttribute+1];
          strncpy(attName, currentLine,indexEndOfAttribute);
          attName[indexEndOfAttribute] = '\0';
          pdest = strchr(currentLine, '"');   
          indexStartOfValue = (int)(pdest - currentLine + 1);
          pdest = strrchr(currentLine, '"');
          indexEndOfValue = (int)(pdest - currentLine + 1);
          attValue = new char[indexEndOfValue-indexStartOfValue+1];
          strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
          attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

          // at this point, we have line separated into, attributeName, and attributeValue
          // now we need to do string matching on attributeName, and further parse attributeValue as it may have more than one value
          if (!strcmp(attName, "NumberOfNeedles"))
              {
              std::stringstream ss;
              ss << attValue;
              int val;
              ss >> val;              

              //node->SetNeedleCount(val); :TODO: implement

              if (val < 0)
                  break;
              char tagName[256]="";
              for (unsigned int i = 0; i < val; i++)
                {
                // read for each needle

                // form the tage name
                // needle type
                sprintf(tagName,"Needle%dType", i+1);
                
                file.getline(&currentLine[0], 256, '\n');   
                nCharCount = strlen(currentLine);
                indexEndOfAttribute = strcspn(currentLine,"=");
                if(indexEndOfAttribute >0)
                  {
                  attName = new char[indexEndOfAttribute+1];
                  strncpy(attName, currentLine,indexEndOfAttribute);
                  attName[indexEndOfAttribute] = '\0';
                  pdest = strchr(currentLine, '"');   
                  indexStartOfValue = (int)(pdest - currentLine + 1);
                  pdest = strrchr(currentLine, '"');
                  indexEndOfValue = (int)(pdest - currentLine + 1);
                  attValue = new char[indexEndOfValue-indexStartOfValue+1];
                  strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
                  attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

                  if (!strcmp(attName, tagName))
                    {
                    std::stringstream ss;
                    ss << attValue;
                    std::string str;
                    ss >> str;
                    node->SetNeedleType(i, str);
                    }
                  }

                // form the tag name
                // needle depth
                sprintf(tagName,"Needle%dDepth", i+1);
                ss.clear();
                file.getline(&currentLine[0], 256, '\n');   
                nCharCount = strlen(currentLine);
                indexEndOfAttribute = strcspn(currentLine,"=");
                if(indexEndOfAttribute >0)
                  {
                  attName = new char[indexEndOfAttribute+1];
                  strncpy(attName, currentLine,indexEndOfAttribute);
                  attName[indexEndOfAttribute] = '\0';
                  pdest = strchr(currentLine, '"');   
                  indexStartOfValue = (int)(pdest - currentLine + 1);
                  pdest = strrchr(currentLine, '"');
                  indexEndOfValue = (int)(pdest - currentLine + 1);
                  attValue = new char[indexEndOfValue-indexStartOfValue+1];
                  strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
                  attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

                  if (!strcmp(attName, tagName))
                    {
                    std::stringstream ss;
                    ss << attValue;
                    float val;
                    ss >> val;
                    node->SetNeedleDepth(i, val);
                    }
                  }

                // form the tage name
                // needle overshoot
                sprintf(tagName,"Needle%dOvershoot", i+1);
                ss.clear();
                file.getline(&currentLine[0], 256, '\n');   
                nCharCount = strlen(currentLine);
                indexEndOfAttribute = strcspn(currentLine,"=");
                if(indexEndOfAttribute >0)
                  {
                  attName = new char[indexEndOfAttribute+1];
                  strncpy(attName, currentLine,indexEndOfAttribute);
                  attName[indexEndOfAttribute] = '\0';
                  pdest = strchr(currentLine, '"');   
                  indexStartOfValue = (int)(pdest - currentLine + 1);
                  pdest = strrchr(currentLine, '"');
                  indexEndOfValue = (int)(pdest - currentLine + 1);
                  attValue = new char[indexEndOfValue-indexStartOfValue+1];
                  strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
                  attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

                  if (!strcmp(attName, tagName))
                    {
                    std::stringstream ss;
                    ss << attValue;
                    float val;
                    ss >> val;
                    node->SetNeedleOvershoot(i, val);
                    }
                  }

                // form the tage name
                // needle description
                sprintf(tagName,"Needle%dDescription", i+1);
                ss.clear();
                file.getline(&currentLine[0], 256, '\n');   
                nCharCount = strlen(currentLine);
                indexEndOfAttribute = strcspn(currentLine,"=");
                if(indexEndOfAttribute >0)
                  {
                  attName = new char[indexEndOfAttribute+1];
                  strncpy(attName, currentLine,indexEndOfAttribute);
                  attName[indexEndOfAttribute] = '\0';
                  pdest = strchr(currentLine, '"');   
                  indexStartOfValue = (int)(pdest - currentLine + 1);
                  pdest = strrchr(currentLine, '"');
                  indexEndOfValue = (int)(pdest - currentLine + 1);
                  attValue = new char[indexEndOfValue-indexStartOfValue+1];
                  strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
                  attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

                  if (!strcmp(attName, tagName))
                    {
                    std::stringstream ss;
                    ss << attValue; 
                    std::string str;
                    ss >> str;
                    node->SetNeedleDescription(i, str);
                    }
                  }

                // form the tage name
                // needle uid
                sprintf(tagName,"Needle%dUID", i+1);
                ss.clear();
                file.getline(&currentLine[0], 256, '\n');   
                nCharCount = strlen(currentLine);
                indexEndOfAttribute = strcspn(currentLine,"=");
                if(indexEndOfAttribute >0)
                  {
                  attName = new char[indexEndOfAttribute+1];
                  strncpy(attName, currentLine,indexEndOfAttribute);
                  attName[indexEndOfAttribute] = '\0';
                  pdest = strchr(currentLine, '"');   
                  indexStartOfValue = (int)(pdest - currentLine + 1);
                  pdest = strrchr(currentLine, '"');
                  indexEndOfValue = (int)(pdest - currentLine + 1);
                  attValue = new char[indexEndOfValue-indexStartOfValue+1];
                  strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
                  attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

                  if (!strcmp(attName, tagName))
                    {
                    std::stringstream ss;
                    ss << attValue;
                    unsigned int val;
                    ss >> val;
                    node->SetNeedleUID(i, val);
                    }
                  }
                }
              }
          else if (!strcmp(attName, "ExperimentSavePath"))
              {
              std::stringstream ss;
              ss << attValue;
              std::string str;
              ss >> str;
              node->SetExperimentSavePathInConfigFile(str.c_str());              
              }
            else
              {
              // error in file?
              }     
           
          }// end if testing for it is a valid attName

        } // end while going through the file
     
    file.close();
  }

  return true;
}
