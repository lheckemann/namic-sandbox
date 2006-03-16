#include "vtkmrmlVolume.h"
#include "vtkImageData.h"

namespace mrml
{
namespace vtk
{
vtkVolume::vtkVolume()
{
}

vtkVolume::~vtkVolume()
{
}

void vtkVolume::SetSourceNode(VolumeNode* node)
{
  (void)node;
}

vtkImageData* vtkVolume::GetImageData()
{
  return NULL;
}

void vtkVolume::SetTargetNode(VolumeNode* node)
{
  (void)node;
}

void vtkVolume::SetSourceImage(vtkImageData* img)
{
  (void)img;
}

} // end namespace vtk
} // end namespace mrml
