#include <cassert>
#include <map>

#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "vtkLineBoxLocator.h"
#include "vtkLine.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPlanes.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"

#include "vtkPointData.h"
#include "vtkCellData.h"


//------------------------------------------------------------------------------
vtkLineBoxLocator* vtkLineBoxLocator::New()                       
{ 
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkLineBoxLocator");                    
  if (ret)
  {
    return (vtkLineBoxLocator *)ret;
  }                                                                                           

  // If the factory was unable to create the object, then create it here. 
  return new vtkLineBoxLocator;                                                 
}


//------------------------------------------------------------------------------
void vtkLineBoxLocator::PrintSelf(ostream& os, vtkIndent indent)
{
}


//------------------------------------------------------------------------------
vtkLineBoxLocator::vtkLineBoxLocator()                       
{
  this->Input = NULL;
  this->Output = vtkPolyData::New();
  this->Extents = vtkVoidArray::New();
}


//------------------------------------------------------------------------------
vtkLineBoxLocator::~vtkLineBoxLocator()                       
{
  this->Output->Delete();

  for (int i = 0; i < this->Extents->GetNumberOfPointers(); ++i)
  {
    BoxExtents *ext =
        static_cast<BoxExtents *>(this->Extents->GetVoidPointer(i));
    if (ext)
    {
      delete ext;
    }
  }
  this->Extents->Delete();
}


//------------------------------------------------------------------------------
int vtkLineBoxLocator::AddBox(vtkPlanes *planes, int ExtractInside)
{
  // Creatre data structures.
  BoxExtents *ext = new BoxExtents();
  ext->ExtractInside = ExtractInside;
  ext->Planes = planes;
  this->CalculateExtent(ext);

  // Insert into list. First check if there are empty entries. Otherwise we
  // insert a new one.
//  vtkstd::cout << "num of entries: " << this->Extents->GetNumberOfPointers()
//               << vtkstd::endl;

  int i;
  for (i = 0; i < this->Extents->GetNumberOfPointers(); ++i)
  {
    if (!this->Extents->GetVoidPointer(i))
    {
      this->Extents->SetVoidPointer(i, static_cast<void *>(ext));
      //vtkstd::cout << "add box (found empty)" << i << vtkstd::endl;
      return i;
    }
  }

  //vtkstd::cout << "add box" << i << " " << ext << vtkstd::endl;
  this->Extents->InsertNextVoidPointer(static_cast<void *>(ext));
  return i;
}


//------------------------------------------------------------------------------
bool vtkLineBoxLocator::RemoveBox(int index)
{
//  for (int i = 0; i < this->Extents->GetNumberOfPointers(); ++i)
//  {
//    vtkstd::cout << "box " << i <<  " " << this->Extents->GetVoidPointer(i)
//                 << vtkstd::endl;
//  }
//
//  vtkstd::cout << "remove box" << index << vtkstd::endl;
  if (!this->Extents->GetVoidPointer(index))
  {
    return false;
  }

  delete static_cast<BoxExtents *>(this->Extents->GetVoidPointer(index));
  this->Extents->SetVoidPointer(index, NULL);
  return true;
}

//------------------------------------------------------------------------------
void vtkLineBoxLocator::CalculateExtent(BoxExtents *ext)
{
  vtkPlanes *planes = ext->Planes;
  if (!planes)
  {
    vtkErrorMacro("No planes given.");
    return;
  }
  if (!ext)
  {
    vtkErrorMacro("Extent must not be NULL.");
    return;
  }

  // Create OBB.
  for (int i = 0; i < 6; ++i)
  {
    vtkPlane *plane = planes->GetPlane(i);

    double c[3];
    if (plane)
    {
      plane->GetOrigin(c);
      //double n[3];
      //plane->GetNormal(n);
      //vtkstd::cout << "n: " << n[0] << " " << n[1] << " " << n[2]
      //             << vtkstd::endl;

      if (i % 2)
      {
        ext->Extent[i / 2].max = c[i / 2];
      }
      else
      {
        ext->Extent[i / 2].min = c[i / 2];
      }
    }
    else
    {
      vtkErrorMacro("No plane.");
      return;
    }
  }

  //vtkstd::cout << "x-extent " << ext->Extent[0].max << " " << ext->Extent[0].min
  //             << vtkstd::endl;
  //vtkstd::cout << "y-extent " << ext->Extent[1].max << " " << ext->Extent[1].min
  //             << vtkstd::endl;
  //vtkstd::cout << "z-extent " << ext->Extent[2].max << " " << ext->Extent[2].min
  //             << vtkstd::endl;
}


