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
#include <string>

#include "vtkPolyLine.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPolyDataReader.h"
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredPoints.h"
#include "vtkIdList.h"
#include "vtkPointData.h"
#include "vtkPoints.h"

#include "itkSpatialObject.h"
#include "itkDTITubeSpatialObject.h"
#include "itkDTITubeSpatialObjectPoint.h"
#include "itkSpatialObjectWriter.h"
#include "itkGroupSpatialObject.h"

#include "TensorStatistics.h"
#include "TensorGeometry.h"
#include "SymmetricSpaceTensorGeometry.h"

int main(int argc,char* argv[])
{
  typedef  itk::SymmetricSecondRankTensor<double,3> ITKTensorType;
  typedef  itk::VectorContainer<unsigned int, ITKTensorType>      ITKTensorListType;

  if(argc < 3 || argc > 5)
    {
    std::cerr << "Usage: VTK2Fib <infile> <outfile> [tensor field] [b-value]" << std::endl;
    return 0;
    }

  bool addtensors = false;

  double bvalue = 1.0;

  if(argc >= 4)
    {
    addtensors = true;
    }

  if(argc == 5)
    {
    bvalue = atof(argv[4]);
    }
  
  vtkPolyDataReader* reader = vtkPolyDataReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkPolyData* fibdata = reader->GetOutput();

  std::cout << "Lines: " << fibdata->GetNumberOfLines() << std::endl;
  std::cout << "Cells: " << fibdata->GetNumberOfCells() << std::endl; 
  assert ( fibdata->GetNumberOfLines() = fibdata->GetNumberOfCells());

  const int nfib = fibdata->GetNumberOfCells();

  // If we are adding the tensors open the tensor file
  vtkStructuredPointsReader* tensorreader = NULL;
  vtkStructuredPoints* tensorfield = NULL;
  vtkDataArray* tensordata = NULL;

  // Initialize tensor statistics
  TensorGeometry<double>* geometry= new SymmetricSpaceTensorGeometry<double>();
  TensorStatistics<double> tensorstats(geometry);

  if(addtensors)
    {
    tensorreader = vtkStructuredPointsReader::New();
    tensorreader->SetTensorsName("tensors");
    tensorreader->SetFileName(argv[3]);
    tensorreader->Update();
    tensorfield = tensorreader->GetOutput();
    tensordata = tensorfield->GetPointData()->GetTensors();


//     std::cout << "Num scalars in file: " << tensorreader->NumberOfScalarsInFile() << std::endl;
//     std::cout << "Num vectors in file: " << tensorreader->NumberOfVectorsInFile() << std::endl;
//     std::cout << "Num tensors in file: " << tensorreader->NumberOfTensorsInFile() << std::endl;

    std::cout << "TComp: " << tensorfield->GetPointData()->GetTensors()->GetNumberOfComponents() << std::endl;
    std::cout << "TPoints: " << tensorfield->GetPointData()->GetTensors()->GetNumberOfTuples() << std::endl;

    std::cout << "Num Scalar Components: " << tensorfield->GetNumberOfScalarComponents() << std::endl;

    std::cout << "Num Points in tensor field: " << tensorfield->GetNumberOfPoints() << std::endl;
    std::cout << "Num Cells in tensor field: " << tensorfield->GetNumberOfCells() << std::endl;
    }

  itk::GroupSpatialObject<3>::Pointer fiberGroup = itk::GroupSpatialObject<3>::New();

  itk::SpatialObjectWriter<3>::Pointer fibWriter = itk::SpatialObjectWriter<3>::New();

  for(int i = 0; i < nfib; ++i)
    {
    itk::DTITubeSpatialObject<3>::Pointer dtiTube = itk::DTITubeSpatialObject<3>::New();
    vtkPoints* points = fibdata->GetCell(i)->GetPoints();

    typedef itk::DTITubeSpatialObjectPoint<3> DTIPointType;
    std::vector<DTIPointType> pointsToAdd;

    for(int j = 0; j < points->GetNumberOfPoints(); ++j)
      {
      double* coordinates = points->GetPoint(j);
      DTIPointType pt;
      pt.SetPosition(coordinates[0],coordinates[1],coordinates[2]);
      pt.AddField("r",0.5);

      // Need to set tensor matrix with data from vtk
      int subId;
      double pcoord[3];
      double weights[8];
//      vtkIdType id = tensorfield->FindPoint(coordinates);
      vtkCell* voxel = tensorfield->FindAndGetCell(coordinates,NULL,-1,1e-6,subId,pcoord,weights);

//      voxel->GetPoints()->SetDataTypeToDouble();
      // we need to interpolate between the tensors
      // pcoords range from 0 <-> 1
      // weights sum to 1
      // Cells are of type vtkVoxel
      
      // For some reason getting the points from the cell and getting
      // the data from those points does not work, so get the points
      // ids, and look up the data in the tensorfield
      
      vtkIdList* idlist = voxel->GetPointIds();
      // eight points with 6 parameters

      assert(idlist->GetNumberOfIds() == 8);
      itk::VectorContainer<unsigned int, double>::Pointer itkweights = itk::VectorContainer<unsigned int, double>::New();

      ITKTensorListType::Pointer voxeltensors = ITKTensorListType::New();
      for(int i = 0; i < idlist->GetNumberOfIds(); ++i)
        {
        double* vtktensor = tensordata->GetTuple9(idlist->GetId(i));
        ITKTensorType itktensor;

        // Map data to six basic elements
        itktensor[0] = vtktensor[0] / bvalue;
        itktensor[1] = vtktensor[1] / bvalue;
        itktensor[2] = vtktensor[2] / bvalue;
        itktensor[3] = vtktensor[4] / bvalue;
        itktensor[4] = vtktensor[5] / bvalue;
        itktensor[5] = vtktensor[8] / bvalue;

        voxeltensors->InsertElement(i,itktensor);
        itkweights->InsertElement(i,weights[i]);
        }
      
//      std::copy(weights,weights+8,itkweights->CastToSTLContainer().begin());

      // interpolate over teh celltensordata points with weights
      ITKTensorType interpolatedtensor;
      tensorstats.ComputeWeightedAve(itkweights,voxeltensors,interpolatedtensor);

      // convert double to float
      double* itensordata = interpolatedtensor.GetDataPointer();
      float SOtensor[6];
      for(int i = 0; i < 6; ++i)
        SOtensor[i] = itensordata[i];

      pt.SetTensorMatrix(SOtensor);
      pointsToAdd.push_back(pt);
      }
    dtiTube->SetPoints(pointsToAdd);
    fiberGroup->AddSpatialObject(dtiTube);
    }

  fibWriter->SetInput(fiberGroup);
  fibWriter->SetFileName(argv[2]);
  fibWriter->Update();
  reader->Delete();
  if(tensorreader != NULL)
    tensorreader->Delete();
  delete geometry;

  return 0;
}
