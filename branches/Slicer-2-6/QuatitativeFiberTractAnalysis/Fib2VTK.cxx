/*=========================================================================
  Program:   Fib2VTK
  Module:    $RCSfile: Fib2VTK.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/07 18:10:25 $
  Version:   $Revision: 1.1 $

  Copyright (c) General Electric Global Research. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkMetaImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkMetaDataDictionary.h"
#include "metaDTITube.h"

#include "itkImage.h"
#include "vnl/vnl_math.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRandomIteratorWithIndex.h"

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/algo/vnl_svd.h"

#include "itkTubeSpatialObject.h"
#include "itkDTITubeSpatialObject.h"
#include "itkDTITubeSpatialObjectPoint.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkVectorResampleImageFilter.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkMetaDTITubeConverter.h"

#include "vtkPolyLine.h"
#include "vtkPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkIdList.h"
#include "vtkPoints.h"

#include <vector>
#include <fstream>
#include <strstream>
#include <string>

const int MaxLineLen = 256;
const int TENSOR_RECON_FAILURE = -1;
const int TENSOR_RECON_SUCCESS = 1;
const unsigned int nDimension = 3;

typedef itk::Vector<float, nDimension> VectorType;
typedef itk::Image<unsigned char, nDimension> FAImageType;
typedef itk::Image<float, nDimension> FloatImageType;
typedef itk::Image<VectorType, nDimension> VectorImageType;

typedef itk::DTITubeSpatialObject< nDimension > TubeType;
typedef itk::DTITubeSpatialObject< nDimension >::Pointer TubePointerType;
typedef itk::DTITubeSpatialObject< nDimension >::TubePointType TubePointType;
typedef TubeType::PointListType SingleFiberType;
typedef std::vector < TubePointerType > BundleType;

void WriterVTKPolyline(const BundleType & bundle, char* filename)
{
  vtkPolyDataWriter *fiberwriter = vtkPolyDataWriter::New();
  vtkPolyData* polydata = vtkPolyData::New();
  vtkIdList *ids = vtkIdList::New();
  vtkPoints *pts = vtkPoints::New();

  polydata ->SetPoints (pts);


  ids -> SetNumberOfIds(0);
  pts -> SetNumberOfPoints(0);

  unsigned int nPoints = 0;
  polydata->Allocate();

  for ( int n = 0; n < bundle.size(); n++)
    {
    unsigned int nPointsOnFiber = bundle[n]->GetNumberOfPoints();
    vtkIdType currentId = ids->GetNumberOfIds();

    for (int k = 0; k < nPointsOnFiber; k++)
      {
      FloatImageType::PointType v = bundle[n]->GetPoint(k)->GetPosition();
      vtkIdType id;
      id = pts->InsertNextPoint(v[0], v[1], v[2]);
      ids->InsertNextId(id);
      }
    polydata->InsertNextCell(VTK_POLY_LINE, nPointsOnFiber, ids->GetPointer(currentId));
    }

  fiberwriter->SetFileTypeToASCII();
  fiberwriter->SetFileName(filename);
  fiberwriter->SetInput(polydata);
    
  fiberwriter->Update();

}


int main(int ac, char* av[])
{
    
  if(ac < 3)
    {
    std::cerr << "Usage: " << av[0] << "fibers.fib(UNC format) fibers.vtk \n";
    std::cerr << "Version:   $Revision: 1.1 $" << std::endl;
    return EXIT_FAILURE;
    }
  
  std::fstream ifile;

  ifile.open(av[1], std::ios::in);

  BundleType AllFibers;
  char line[1024];

  while (!ifile.eof())
    {
    std::string aLine;
    ifile.getline(line, 1024);
    aLine = line;

    SingleFiberType p;

    if (aLine.find("NPoints") == std::string::npos)
      {
      continue;
      }
    else
      {
      std::size_t posEqual = aLine.find_last_of("=");
      aLine = aLine.substr(posEqual+1, aLine.length()-posEqual);
      int nPoints = atoi(aLine.c_str());
      VectorType pt;

      ifile.getline(line, 1024);
      aLine = line;
      if (aLine.find("Points") == std::string::npos)
        {
        ifile.getline(line, 1024);      
        }

      for (int k = 0; k< nPoints; k++)
        {
        ifile.getline(line, 1024);      
        std::strstream aStringStream(line, 1024, std::ios_base::in);
        aStringStream >> pt[0] >> pt[1] >> pt[2];

        TubePointType tempTubePoint;
        tempTubePoint.SetPosition(pt[0], pt[1], pt[2]);
        p.push_back(tempTubePoint);
        }
      }

    TubePointerType Fiber = TubeType::New();
    Fiber->SetPoints(p);
    Fiber->ComputeTangentAndNormals();  
    Fiber->ComputeBoundingBox();
    AllFibers.push_back(Fiber);
    }

  WriterVTKPolyline(AllFibers, av[2]);
  return EXIT_SUCCESS;  
}