//------------------------------------------------------------------------------
void vtkLineBoxLocator::Extract()
{
  if (!this->Input)
  {
    vtkErrorMacro("No input available.");
  }

  vtkIdTypeArray* idArray = vtkIdTypeArray::New();

  vtkIdList *ptIds = vtkIdList::New();

  for (vtkIdType i = 0; i < this->Input->GetNumberOfLines(); ++i)
  {
    // Get line.
    this->Input->GetCellPoints(i, ptIds);

    // First go through all the negative boxes, if the point is in one of the
    // negative boxes, we don't keep the line.
    bool abort = false;
    for (vtkIdType j = 0; j < ptIds->GetNumberOfIds(); ++j)
    {
      // Intersect the line with the AABB.
      double point[3];
      this->Input->GetPoint(ptIds->GetId(j), point);

      for (int i = 0; i < this->Extents->GetNumberOfPointers(); ++i)
      {
        BoxExtents *ext =
            static_cast<BoxExtents *>(this->Extents->GetVoidPointer(i));
        if (ext && !ext->ExtractInside)
        {
          if (TestPointInAABB(point, ext))
          {
            abort = true;
            break;
          }
        }
      }
    }

    if (abort)
    {
      continue;
    }
      
    // In a second step we go trough all the positive boxes. If one of the
    // points is insdie one box, we'll keep it.
    bool inBox = false;
    for (vtkIdType j = 0; j < ptIds->GetNumberOfIds(); ++j)
    {
      // Intersect the line with the AABB.
      double point[3];
      this->Input->GetPoint(ptIds->GetId(j), point);

      for (int i = 0; i < this->Extents->GetNumberOfPointers(); ++i)
      {
        BoxExtents *ext =
            static_cast<BoxExtents *>(this->Extents->GetVoidPointer(i));
        if (ext && ext->ExtractInside)
        {
          if (TestPointInAABB(point, ext))
          {
            inBox = true;
            break;
          }
        }
      }
    }

    // Insert the line if it's in box.
    if (inBox)
    {
      idArray->InsertNextValue(i);
    }
  }
  ptIds->Delete();


  vtkIdType numCells =
      idArray->GetNumberOfComponents() * idArray->GetNumberOfTuples();
//  vtkstd::cout << "comp: " << idArray->GetNumberOfComponents() << " tuples: "
//               << idArray->GetNumberOfTuples() << vtkstd::endl;

  this->Output->Allocate(numCells);
  this->Output->SetPoints(this->Input->GetPoints());

  vtkPointData *pd = this->Input->GetPointData();
  vtkCellData *cd = this->Input->GetCellData();

  vtkPointData *outputPD = this->Output->GetPointData();
  vtkCellData *outputCD = this->Output->GetCellData();

  outputPD->PassData(pd);
  outputCD->CopyAllocate(cd);
 
  // Now loop over all cells to see whether they are in the selection.
  // Copy if they are.
  vtkIdList* ids = vtkIdList::New();
 
  vtkIdType numInputCells = this->Input->GetNumberOfCells();
  for (vtkIdType i = 0; i < numCells; i++)
  {
    vtkIdType cellId = idArray->GetValue(i);
    if (cellId >= numInputCells)
    {
      continue;
    }
    this->Input->GetCellPoints(cellId, ids);
    vtkIdType newId =
        this->Output->InsertNextCell(this->Input->GetCellType(cellId), ids);
    outputCD->CopyData(cd, cellId, newId);
  }
  ids->Delete();
  this->Output->Squeeze();

  idArray->Delete();


//  if (!this->Input)
//  {
//    vtkErrorMacro("No input available.");
//  }
//
//  this->Output->Reset();
//
//  int pointCounter = 0;  
//  std::map<vtkIdType, vtkIdType> pointIdMap;
//  vtkIdList *ptIds = vtkIdList::New();
//  vtkPoints *points = vtkPoints::New();
//  vtkCellArray *lines = vtkCellArray::New();
//  std::pair<std::map<vtkIdType, vtkIdType>::iterator, bool> res;
//
//  for (vtkIdType i = 0; i < this->Input->GetNumberOfLines(); ++i)
//  {
//    // Get line.
//    this->Input->GetCellPoints(i, ptIds);
//
//    // First go through all the negative boxes, if the point is in one of the
//    // negative boxes, we don't keep the line.
//    bool abort = false;
//    for (vtkIdType j = 0; j < ptIds->GetNumberOfIds(); ++j)
//    {
//      // Intersect the line with the AABB.
//      double point[3];
//      this->Input->GetPoint(ptIds->GetId(j), point);
//
//      for (int i = 0; i < this->Extents->GetNumberOfPointers(); ++i)
//      {
//        BoxExtents *ext =
//            static_cast<BoxExtents *>(this->Extents->GetVoidPointer(i));
//        if (ext && !ext->ExtractInside)
//        {
//          if (TestPointInAABB(point, ext))
//          {
//            abort = true;
//            break;
//          }
//        }
//      }
//    }
//
//    if (abort)
//    {
//      continue;
//    }
//      
//    // In a second step we go trough all the positive boxes. If one of the
//    // points is insdie one box, we'll keep it.
//    bool inBox = false;
//    for (vtkIdType j = 0; j < ptIds->GetNumberOfIds(); ++j)
//    {
//      // Intersect the line with the AABB.
//      double point[3];
//      this->Input->GetPoint(ptIds->GetId(j), point);
//
//      for (int i = 0; i < this->Extents->GetNumberOfPointers(); ++i)
//      {
//        BoxExtents *ext =
//            static_cast<BoxExtents *>(this->Extents->GetVoidPointer(i));
//        if (ext && ext->ExtractInside)
//        {
//          if (TestPointInAABB(point, ext))
//          {
//            inBox = true;
//            break;
//          }
//        }
//      }
//    }
//
//    // Insert the line if it's in box.
//    if (inBox)
//    {
//      vtkLine *line = vtkLine::New();
//      line->GetPointIds()->SetNumberOfIds(ptIds->GetNumberOfIds());
//
//      for (vtkIdType j = 0; j < ptIds->GetNumberOfIds(); ++j)
//      {
//        int newId;
//        double point[3];
//        this->Input->GetPoint(ptIds->GetId(j), point);
//
//        res = pointIdMap.insert(std::pair<vtkIdType, vtkIdType>(ptIds->GetId(j),
//                                pointCounter));
//        if (!res.second)
//        {
//          newId = res.first->second;
//        }
//        else
//        {
//          points->InsertNextPoint(point[0], point[1], point[2]);
//          newId = pointCounter;
//          ++pointCounter;
//        }
//        line->GetPointIds()->SetId(j, newId);
//      }
//      lines->InsertNextCell(line);
//      line->Delete();
//    }
//  }
//  ptIds->Delete();
//
//  Output->SetPoints(points);
//  Output->SetLines(lines);
//  points->Delete();
//  lines->Delete();
}


