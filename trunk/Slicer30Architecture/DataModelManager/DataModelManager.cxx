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
  pthread_create (&m_Thread, NULL, StartRunning, 0);


  /** Initializing SQLite */
  
  m_DataBase = NULL;


  const int status = sqlite3_open("Slicer3", & m_DataBase);

  if( status )
    {
    std::cerr << "Can't open database: " << sqlite3_errmsg( m_DataBase ) << std::endl;
    sqlite3_close( m_DataBase );
  }



  /** Initializing MetaKit */
  
  m_MetaKitStorage = NULL;
  
  m_MetaKitStorage = new c4_Storage("Slicer3.dat",true);

}

DataModelManager::~DataModelManager()
{
  sqlite3_close( m_DataBase );
  
//pthread_delete ( m_Thread);

  m_MetaKitStorage->Commit();
  delete m_MetaKitStorage;
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
RequestAddImageToNode( const NodeNameType & ,
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



void * 
DataModelManager::
StartRunning(void *arg)
{
  struct sched_param p;
  p . sched_priority = 1;
  pthread_setschedparam (pthread_self(), SCHED_FIFO, &p);

//    pthread_make_periodic (pthread_self(), 1, 500000000);

  while (1) 
    {
//     pthread_wait ();
    printf("I'm here; my arg is %x\n", (unsigned) arg);
    }
  return 0;
}





}  // end of Slicer namespace 




