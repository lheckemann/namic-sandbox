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

namespace mrml 
{

Tree::Tree()
{
}


Tree::~Tree()
{
}


void 
Tree::
RequestAddNode( const NodeNameType & ,
                const NodeNameType & )
{
}



void 
Tree::
RequestGetTransformBetweenNodes( const NodeNameType & ,
                                 const NodeNameType & )
{
}



}  // end of Slicer namespace 




