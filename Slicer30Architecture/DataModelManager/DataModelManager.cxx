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

namespace Slicer 
{

DataModelManager::DataModelManager()
{
}

DataModelManager::~DataModelManager()
{
}


void 
DataModelManager::
RequestAddNode( const NodeNameType & )
{
}

void 
DataModelManager::
RequestAddTransformToNode( const NodeNameType & ,
                           const ElementNameType & ,
                           const TransformType * )
{
}


void 
DataModelManager::
RequestAddTransformToNode( const NodeNameType & ,
                           const ElementNameType & ,
                           const ImageType * )
{
}



void 
DataModelManager::
RequestGetTransformBetweenNodes( const NodeNameType & ,
                                 const NodeNameType & )
{
}


}
