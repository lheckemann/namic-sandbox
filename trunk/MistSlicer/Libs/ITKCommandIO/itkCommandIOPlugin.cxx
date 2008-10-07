/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   ITKCommandIO
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/ITKCommandIO/itkCommandIOPlugin.cxx $
  Date:      $Date: 2008-01-15 03:18:54 +0900 (火, 15 1 2008) $
  Version:   $Revision: 5492 $

==========================================================================*/
#include "itkCommandIOPlugin.h"
#include "itkCommandIOFactory.h"

/**
 * Routine that is called when the shared library is loaded by
 * itk::ObjectFactoryBase::LoadDynamicFactories().
 *
 * itkLoad() is C (not C++) function.
 */
itk::ObjectFactoryBase* itkLoad()
{
  static itk::CommandIOFactory::Pointer f = itk::CommandIOFactory::New();
  return f;
}

