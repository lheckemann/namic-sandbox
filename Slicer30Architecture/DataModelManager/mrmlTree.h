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


#include <string> 



namespace mrml
{

/** \brief Tree data structure to be used as the core element of Slicer 3.0  Data Model.
     
     \class Tree  This class provides services for organizing together the
elements of a MRML scene. It also provides services for computing Transform
paths between nodes.

*/

class Tree
{
public:

  /** Type for naming the nodes */
  typedef std::string   NodeNameType;

  /** Request attaching a data type with a name to a named node */
  void RequestAddNode( const NodeNameType & nodeName,
                       const NodeNameType & parentNodeName );

  /** Request Transform between named nodes */
  void RequestTransformCollectionBetweenNodes( const NodeNameType & nodeName1,
                                               const NodeNameType & nodeName2 );

  /** Constructor */
  Tree();

  /** Destructor */
  ~Tree();

private:

};


}


