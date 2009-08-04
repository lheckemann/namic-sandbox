/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkPerkStationModuleLogic.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationModule.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"

#include <itksys/SystemTools.hxx>

#include <algorithm>

#include <vtksys/stl/string>
#include <vtksys/SystemTools.hxx>


vtkPerkStationModuleLogic* vtkPerkStationModuleLogic::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPerkStationModuleLogic");
  if(ret)
    {
      return (vtkPerkStationModuleLogic*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPerkStationModuleLogic;
}


//----------------------------------------------------------------------------
vtkPerkStationModuleLogic::vtkPerkStationModuleLogic()
{
  this->PerkStationModuleNode = NULL;
}

//----------------------------------------------------------------------------
vtkPerkStationModuleLogic::~vtkPerkStationModuleLogic()
{
  vtkSetMRMLNodeMacro(this->PerkStationModuleNode, NULL);
}

//----------------------------------------------------------------------------
void vtkPerkStationModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{
}
//---------------------------------------------------------------------------
bool vtkPerkStationModuleLogic::ReadConfigFile(istream &file)
{
  bool mat1read = false;
  bool mat2read = false;

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
        if (!strcmp(attName, "NeedleToolPort"))
            {
            std::stringstream ss;
            ss << attValue;
            int val;
            ss >> val;              
            this->GetPerkStationModuleNode()->SetNeedleToolPort(val);          
            }
        else if (!strcmp(attName, "ReferenceBodyToolPort"))
            {
            std::stringstream ss;
            ss << attValue;
            int val;
            ss >> val;              
            this->GetPerkStationModuleNode()->SetReferenceBodyToolPort(val);              
            }
        else if (!strcmp(attName, "TrackerToPhantomMatrix"))
            {
            // read data into a temporary vector
            std::stringstream ss;
            ss << attValue;
            double d;
            std::vector<double> tmpVec;
            while (ss >> d)
            {
            tmpVec.push_back(d);
            }
            if (tmpVec.size()==16)
            {
            vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
            for (unsigned int i = 0; i < 4; i++)
                for (unsigned int j = 0; j < 4; j++)
                    matrix->SetElement(i,j,tmpVec[4*i+j]);
            this->GetPerkStationModuleNode()->SetTrackerToPhantomMatrix(matrix);
            mat1read = true;
            }
            else
            {
            // error in file?
            }
            }
        else if (!strcmp(attName, "PhantomToImageMatrix"))
            {
            // read data into a temporary vector
            std::stringstream ss;
            ss << attValue;
            double d;
            std::vector<double> tmpVec;
            while (ss >> d)
            {
            tmpVec.push_back(d);
            }
            if (tmpVec.size()==16)
            {
            vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
            for (unsigned int i = 0; i < 4; i++)
                for (unsigned int j = 0; j < 4; j++)
                    matrix->SetElement(i,j,tmpVec[4*i+j]);
            this->GetPerkStationModuleNode()->SetPhantomToImageRASMatrix(matrix);
            mat2read = true;
            
            }
            else
            {
            // error in file?
            }
            }
        else
            {
            // error in file?
            }     
        
        }// end if testing for it is a valid attName

    } // end while going through the file

  if (mat1read && mat2read)
      return true;
  else
      return false;
  
}
//----------------------------------------------------------------------------------
char *vtkPerkStationModuleLogic::strrev(char *s,int n)
{
       int i=0;
       while (i<n/2)
       {
               *(s+n) = *(s+i);       //uses the null character as the temporary storage.
               *(s+i) = *(s + n - i -1);
               *(s+n-i-1) = *(s+n);
               i++;
       }
       *(s+n) = '\0';

       return s;
}
