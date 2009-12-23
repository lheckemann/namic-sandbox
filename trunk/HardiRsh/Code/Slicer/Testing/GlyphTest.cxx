
#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkDebugLeaks.h"
#include "vtkDiffusionRSHGlyph.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"

#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkLookupTable.h"
#include <vtkMatrix4x4.h> 

class vtkMatrix4x4;

int main( int argc, char *argv[] )
{
  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

  vtkSphereSource *sphere = vtkSphereSource::New();
    sphere->SetThetaResolution(9); sphere->SetPhiResolution(9);

  vtkImageData *id = vtkImageData::New();

    double c1[15] = {0.282095, -0.0000537938, 0.000229927, 0.117261, 0.000310647, \
-0.000781397, 0.000818194, 0.000125872, -0.0000930748, -0.000393471, \
0.0455436, -0.000601329, -0.000125309, 0.000179752, 0.000565448};

  double c2[15] = {0.282095, -0.0505327, -0.000249706, 0.0296428,
 0, 0.000165669, 0.0173124, 0.0000228754, 0.0126528, \
-0.000183299, 0.0313063, -0.000347831, 0.00002271, 0.0000680947, \
0.000285628};

  double c3[15] = {0.282095, 0, -0.000273104, 0.0000949636, 0.000111416, \
-0.00008397, 0.0228601,0, -0.0000253062, -0.0000909136, 0.0267146, \
-0.000311602, 0.0000104515, 0.000179483, -0.0000276736};
  
  id->SetDimensions( 3, 1, 1 );
  id->SetSpacing( 5.0, 5.0, 5.0 );
  id->SetNumberOfScalarComponents( 15 );

  id->SetScalarTypeToDouble();
  id->AllocateScalars();
  
  id->GetPointData()->GetScalars()->SetTuple(0,c1);
  id->GetPointData()->GetScalars()->SetTuple(1,c2);
  id->GetPointData()->GetScalars()->SetTuple(2,c3);

  vtkMatrix4x4 *gRotMat = vtkMatrix4x4::New();
  gRotMat->Identity();
  gRotMat->Zero();
  gRotMat->SetElement(0,0,1);
  gRotMat->SetElement(1,2,1);
  gRotMat->SetElement(2,1,1);
  gRotMat->SetElement(3,3,1);
  
  vtkDiffusionRSHGlyph *glyph = vtkDiffusionRSHGlyph::New();
    glyph->SetSourceConnection(sphere->GetOutputPort());
    glyph->SetInput(id);
    glyph->SetScaleFactor(5);
    glyph->SetDimensions(3,1);
    glyph->SetGlyphRotationMatrix(gRotMat);
//    glyph->MinMaxNormalizationOn();
    

    
  vtkPolyDataMapper *spikeMapper = vtkPolyDataMapper::New();
    spikeMapper->SetInputConnection(glyph->GetOutputPort());



  // create a hue lookup table
  vtkLookupTable *lut = vtkLookupTable::New();
  lut->SetHueRange(0.0, 1.0);
  lut->SetSaturationRange(1.0,1.0);
  lut->SetValueRange(0.5, 1.0);
  lut->SetAlphaRange(1.0, 1.0);
  lut->SetNumberOfColors(256);
  lut->SetRampToLinear();
  lut->Build();
  spikeMapper->SetLookupTable (lut);

  vtkActor *spikeActor = vtkActor::New();
    spikeActor->SetMapper(spikeMapper);

  renderer->AddActor(spikeActor);
  renderer->SetBackground(1,1,1);
  renWin->SetSize(300,300);


  // Set up the lighting.
  //
  vtkLight *light = vtkLight::New();
  light->SetFocalPoint(1.875,0.6125,0);
  light->SetPosition(0.875,1.6125,1);

  // We want to eliminate perspective effects on the apparent lighting.
  // Parallel camera projection will be used. To zoom in parallel projection
  // mode, the ParallelScale is set.
  //
  renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
  renderer->GetActiveCamera()->SetPosition(-0.5,0,0.0);
  renderer->GetActiveCamera()->SetViewUp(0,0,1);
  renderer->ResetCamera();

  // interact with data
  renWin->Render();

  int retVal = vtkRegressionTestImage( renWin );

  iren->Start();

  std::cerr << "TRY 2" << std::endl;
  retVal = vtkRegressionTestImage( renWin );

  sphere->SetThetaResolution(15); sphere->SetPhiResolution(15);

  glyph->MinMaxNormalizationOn();

  iren->Start();
  

  // Clean up
  renderer->Delete();
  renWin->Delete();
  iren->Delete();
  sphere->Delete();
  id->Delete();
  glyph->Delete();
  spikeMapper->Delete();
  spikeActor->Delete();
  light->Delete();
  lut->Delete();
  gRotMat->Delete();
  return !retVal;
}
