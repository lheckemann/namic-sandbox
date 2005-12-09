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

#include "itkTransform.h" 
#include "itkImageBase.h" 
#include "itkObject.h" 

#include <time.h>
#include <pthread.h>

namespace Slicer
{

/** \brief This is a draft prototype of the DataModelManager to be used as the core element of Slicer 3.0 
     
     \class DataModelManager  This class provides services for finding data
elements that were loaded from a MRML data structure. It also provides services
for computing Transform paths between nodes.


*/

class DataModelManager
{
public:

    
  /** Type for naming the nodes */
  typedef std::string   NodeNameType;

  /** Type for naming the elements */
  typedef std::string   ElementNameType;

  /** Base Type for Transform hierarchy */
  typedef ::itk::TransformBase   TransformType;

  /** Base Type for Image hierarchy */
  typedef ::itk::ImageBase<3>   ImageType;

  typedef pthread_t               ThreadType;

  /** Request to add a name with a node */
  void RequestAddNode( const NodeNameType & nodeName );

  /** Request attaching a data type with a name to a named node */
  void RequestAddTransformToNode( const NodeNameType & nodeName,
                                  const ElementNameType & name,
                                  const TransformType * transform );

  /** Request attaching a dataset with a name to a named node */
  void RequestAddImageToNode(     const NodeNameType & nodeName,
                                  const ElementNameType & name,
                                  const ImageType * transform );


  /** Request Transform between named nodes */
  void RequestGetTransformBetweenNodes( const NodeNameType & nodeName,
                                        const NodeNameType & nodeName );

  /** Constructor */
  DataModelManager();

  /** Destructor */
  ~DataModelManager();

   static void * StartRunning(void *arg);

private:

  /** Send Transform between two nodes in the payload of an Event */
  void SendTransformBetweenNodes() const;

  ThreadType   m_Thread;

};



}



