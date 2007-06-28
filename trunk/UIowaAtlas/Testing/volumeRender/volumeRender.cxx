/*=========================================================================

  Program: 3D ITK Volume Renderer
  Module: volumeRender.cxx

=========================================================================*/

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeTextureMapper2D.h"

#include "vtkXMLImageDataWriter.h"

int main( int argc, char * argv [] )
{

  if( argc < 2 )
    {
      std::cerr << "USAGE: " << argv[0] << " inputFileName" << std::endl;
      exit(-1);
    }

  try
    {
    typedef unsigned char PixelType;
    const unsigned int Dimension = 3;
    
    typedef itk::Image< PixelType, Dimension > ImageType;
    typedef itk::ImageFileReader< ImageType > ReaderType;
    typedef itk::ImageToVTKImageFilter< ImageType > ConnectorFilterType;

    ReaderType::Pointer reader = ReaderType::New();
    ConnectorFilterType::Pointer connector = ConnectorFilterType::New();

    reader->SetFileName( argv[1] );
    reader->Update();

    connector->SetInput( reader->GetOutput() );

    vtkRenderer * renderer = vtkRenderer::New();
    vtkRenderWindow * renWin = vtkRenderWindow::New();
    vtkRenderWindowInteractor * iren = vtkRenderWindowInteractor::New();

    renWin->AddRenderer( renderer );
    iren->SetRenderWindow( renWin );

    vtkInteractorStyleTrackballCamera * style
      = vtkInteractorStyleTrackballCamera::New();
    iren->SetInteractorStyle( style );

    renderer->SetBackground( 0.5, 0.5, 0.5 );

    vtkPiecewiseFunction * opacityTransferFunction
      = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint( 0, 0.00 );
    opacityTransferFunction->AddPoint( 1, 0.20 );

    vtkColorTransferFunction * colorTransferFunction
      = vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
    colorTransferFunction->AddRGBPoint( 1.0, 1.0, 1.0, 1.0 );

    vtkVolumeProperty * volumeProperty
      = vtkVolumeProperty::New();
    volumeProperty->SetColor( colorTransferFunction );
    volumeProperty->SetScalarOpacity( opacityTransferFunction );
    volumeProperty->ShadeOn();
    volumeProperty->SetInterpolationTypeToLinear();

    vtkVolumeTextureMapper2D * volumeMapper
      = vtkVolumeTextureMapper2D::New();
    volumeMapper->SetInput( connector->GetOutput() );

    vtkVolume * volume = vtkVolume::New();
    volume->SetMapper( volumeMapper );
    volume->SetProperty( volumeProperty );

    if( argc > 2 )
      {
      vtkXMLImageDataWriter * writer = vtkXMLImageDataWriter::New();
      writer->SetInput( connector->GetOutput() );
      writer->SetFileName( argv[2] );
      writer->Write();
      writer->Delete();
      }

    renderer->AddVolume( volume );
    renWin->SetSize( 600, 600 );
    renWin->SetPosition( 100, 100 );

    renWin->Render();

    iren->SetDesiredUpdateRate( 20.0 );
    iren->SetStillUpdateRate( 0.001 );
    iren->Start();

    opacityTransferFunction->Delete();
    colorTransferFunction->Delete();
    volumeMapper->Delete();
    volumeProperty->Delete();
    volume->Delete();
    renderer->Delete();
    style->Delete();
    iren->Delete();
    renWin->Delete();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return 0;

}
