#ifndef __vtkKWMeshVisu_h
#define __vtkKWMeshVisu_h

#include "vtkKWObject.h"

class vtkKWRenderWidget;
class vtkImageViewer2;
class vtkKWScale;
class vtkKWWindowLevelPresetSelector;
class vtkKWColorTransferFunctionEditor;
class vtkColorTransferFunction;
class vtkPolyData ;
class vtkPolyDataMapper;
class vtkActor;
class vtkPolyDataNormals;
class vtkGlyph3D ;
class vtkArrowSource ;

class vtkKWMeshVisu : public vtkKWObject
{
public:
  static vtkKWMeshVisu* New();
  
  vtkTypeRevisionMacro(vtkKWMeshVisu,vtkKWObject);

  // Description:
  // Run the example.
  int Run(int argc, char *argv[]);

  // Description:
  // Callbacks
  virtual void LoadMeshCallback();
  virtual void Load1DAttributeCallback();
  virtual void Load3DAttributeCallback();
  virtual void LoadAttributeMeshCallback();
  virtual void SaveAttributeMeshCallback();
  virtual void PValueCallback();
  virtual void DistanceMapCallback();

  
protected:
  vtkKWMeshVisu();
  ~vtkKWMeshVisu();

  vtkKWRenderWidget *RenderWidget;
  vtkKWColorTransferFunctionEditor *TFuncEditor;
  vtkColorTransferFunction *PValueTFunc;
  vtkColorTransferFunction *DistanceMapTFunc;

  void UpdateDistanceMapTFunc ();

  vtkPolyData *vtkMesh ;

  vtkPolyDataMapper *meshMapper ;
  vtkActor *meshActor ;

  vtkPolyDataNormals *normalFilter ;

  vtkArrowSource *arrowSource ;
  vtkGlyph3D *glyphFilter;
  vtkPolyDataMapper *glyphMapper ;
  vtkActor *glyphActor ;

private:
  vtkKWMeshVisu(const vtkKWMeshVisu&);   // Not implemented.
  void operator=(const vtkKWMeshVisu&);  // Not implemented.
};

#endif
