/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkITK
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/vtkITK/vtkITKImageToImageFilter2DFF.h $
  Date:      $Date: 2006-12-21 21:21:52 +0900 (木, 21 12 2006) $
  Version:   $Revision: 1900 $

==========================================================================*/

// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __vtkITKImageToImageFilterFF_h
#define __vtkITKImageToImageFilterFF_h


#include "vtkITKImageToImageFilter.h"
#include "vtkImageToImageFilter.h"
#include "itkImageToImageFilter.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "vtkITKUtility.h"


class VTK_ITK_EXPORT vtkITKImageToImageFilter2DFF : public vtkITKImageToImageFilter
{
public:
  vtkTypeMacro(vtkITKImageToImageFilter2DFF,vtkITKImageToImageFilter);
  static vtkITKImageToImageFilter2DFF* New() { return 0; };
  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
    os << m_Filter;
  };

protected:
  //BTX
  
  // To/from ITK
  typedef float InputImagePixelType;
  typedef float OutputImagePixelType;
  typedef itk::Image<InputImagePixelType, 2> InputImageType;
  typedef itk::Image<OutputImagePixelType, 2> OutputImageType;

  typedef itk::VTKImageImport<InputImageType> ImageImportType;
  typedef itk::VTKImageExport<OutputImageType> ImageExportType;
  ImageImportType::Pointer itkImporter;
  ImageExportType::Pointer itkExporter;

  typedef itk::ImageToImageFilter<InputImageType,OutputImageType> GenericFilterType;
  GenericFilterType::Pointer m_Filter;

  vtkITKImageToImageFilter2DFF ( GenericFilterType* filter )
  {
    // Need an import, export, and a ITK pipeline
    m_Filter = filter;
    this->itkImporter = ImageImportType::New();
    this->itkExporter = ImageExportType::New();
    ConnectPipelines(this->vtkExporter, this->itkImporter);
    ConnectPipelines(this->itkExporter, this->vtkImporter);
    this->LinkITKProgressToVTKProgress ( m_Filter );
    
    // Set up the filter pipeline
    m_Filter->SetInput ( this->itkImporter->GetOutput() );
    this->itkExporter->SetInput ( m_Filter->GetOutput() );
    this->vtkCast->SetOutputScalarTypeToFloat ();
  };

  ~vtkITKImageToImageFilter2DFF()
  {
  };
  //ETX
  
private:
  vtkITKImageToImageFilter2DFF(const vtkITKImageToImageFilter2DFF&);  // Not implemented.
  void operator=(const vtkITKImageToImageFilter2DFF&);  // Not implemented.
};

#endif



