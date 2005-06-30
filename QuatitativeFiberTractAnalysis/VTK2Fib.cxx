/*=========================================================================
  Program:   VTK2Fib
  Module:    $RCSfile: Fib2VTK.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/07 18:10:25 $
  Version:   $Revision: 1.1 $
  Authors:   Xiadong Tao
             Casey Goodlett

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notices for more information.

  ========================================================================== */
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <vector>
#include <list>

#include "vtkPolyLine.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPolyDataReader.h"
#include "vtkIdList.h"
#include "vtkPoints.h"

#include "itkSpatialObject.h"
#include "itkDTITubeSpatialObject.h"
#include "itkDTITubeSpatialObjectPoint.h"
#include "itkSpatialObjectWriter.h"
#include "itkGroupSpatialObject.h"

int main(int argc,char* argv[])
{
  if(argc != 3)
    {
    std::cerr << "Usage: VTK2Fib <infile> <outfile>" << std::endl;
    return 0;
    }

  vtkPolyDataReader* reader = vtkPolyDataReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkPolyData* data = reader->GetOutput();

  std::cout << "Lines: " << data->GetNumberOfLines() << std::endl;
  std::cout << "Cells: " << data->GetNumberOfCells() << std::endl; 
  
  const int nfib = data->GetNumberOfCells();

  itk::GroupSpatialObject<3>::Pointer fiberGroup = itk::GroupSpatialObject<3>::New();

  itk::SpatialObjectWriter<3>::Pointer fibWriter = itk::SpatialObjectWriter<3>::New();

  for(int i = 0; i < nfib; ++i)
    {
    itk::DTITubeSpatialObject<3>::Pointer dtiTube = itk::DTITubeSpatialObject<3>::New();
    vtkPoints* points = data->GetCell(i)->GetPoints();

    typedef itk::DTITubeSpatialObjectPoint<3> DTIPointType;
    std::vector<DTIPointType> pointsToAdd;

    for(int j = 0; j < points->GetNumberOfPoints(); ++j)
      {
      double* coordinates = points->GetPoint(j);
      DTIPointType pt;
      pt.SetPosition(coordinates[0],coordinates[1],coordinates[2]);
      pt.AddField("r",0.5);
      // Need to set tensor matrix with data from vtk

      pointsToAdd.push_back(pt);
      }
    dtiTube->SetPoints(pointsToAdd);
    fiberGroup->AddSpatialObject(dtiTube);
    }

  fibWriter->SetInput(fiberGroup);
  fibWriter->SetFileName(argv[2]);
  fibWriter->Update();
  reader->Delete();

  return 0;
}
