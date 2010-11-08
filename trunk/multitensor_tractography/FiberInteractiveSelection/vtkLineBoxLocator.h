#ifndef __vtkLineBoxLocator_h
#define __vtkLineBoxLocator_h

#include "vtkObject.h"
#include "vtkVoidArray.h"


class vtkPlanes;
class vtkPolyData;
class vtkTransform;

struct BoxExtents
{
  struct Extents
  {
    double max;
    double min;
  };
  Extents Extent[3];
  int ExtractInside;

  vtkPlanes *Planes;
};

class VTK_FILTERING_EXPORT vtkLineBoxLocator : public vtkObject
{
public:
  vtkTypeMacro(vtkLineBoxLocator, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkLineBoxLocator *New();

  int AddBox(vtkPlanes *planes, int ExtractInside);
  bool RemoveBox(int index);

  void SetInput(vtkPolyData *input);
  vtkPolyData *GetOutput();

  int GetNumberOfPositiveBoxes();
  int GetNumberOfNegativeBoxes();

  void Update();

protected:
  vtkLineBoxLocator();                                                         
  virtual ~vtkLineBoxLocator();                                                
  vtkLineBoxLocator(const vtkLineBoxLocator&);
  void operator=(const vtkLineBoxLocator&);

  bool TestPointInAABB(double p[3], BoxExtents *ext);

  vtkVoidArray *Extents;

  void CalculateExtent(BoxExtents *ext);
  void Extract();

  vtkPolyData *Input;
  vtkPolyData *Output;
};

#endif