//------------------------------------------------------------------------------
void vtkLineBoxLocator::SetInput(vtkPolyData *input)
{
  this->Input = input;
}


//------------------------------------------------------------------------------
vtkPolyData *vtkLineBoxLocator::GetOutput()
{
  return this->Output;
}


//------------------------------------------------------------------------------
int vtkLineBoxLocator::GetNumberOfPositiveBoxes()
{
  int count = 0;
  for (int i = 0; i < this->Extents->GetNumberOfPointers(); ++i)
  {
    BoxExtents *ext =
        static_cast<BoxExtents *>(this->Extents->GetVoidPointer(i));
    if (ext && ext->ExtractInside)
    {
      ++count;
    }
  }

  return count;
}


//------------------------------------------------------------------------------
int vtkLineBoxLocator::GetNumberOfNegativeBoxes()
{
  int count = 0;
  for (int i = 0; i < this->Extents->GetNumberOfPointers(); ++i)
  {
    BoxExtents *ext =
        static_cast<BoxExtents *>(this->Extents->GetVoidPointer(i));
    if (ext && !ext->ExtractInside)
    {
      ++count;
    }
  }

  return count;
}


//------------------------------------------------------------------------------
void vtkLineBoxLocator::Update()
{
  for (int i = 0; i < this->Extents->GetNumberOfPointers(); ++i)
  {
    BoxExtents *ext =
        static_cast<BoxExtents *>(this->Extents->GetVoidPointer(i));
    if (ext)
    {
      this->CalculateExtent(ext);
    }
  }

  this->Extract();
}


//------------------------------------------------------------------------------
vtkLineBoxLocator::vtkLineBoxLocator(const vtkLineBoxLocator&)
{
  assert(false);
}


//------------------------------------------------------------------------------
void vtkLineBoxLocator::operator=(const vtkLineBoxLocator&)
{
  assert(false);
}


//------------------------------------------------------------------------------
bool vtkLineBoxLocator::TestPointInAABB(double p[3], BoxExtents *ext)
{
  if (p[0] > ext->Extent[0].max || p[0] < ext->Extent[0].min ||
      p[1] > ext->Extent[1].max || p[1] < ext->Extent[1].min ||
      p[2] > ext->Extent[2].max || p[2] < ext->Extent[2].min)
  {
    return false;
  }

  return true;
}
