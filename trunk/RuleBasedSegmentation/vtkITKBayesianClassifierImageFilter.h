// .NAME
// .SECTION
//
//

#ifndef __vtkITKBayesianClassifierImageFilter_h
#define __vtkITKBayesianClassifierImageFilter_h

// This filter takes an image a Float image as input and produces a label map
// image of unsigned chars
// The filter provides the following interfaces
// 1. Number of smoothing iterations
// 2. ( Internally an Anisotropic diffusion smoothing filter is used. A couple 
//   of options could be provided to the user via enums to switch between
//   Anisotropic diffusion and Curvature flow, but I'll let you guys decide that)
// 3. Default density functions are provided that are gaussian density functionsn 
//    centered around the Kmeans of the input. In future this could also be 
//    changed to be toggled between different types of density functions via 
//    enums..


#include "itkBayesianClassificationImageFilter.h"

class VTK_EXPORT vtkITKBayesianClassifierImageFilter 
                              : public vtkITKImageToImageFilterFUC
{
public: 
  vtkTypeMacro( vtkITKBayesianClassifierImageFilter, vtkITKImageToImageFilterFUC );
  static vtkITKBayesianClassifierImageFilter * New();

  void PrintSelf( ostream &os, vtkIndent indent )
    {
    Superclass::PrintSelf( os, indent );
    }

  void SetNumberOfSmoothingIterations( int n )
    {
    DelegateITKInputMacro( SetNumberOfSmoothingIterations, static_cast< unsigned int >( n ) );
    }

  int GetNumberOfSmoothingIterations()
    {
    DelegateITKOutputMacro( GetNumberOfSmoothingIterations);
    }

  void SetNumberOfClasses( int n )
    {
    DelegateITKInputMacro( SetNumberOfClasses, static_cast< unsigned int >( n ) );
    }

  int GetNumberOfClasses()
    {
    DelegateITKOutputMacro( GetNumberOfClasses);
    }
  
protected:
  //BTX
  typedef itk::BayesianClassifierImageFilter<
     Superclass::InputImageType, 
     Superclass::OutputImageType>                  ImageFilterType;
  vtkITKBayesianClassifierImageFilter() 
                    : Superclass ( ImageFilterType::New() ){};
  ~vtkITKBayesianClassificationImageFilter() {};
  ImageFilterType* GetImageFilterPointer() 
    { 
    return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); 
    }
  //ETX
  
private:
  vtkITKBayesianClassifierImageFilter(const vtkITKBayesianClassifierImageFilter&);  // Not implemented.
  void operator=(const vtkITKBayesianClassifierImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKBayesianClassifierImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKBayesianClassifierImageFilter);

#endif

