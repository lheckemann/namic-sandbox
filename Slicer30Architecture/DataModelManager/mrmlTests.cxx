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


#include "mrmlTree.h"

int main( int argc , char * argv [] )
{

  mrml::Tree  dataModel;

  typedef mrml::Tree::NodeNameType      NodeNameType;

  NodeNameType     nodeName1    = "Root";
  NodeNameType     nodeName2    = "patientA";
  NodeNameType     nodeName3    = "MRI";
  NodeNameType     nodeName4    = "CT";

  dataModel.RequestAddNode( nodeName1, nodeName2 );
  dataModel.RequestAddNode( nodeName2, nodeName3 );
  dataModel.RequestAddNode( nodeName2, nodeName4 );

  dataModel.RequestTransformCollectionBetweenNodes( nodeName3, nodeName4 );

  return 0;

}

