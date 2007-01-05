/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkImageSpatialObject.txx,v $
  Language:  C++
  Date:      $Date: 2005/12/10 21:37:27 $
  Version:   $Revision: 1.5 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstk_ImageSpatialObject_txx
#define __igstk_ImageSpatialObject_txx


#include "igstkImageSpatialObject.h"


namespace igstk
{ 

/** Constructor */
template< class TPixelType, unsigned int VDimension >
ImageSpatialObject< TPixelType, VDimension >
::ImageSpatialObject():m_StateMachine(this)
{
  // Create the image spatial object
  m_ImageSpatialObject = ImageSpatialObjectType::New();
  this->RequestSetSpatialObject( m_ImageSpatialObject );

  m_itkExporter = ITKExportFilterType::New();
  m_vtkImporter = VTKImportFilterType::New();
  
  // Connect the given itk::VTKImageExport filter to
  // the given vtkImageImport filter.
 
  m_vtkImporter->SetUpdateInformationCallback(m_itkExporter->GetUpdateInformationCallback());
  m_vtkImporter->SetPipelineModifiedCallback(m_itkExporter->GetPipelineModifiedCallback());
  m_vtkImporter->SetWholeExtentCallback(m_itkExporter->GetWholeExtentCallback());
  m_vtkImporter->SetSpacingCallback(m_itkExporter->GetSpacingCallback());
  m_vtkImporter->SetOriginCallback(m_itkExporter->GetOriginCallback());
  m_vtkImporter->SetScalarTypeCallback(m_itkExporter->GetScalarTypeCallback());
  m_vtkImporter->SetNumberOfComponentsCallback(m_itkExporter->GetNumberOfComponentsCallback());
  m_vtkImporter->SetPropagateUpdateExtentCallback(m_itkExporter->GetPropagateUpdateExtentCallback());
  m_vtkImporter->SetUpdateDataCallback(m_itkExporter->GetUpdateDataCallback());
  m_vtkImporter->SetDataExtentCallback(m_itkExporter->GetDataExtentCallback());
  m_vtkImporter->SetBufferPointerCallback(m_itkExporter->GetBufferPointerCallback());
  m_vtkImporter->SetCallbackUserData(m_itkExporter->GetCallbackUserData());

}


/** Destructor */
template< class TPixelType, unsigned int VDimension >
ImageSpatialObject< TPixelType, VDimension >
::~ImageSpatialObject()  
{
  if( m_vtkImporter )
    {
    m_vtkImporter->Delete(); 
    m_vtkImporter = NULL;
    }
}


template< class TPixelType, unsigned int VDimension >
const vtkImageData * 
ImageSpatialObject< TPixelType, VDimension >
::GetVTKImageData() const
{
  return m_vtkImporter->GetOutput();
}



template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::SetImage( const ImageType * image ) 
{
  m_Image = image;
  // This line should be added once a StateMachine in this class
  // guarrantees that the m_Image pointer is not null.
  m_ImageSpatialObject->SetImage( m_Image );
  m_itkExporter->SetInput( m_Image );
}



template< class TPixelType, unsigned int VDimension >
bool
ImageSpatialObject< TPixelType, VDimension >
::IsInside( const PointType & point ) const 
{ 
  return m_ImageSpatialObject->IsInside( point ); 
}



template< class TPixelType, unsigned int VDimension >
bool
ImageSpatialObject< TPixelType, VDimension >
::IsEmpty() const 
{ 
  typedef ::itk::ImageRegion< VDimension > RegionType;
  RegionType region = 
    m_ImageSpatialObject->GetImage()->GetLargestPossibleRegion();
  const unsigned int numberOfPixels = region.GetNumberOfPixels();
  const bool isEmpty = ( numberOfPixels == 0 );
  return isEmpty;
}



template< class TPixelType, unsigned int VDimension >
bool
ImageSpatialObject< TPixelType, VDimension >
::TransformPhysicalPointToIndex ( const PointType & point, 
                                        IndexType & index ) const 
{ 
  return m_Image->TransformPhysicalPointToIndex( point, index);  
}



/** Print Self function */
template< class TPixelType, unsigned int VDimension >
void 
ImageSpatialObject< TPixelType, VDimension >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Details of the image " << m_Image.GetPointer() << std::endl;
  os << "ITK Exporter filter " << m_itkExporter.GetPointer() << std::endl;
  os << "VTK Importer filter " << m_vtkImporter << std::endl;
}


} // end namespace igstk

#endif

