/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile:$
  Date:      $Date:$
  Version:   $Revision:$

=========================================================================auto=*/

#include "vtkObjectFactory.h"
#include "vtkIntensityCurves.h"

#include "vtkDoubleArray.h"
#include "vtkMRML4DBundleNode.h"
#include "vtkMRMLScalarVolumeNode.h"

vtkStandardNewMacro(vtkIntensityCurves);
vtkCxxRevisionMacro(vtkIntensityCurves, "$Revision: $");

//---------------------------------------------------------------------------
vtkIntensityCurves::vtkIntensityCurves()
{
  this->BundleNode = NULL;
  this->MaskNode   = NULL;

  this->IntensityCurveMean.clear();
  this->IntensityCurveSD.clear();
}


//---------------------------------------------------------------------------
vtkIntensityCurves::~vtkIntensityCurves()
{
}


//---------------------------------------------------------------------------
void vtkIntensityCurves::PrintSelf(ostream& os, vtkIndent indent)
{
}


//---------------------------------------------------------------------------
int vtkIntensityCurves::Update()
{

  if (!this->BundleNode || !this->MaskNode)
    {
    std::cerr << "bundle or mask node has not been set." << std::endl;
    return 0;
    }

  std::cerr << "intensity update: " << this->GetMTime() << std::endl;
  std::cerr << "Bundle update: " << this->BundleNode->GetMTime() << std::endl;
  std::cerr << "Mask update: " << this->MaskNode->GetMTime() << std::endl;

  if (this->GetMTime() > this->BundleNode->GetMTime() &&
      this->GetMTime() > this->MaskNode->GetMTime())
    {
    // if the intensity curve is newer than the 4D bundle and the mask
    // do nothing
    std::cerr << "intensity curve is up to date." << std::endl;
    return 1;
    }
  
  // Generate intensity curves
  GenerateIntensityCurve();
  this->Modified();
}


//---------------------------------------------------------------------------
vtkIntArray* vtkIntensityCurves::GetLabelList()
{
  vtkIntArray* array = vtkIntArray::New();

  IntensityCurveMapType::iterator iter;
  for (iter = this->IntensityCurveMean.begin(); iter != this->IntensityCurveMean.end(); iter ++)
    {
    int label = iter->first;
    array->InsertNextValue(label);
    }

  return array;
}


//---------------------------------------------------------------------------
vtkDoubleArray* vtkIntensityCurves::GetCurve(int label)
{
  return this->IntensityCurveMean[label];
}


//---------------------------------------------------------------------------
void vtkIntensityCurves::GenerateIntensityCurve()
{

  this->IntensityCurveMean.clear();
  this->IntensityCurveSD.clear();

  if (this->BundleNode && this->MaskNode)
    {
    vtkImageData*  mask = this->MaskNode->GetImageData();

    IndexTableMapType  indexTableMap;
    GenerateIndexMap(mask, indexTableMap);

    IndexTableMapType::iterator iter;
    for (iter = indexTableMap.begin(); iter != indexTableMap.end(); iter ++)
      {
      int label = iter->first;
      this->IntensityCurveMean[label] = vtkDoubleArray::New();
      this->IntensityCurveSD[label]   = vtkDoubleArray::New();
      this->IntensityCurveMean[label]->SetNumberOfComponents( static_cast<vtkIdType>(2) );
      this->IntensityCurveSD[label]->SetNumberOfComponents( static_cast<vtkIdType>(2) );

      std::cerr << "creating array for label = " << label << std::endl;
      }
    
    int nFrames = this->BundleNode->GetNumberOfFrames();
    for (int i = 0; i < nFrames; i ++)
      {
      std::cerr << "processing frame = " << i << std::endl;
      vtkMRMLScalarVolumeNode* inode
        = vtkMRMLScalarVolumeNode::SafeDownCast(this->BundleNode->GetFrameNode(i));
      if (inode)
        {
        IndexTableMapType::iterator iter2;
        for (iter2 = indexTableMap.begin(); iter2 != indexTableMap.end(); iter2 ++)
          {
          int label = iter2->first;
          std::cerr << "    processing label = " << label << std::endl;
          IndexTableType& indexTable = iter2->second;
          double meanvalue = GetMeanIntensity(inode->GetImageData(), indexTable);
          double sdvalue   = GetSDIntensity(inode->GetImageData(), meanvalue, indexTable);
          //std::cerr << "mean = " << meanvalue << ", sd = " << sdvalue << std::endl;
          double xy[2];
          xy[0] = (double)i;
          xy[1] = meanvalue;
          this->IntensityCurveMean[label]->InsertNextTuple(xy);
          xy[0] = (double)i;
          xy[1] = sdvalue;
          this->IntensityCurveSD[label]->InsertNextTuple(xy);
          }
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkIntensityCurves::GenerateIndexMap(vtkImageData* mask, IndexTableMapType& indexTableMap)
{
  int maskDim[3];

  mask->GetDimensions(maskDim);
  int x = maskDim[0];
  int y = maskDim[1];
  int z = maskDim[2];

  indexTableMap.clear();
  for (int i = 0; i < x; i ++)
    {
    for (int j = 0; j < y; j ++)
      {
      for (int k = 0; k < z; k ++)
        {
        int label = mask->GetScalarComponentAsDouble(i, j, k, 0);
        if (label != 0)
          {
          CoordType index;
          index.x = i;
          index.y = j;
          index.z = k;
          indexTableMap[label].push_back(index);
          }
        }
      }
    }
}


//---------------------------------------------------------------------------
double vtkIntensityCurves::GetMeanIntensity(vtkImageData* image, IndexTableType& indexTable)
{
  double sum = 0.0;
  
  IndexTableType::iterator iter;
  for (iter = indexTable.begin(); iter != indexTable.end(); iter ++)
    {
    sum += image->GetScalarComponentAsDouble(iter->x, iter->y, iter->z, 0);
    }

  double mean = sum / (double)indexTable.size();

  return mean;
}


double vtkIntensityCurves::GetSDIntensity(vtkImageData* image, double mean, IndexTableType& indexTable)
{
  double s = 0.0;
  double n = (double) indexTable.size();

  IndexTableType::iterator iter;
  for (iter = indexTable.begin(); iter != indexTable.end(); iter ++)
    {
    double i = image->GetScalarComponentAsDouble(iter->x, iter->y, iter->z, 0);
    s += (i - mean)*(i - mean);
    }
  double sigma = sqrt(s/n);
  return sigma;
}



