/*=========================================================================
  Program:   Slicer2Fib
  Module:    $RCSfile: Slicer2Fib.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/07 18:10:25 $
  Version:   $Revision: 1.1 $
  Authors:   Xiaodong Tao
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

#include "itkNumericTraits.h"
#include "itkSpatialObject.h"
#include "itkDTITubeSpatialObject.h"
#include "itkDTITubeSpatialObjectPoint.h"
#include "itkSpatialObjectWriter.h"
#include "itkGroupSpatialObject.h"

typedef itk::DTITubeSpatialObjectPoint<3> DTIPointType;
typedef std::vector<DTIPointType> DTIPointListType;


void CascadeFibers(DTIPointListType & forward, DTIPointListType & backward)
{
  const int nForward = forward.size();
  const int nBackward = backward.size();

  for (int k = 0; k < nForward/2; k++)
    {
    DTIPointType d = forward[k];
    forward[k] = forward[nForward-k-1];
    forward[nForward-k-1] = d;
    }

  for (int k = 0; k < nBackward/2; k++)
    {
    DTIPointType d = backward[k];
    backward[k] = backward[nBackward-k-1];
    backward[nBackward-k-1] = d;
    }

  for (int k = 1; k < nBackward; k++)
    {
    forward.push_back(backward[k]);
    }

  return;
}

int main(int argc,char* argv[])
{
  if(argc != 3)
    {
    std::cerr << "Usage: Slicer2Fib <infile> <outfile>" << std::endl;
    return 0;
    }

  vtkPolyDataReader* reader = vtkPolyDataReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkPolyData* data = reader->GetOutput();

  std::cout << "Lines: " << data->GetNumberOfLines() << std::endl;
  std::cout << "Cells: " << data->GetNumberOfCells() << std::endl; 
  
  const int nCell = data->GetNumberOfCells();
  const int nfib = nCell/12;

  itk::GroupSpatialObject<3>::Pointer fiberGroup = itk::GroupSpatialObject<3>::New();

  itk::SpatialObjectWriter<3>::Pointer fibWriter = itk::SpatialObjectWriter<3>::New();

  // Each fiber is stored as a tube with six samples around its axis. The following code
  // extract the axis by averaging the coordinates of the samples. 
  for(int i = 0; i < nfib; ++i)
    {
    DTIPointListType pointsToAdd;

    itk::DTITubeSpatialObject<3>::Pointer dtiTube = itk::DTITubeSpatialObject<3>::New();
    std::vector<vtkPoints*> pointsOnCylinder(6);
    for (int j = 0; j < 6; j++)
      {
      pointsOnCylinder[j] = data->GetCell(i*12+j)->GetPoints();
      }

    int nPoints = pointsOnCylinder[0]->GetNumberOfPoints()/2;
    for(int j = 0; j < nPoints; ++j)
      {
      double* coordinates = pointsOnCylinder[0]->GetPoint(j*2);
      for (int k = 1; k < 6; k++)
        {
        double* coordinatesNext = pointsOnCylinder[k]->GetPoint(j*2);
        for (int m = 0; m < 3; m++)
          {
          coordinates[m] += coordinatesNext[m];
          }
        }
      for (int m = 0; m < 3; m++)         
        {
        coordinates[m] /= 6;        
        }

      DTIPointType pt;
      pt.SetPosition(coordinates[0],coordinates[1],coordinates[2]);
      pt.AddField("r",0.5);

      pointsToAdd.push_back(pt);
      }

    // Find backward piece
    DTIPointListType pointsToAddBK;
    std::vector<vtkPoints*> pointsOnBKCylinder(6);

    for (int j = 0; j < 6; j++)
      {
      pointsOnBKCylinder[j] = data->GetCell(i*12+j+6)->GetPoints();
      }
    int nPointsBK = pointsOnBKCylinder[0]->GetNumberOfPoints()/2;
    for(int j = 0; j < nPointsBK; ++j)
      {
      double* coordinates = pointsOnBKCylinder[0]->GetPoint(j*2);
      for (int k = 1; k < 6; k++)
        {
        double* coordinatesNext = pointsOnBKCylinder[k]->GetPoint(j*2);
        for (int m = 0; m < 3; m++)
          {
          coordinates[m] += coordinatesNext[m];
          }
        }
      for (int m = 0; m < 3; m++)         
        {
        coordinates[m] /= 6;        
        }

      DTIPointType pt;
      pt.SetPosition(coordinates[0],coordinates[1],coordinates[2]);
      pt.AddField("r",0.5);

      pointsToAddBK.push_back(pt);
      }

    CascadeFibers(pointsToAdd, pointsToAddBK);

    dtiTube->SetPoints(pointsToAdd);
    fiberGroup->AddSpatialObject(dtiTube);
    }

  fibWriter->SetInput(fiberGroup);
  fibWriter->SetFileName(argv[2]);
  fibWriter->Update();
  reader->Delete();

  return 0;
}
