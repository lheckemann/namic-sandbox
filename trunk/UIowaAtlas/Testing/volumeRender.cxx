/*=========================================================================

  Program: 3D ITK Volume Renderer
  Module: volumeRender.cxx

=========================================================================*/

#include "itkImage.h"
#include "vtkImageData.h"
#include "../Source/vtkKWImage.h"
#include "../Source/vtkKWImageIO.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastMapper.h"

int main (int argc, char *argv[])
{

  if (argc < 2)
    {
      std::cerr << "Missing filename command line argument" << std::endl;
      std::cerr << "Usage: " << argv[0] << " inputFilename ";
      return EXIT_FAILURE;
    }

  vtkKWImageIO *reader = vtkKWImageIO::New ();

  reader->SetFileName (argv[1]);

  std::cout << "Reading the ITK image" << std::endl;
  try
    {
    reader->ReadImage ();
    }
  catch (itk::ExceptionObject & excp)
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  vtkKWImage *kwImage = NULL;
  kwImage = reader->HarvestReadImage ();

  reader->Delete ();

  vtkImageData *vtkImage = kwImage->GetVTKImage ();

  kwImage->Delete ();

  vtkRenderer *renderer = vtkRenderer::New ();
  vtkRenderWindow *renWin = vtkRenderWindow::New ();
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New ();

  renWin->AddRenderer (renderer);
  iren->SetRenderWindow (renWin);

  vtkInteractorStyleTrackballCamera *style =
    vtkInteractorStyleTrackballCamera::New ();
  iren->SetInteractorStyle (style);

  renderer->SetBackground (0.4392, 0.5020, 0.5647);

  vtkPiecewiseFunction *opacityTransferFunction =
    vtkPiecewiseFunction::New ();
  opacityTransferFunction->AddPoint (0, 0.2);
  opacityTransferFunction->AddPoint (1, 0.00);

  vtkColorTransferFunction *colorTransferFunction =
    vtkColorTransferFunction::New ();
  colorTransferFunction->AddRGBPoint (0.0, 1.0, 1.0, 1.0);
  colorTransferFunction->AddRGBPoint (1.0, 0.0, 0.0, 0.0);

  vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New ();
  volumeProperty->SetColor (colorTransferFunction);
  volumeProperty->SetScalarOpacity (opacityTransferFunction);
  volumeProperty->ShadeOn ();
  volumeProperty->SetInterpolationTypeToLinear ();

  vtkVolumeRayCastCompositeFunction *compositeFunction =
    vtkVolumeRayCastCompositeFunction::New ();
  vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New ();
  volumeMapper->SetVolumeRayCastFunction (compositeFunction);

  volumeMapper->SetInput (vtkImage);

  vtkVolume *volume = vtkVolume::New ();
  volume->SetMapper (volumeMapper);
  volume->SetProperty (volumeProperty);
  volume->SetScale (1, 1, 1);

  renderer->AddVolume (volume);
  renWin->SetSize (600, 600);
  renWin->SetPosition (100, 100);

  renWin->Render ();
  iren->Start ();

  volume->Delete ();
  volumeMapper->Delete ();
  compositeFunction->Delete ();
  volumeProperty->Delete ();
  colorTransferFunction->Delete ();
  opacityTransferFunction->Delete ();
  style->Delete ();
  renWin->Delete ();
  renderer->Delete ();
  iren->Delete ();
  vtkImage->Delete ();

  return 0;
}
