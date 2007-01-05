/*=========================================================================

Program: Insight Segmentation & Registration Toolkit
Module: $RCSfile: KnowledgeBasedSegmentation.cxx,v $
Language: C++
Date: $Date: 2005/09/17 20:12:37 $
Version: $Revision: 1.0 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notices for more information.

=========================================================================*/


#include "DataModelManager.h"

int main( int argc , char * argv [] )
{

  Slicer::DataModelManager manager;

  typedef Slicer::DataModelManager::TransformType     TransformType;
  typedef Slicer::DataModelManager::ImageType         ImageType;
  typedef Slicer::DataModelManager::NodeNameType      NodeNameType;
  typedef Slicer::DataModelManager::ElementNameType   ElementNameType;

  NodeNameType     nodeName1    = "Slicer.patient";
  ElementNameType  elementName1 = "MRI";

  manager.RequestAddNode( nodeName1 );

  return 0;
}

