#ifndef __vtkITKBayesianClassifierInitializationImageFilterFF2D_h
#define __vtkITKBayesianClassifierInitializationImageFilterFF2D_h

#include "vtkITKImageToImageFilter2DFF.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKBayesianClassifierInitializationImageFilter :
                                   public vtkITKImageToImageFilter2DFF
{
public:
  static vtkITKBayesianClassifierInitializationImageFilter *New();
  vtkTypeRevisionMacro( vtkITKBayesianClassifierInitializationImageFilter, 
                        vtkITKImageToImageFilter2DFF );

  void SetNumberOfClasses( int n ) 
    { 
    this->GetFilterPointer()->SetNumberOfClasses( n );
    }
  
  int GetNumberOfClasses( int n ) 
    { 
    return this->GetFilterPointer()->GetNumberOfClasses( n );
    }
  
};

#endif
