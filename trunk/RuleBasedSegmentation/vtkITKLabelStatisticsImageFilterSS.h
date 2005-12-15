/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKLabelStatisticsImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/03/27 16:59:58 $
  Version:   $Revision: 1.10 $
*/
// .NAME vtkITKLabelStatisticsImageFilter - Wrapper class around itk::LabelStatisticsImageFilterImageFilter
// .SECTION Description
// vtkITKLabelStatisticsImageFilter


#ifndef __vtkITKLabelStatisticsImageFilter_h
#define __vtkITKLabelStatisticsImageFilter_h


#include "vtkITKImageToImageFilterSS.h"
#include "itkLabelStatisticsImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKLabelStatisticsImageFilterSS : public vtkITKImageToImageFilterSS
{
 public:
  static vtkITKLabelStatisticsImageFilterSS *New();
  vtkTypeRevisionMacro(vtkITKLabelStatisticsImageFilterSS, vtkITKImageToImageFilterSS);

  void SetLabelInput ( vtkImageData *value)
  {
    this->vtkLabelExporter->SetInput(value);
  }

  double GetMinimum (short label)
  {
    return ((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetMinimum(label));
  };

  double GetMaximum (short label)
  {
    return ((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetMaximum(label));
  };

  double GetMedian (short label)
  {
    return ((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetMedian(label));
  };

  double GetMean (short label)
  {
    return ((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetMean(label));
  };

  double GetSigma (short label)
  {
    return ((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetSigma(label));
  };

  double GetVariance (short label)
  {
    return ((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetVariance(label));
  };

  double GetSum (short label)
  {
    return ((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetSum(label));
  };

  unsigned long GetCount (short label)
  {
    return ((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetCount(label));
  };

  // Description: Override vtkSource's Update so that we can access this class's GetOutput(). vtkSource's GetOutput is not virtual.
  void Update()
  {
    if (this->vtkLabelExporter->GetInput())
      {
        this->itkLabelImporter->Update();
        
        if (this->GetOutput(0))
          {
            this->GetOutput(0)->Update();
            if ( this->GetOutput(0)->GetSource() )
              {
                //          this->SetErrorCode( this->GetOutput(0)->GetSource()->GetErrorCode() );
              }
          }
      }
  }
    
protected:
  //BTX
  typedef itk::LabelStatisticsImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  typedef itk::VTKImageImport<InputImageType> LabelImageImportType;
  
  vtkITKLabelStatisticsImageFilterSS() : Superclass ( ImageFilterType::New() )
  {
    this->vtkLabelExporter = vtkImageExport::New();
    this->itkLabelImporter = LabelImageImportType::New();
    ConnectPipelines(this->vtkLabelExporter, this->itkLabelImporter);
    (dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->SetLabelInput(this->itkLabelImporter->GetOutput());
  };
  ~vtkITKLabelStatisticsImageFilterSS() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  
  LabelImageImportType::Pointer itkLabelImporter;
  //ETX

  vtkImageExport *vtkLabelExporter;
  
private:
  vtkITKLabelStatisticsImageFilterSS(const vtkITKLabelStatisticsImageFilterSS&);  // Not implemented.
  void operator=(const vtkITKLabelStatisticsImageFilterSS&);  //
                                                                          // Not implemented
  
};

vtkCxxRevisionMacro(vtkITKLabelStatisticsImageFilterSS, "$Revision: 1.10 $");
vtkStandardNewMacro(vtkITKLabelStatisticsImageFilterSS);

#endif
