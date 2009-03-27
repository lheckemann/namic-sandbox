/*=========================================================================
  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: OtsuThresholdImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2007-07-13 10:59:25 -0400 (Fri, 13 Jul 2007) $
  Version:   $Revision: 3833 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkLineIterator.h"
#include "itkExtractImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkRigid3DTransform.h"
#include "itkResampleImageFilter.h"
#include "itkSimpleContourExtractorImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkAndImageFilter.h"

#include "itkPluginFilterWatcher.h"
#include "itkPluginUtilities.h"

#include "LabelDiameterEstimationCLP.h"

#include <fstream>
#include <vector>

// Image Types
typedef  unsigned char  PixelType;

typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::Image< PixelType, 2 > ImageType2D;
typedef itk::ImageRegionConstIteratorWithIndex<ImageType> ConstIterWithIndex;
typedef itk::ImageRegionConstIterator<ImageType> ConstIter;
typedef itk::NeighborhoodIterator<ImageType2D> NeiIterType;
typedef itk::NeighborhoodIterator<ImageType> NeiIterType3D;
typedef itk::LineIterator<ImageType> LineIteratorType;

typedef ImageType2D::PointType Point2D;
typedef ImageType::PointType Point;
typedef ImageType2D::IndexType Index2D;
typedef std::pair<Point,Point> PointPair;
typedef std::pair<Point2D,Point2D> Point2DPair;
typedef std::pair<double,Point2DPair> DistPointPair2D;
typedef itk::LineIterator<ImageType2D> LineIteratorType2D;
typedef std::pair<double,PointPair> DistPointPair3D;

// Filter Types
typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ThresholdType;
typedef itk::ExtractImageFilter<ImageType, ImageType2D> ExtractorType;
typedef itk::Rigid3DTransform<double> TransformType;
typedef itk::ResampleImageFilter<ImageType,ImageType> ResampleFilterType;
typedef itk::NearestNeighborInterpolateImageFunction<ImageType> InterpolatorType;
typedef itk::SimpleContourExtractorImageFilter<ImageType,ImageType> ContourExtractorType;
typedef itk::SimpleContourExtractorImageFilter<ImageType2D,ImageType2D> ContourExtractorType2D;
typedef itk::ConnectedThresholdImageFilter<ImageType,ImageType> ConnectedThresholdType;

typedef itk::BinaryBallStructuringElement<PixelType, 3> KernelType;
typedef itk::BinaryErodeImageFilter<ImageType,ImageType,KernelType> EroderType;

typedef itk::AndImageFilter<ImageType,ImageType> AndFilterType;

// I/O Types
typedef itk::ImageFileReader< ImageType >  ReaderType;
typedef itk::ImageFileWriter< ImageType >  WriterType;
typedef itk::ImageFileWriter<ImageType2D> WriterType2D;

int DoIt( int argc, char * argv[]);
void DoItSimple(ImageType::Pointer, DistPointPair3D&, DistPointPair3D&, DistPointPair3D&);
void DoItHard(ImageType::Pointer, DistPointPair3D&, DistPointPair3D&, DistPointPair3D&);

void SaveImage(ImageType::Pointer, std::string);

void SaveIndexAsRASPoint(ImageType::Pointer,
                         ImageType::IndexType&, 
                         std::ofstream&, 
                         std::string);
void SavePointAsRASPoint(ImageType::Pointer image,
                         ImageType::PointType& point, 
                         std::ofstream& fs, 
                         std::string name);
int FindBoundaryPoints(ImageType2D::Pointer,
                       std::vector<ImageType2D::PointType>&);
int FindBoundaryPoints3D(ImageType::Pointer slice,
                        std::vector<ImageType::PointType>& boundaryPts);
double FindInSliceDiameters(ImageType2D::Pointer,
                            std::vector<ImageType2D::PointType>&,
                            DistPointPair2D&, DistPointPair2D&);
double PtDistance2D(Point2D p0, Point2D p1);
double PtDistance(ImageType::PointType p0, ImageType::PointType p1);
double EstimateDiameterBresenham(ImageType2D::Pointer, Point2D, Point2D, double);
double EstimateDiameterBresenham3D(ImageType::Pointer image, 
  ImageType::PointType pt0, ImageType::PointType pt1,
  double currentDiameter);
void UpdateImageWithDiameter(ImageType::Pointer, ImageType::PointType, ImageType::PointType);
void FindLabelBoundingBox(ImageType::Pointer, PixelType, ImageType::IndexType&, ImageType::IndexType&);
ImageType::IndexType FindLargestAreaSliceAlongDirection(ImageType::Pointer, int);
ImageType2D::PointType SegmentIntersection2D(Point2DPair &p0, Point2DPair &p1);
double FindLabelVolume(ImageType::Pointer image, int label);

struct DistPtPairSort
{
  bool operator()(DistPointPair2D p0, DistPointPair2D p1) { 
    return p0.first>p1.first;
  }
};

struct DistPtPairSort3D
{
  bool operator()(DistPointPair3D p0, DistPointPair3D p1) { 
    return p0.first>p1.first;
  }
};


int main( int argc, char * argv[] )
{
  
  PARSE_ARGS;

  try
    {
    return DoIt(argc, argv);
    }
  catch( itk::ExceptionObject &excep)
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

int DoIt( int argc, char * argv[])
{
  PARSE_ARGS;

  DistPointPair3D diameterA, diameterB, diameterC;
  
  // fiducial list will store three pairs of fiducials, each corresponding to
  // the endpoints of the three diameters ordered by length
  std::ofstream fiducialsFile(outputFiducialFile.c_str()); 

// Instances
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName (inputVolume.c_str());
  reader->Update();

  itk::ImageDuplicator<ImageType>::Pointer duplicator = 
    itk::ImageDuplicator<ImageType>::New();
  duplicator->SetInputImage(reader->GetOutput());
  duplicator->Update();
  ImageType::Pointer outputImage = duplicator->GetOutput();
  outputImage->FillBuffer(0);

  ThresholdType::Pointer threshold = ThresholdType::New();
  threshold->SetInput(reader->GetOutput());
  threshold->SetUpperThreshold(inputLabel);
  threshold->SetLowerThreshold(inputLabel);
  threshold->SetInsideValue(1);
  threshold->SetOutsideValue(0);
  threshold->Update();

  ImageType::Pointer binaryImage = threshold->GetOutput();

  if(1)
    {
    std::cout << "2d diameter estimation: " << std::endl;
    DoItSimple(binaryImage, diameterA, diameterB, diameterC);

    SavePointAsRASPoint(binaryImage, diameterA.second.first, fiducialsFile, std::string("DA-0"));
    SavePointAsRASPoint(binaryImage, diameterA.second.second, fiducialsFile, std::string("DA-1"));
    std::cout << "Largest diameter (A) length: " << diameterA.first << std::endl;
    SavePointAsRASPoint(binaryImage, diameterB.second.first, fiducialsFile, std::string("DB-0"));
    SavePointAsRASPoint(binaryImage, diameterB.second.second, fiducialsFile, std::string("DB-1"));
    std::cout << "Second diameter (B) length: " << diameterB.first << std::endl;
    SavePointAsRASPoint(binaryImage, diameterC.second.first, fiducialsFile, std::string("DC-0"));
    SavePointAsRASPoint(binaryImage, diameterC.second.second, fiducialsFile, std::string("DC-1"));
    std::cout << "Third diameter (C) length: " << diameterC.first << std::endl;

    UpdateImageWithDiameter(outputImage, diameterA.second.first, diameterA.second.second);
    UpdateImageWithDiameter(outputImage, diameterB.second.first, diameterB.second.second);
    UpdateImageWithDiameter(outputImage, diameterC.second.first, diameterC.second.second);

    std::cout << "Estimate tumor volume (ABC/2): " << diameterA.first*diameterB.first*diameterC.first/2. << std::endl;
    std::cout << "True tumor volume: " << FindLabelVolume(binaryImage, 1) << " mm^3" << std::endl;
    }

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputDiameters.c_str());
  writer->SetInput(outputImage);
  writer->Update();
  
  return EXIT_SUCCESS;
}

void SaveIndexAsRASPoint(ImageType::Pointer image,
                         ImageType::IndexType& index, 
                         std::ofstream& fs, 
                         std::string name)
{
  ImageType::PointType point;

  image->TransformIndexToPhysicalPoint(index, point);
  point[0] = -point[0];
  point[1] = -point[1];

  fs << name << "," << point[0] << "," << point[1] << "," << point[2] << std::endl;
}

void SavePointAsRASPoint(ImageType::Pointer image,
                         ImageType::PointType& point, 
                         std::ofstream& fs, 
                         std::string name)
{
  point[0] = -point[0];
  point[1] = -point[1];

  fs << name << "," << point[0] << "," << point[1] << "," << point[2] << std::endl;
}

int FindBoundaryPoints(ImageType2D::Pointer slice,
                        std::vector<ImageType2D::PointType>& boundaryPts)
{
  boundaryPts.clear();
  int i;
  NeiIterType::RadiusType neiRadius;
  neiRadius.Fill(1);
  NeiIterType neiIt(neiRadius, slice, slice->GetLargestPossibleRegion());
  for(neiIt.GoToBegin();!neiIt.IsAtEnd();++neiIt)
    {
    if(neiIt.GetPixel(4)==0)
      continue;
    for(i=0;i<9;i++)
      if(neiIt.GetPixel(i)==0)
        {
        ImageType2D::PointType boundaryPt;
        ImageType2D::IndexType boundaryIdx;
        neiIt.SetPixel(4, neiIt.GetPixel(4)*4);
        slice->TransformIndexToPhysicalPoint(neiIt.GetIndex(), boundaryPt);
        slice->TransformPhysicalPointToIndex(boundaryPt, boundaryIdx);
           boundaryPts.push_back(boundaryPt);
           break;
        }
    }
  return boundaryPts.size();
}

int FindBoundaryPoints3D(ImageType::Pointer slice,
                        std::vector<ImageType::PointType>& boundaryPts)
{
  boundaryPts.clear();
  int i;
  NeiIterType3D::RadiusType neiRadius;
  neiRadius.Fill(1);
  NeiIterType3D neiIt(neiRadius, slice, slice->GetLargestPossibleRegion());
  for(neiIt.GoToBegin();!neiIt.IsAtEnd();++neiIt)
    {
    if(neiIt.GetPixel(13)==0)
      continue;
    for(i=0;i<27;i++)
      if(neiIt.GetPixel(i)==0)
        {
        ImageType::PointType boundaryPt;
        ImageType::IndexType boundaryIdx;
        slice->TransformIndexToPhysicalPoint(neiIt.GetIndex(), boundaryPt);
        slice->TransformPhysicalPointToIndex(boundaryPt, boundaryIdx);
           boundaryPts.push_back(boundaryPt);
           break;
        }
    }
  return boundaryPts.size();
}

double FindInSliceDiameters(ImageType2D::Pointer slice, 
                            std::vector<ImageType2D::PointType> &boundaryPts,
                            DistPointPair2D &maxDiameterTuple,
                            DistPointPair2D &maxDiameterTuple2nd)
{
  int numBoundaryPts = boundaryPts.size();
  int i, j;
  std::vector<DistPointPair2D> distPtVector;
  
  // Find all distance combinations, excluding duplicates
  double maxDist = 0;

  Point2D zeroPoint;
  zeroPoint.Fill(0.0);

  Point2DPair maxDiameterPair = Point2DPair(zeroPoint,zeroPoint);
  for(i=0;i<numBoundaryPts;i++)
    {
    for(j=i+1;j<numBoundaryPts;j++)
      {
      double dist;
      dist = PtDistance2D(boundaryPts[i],boundaryPts[j]);
      distPtVector.push_back(
        DistPointPair2D(dist,Point2DPair(boundaryPts[i],boundaryPts[j])));
      if(dist>maxDist)
        {
        maxDiameterPair = Point2DPair(boundaryPts[i],boundaryPts[j]);
        maxDist = dist;
        }
      }
    }

  // sort them by the estimated diameter
  std::sort(distPtVector.begin(), distPtVector.end(), DistPtPairSort());

  // Follow the line s for each pair to find intersection points, exclude 
  // outside segments from the diameter estimate
  maxDiameterTuple = distPtVector[0];
  maxDiameterTuple2nd = distPtVector[1];
  maxDist = maxDiameterTuple.first;
  double diameterEstimate = 0, diameterEstimate2nd = 0;
  i = 0;
  int tupleCnt = distPtVector.size();
  for(std::vector<DistPointPair2D>::const_iterator pIt = distPtVector.begin();
      pIt!=distPtVector.end();++pIt,++i)
    {

    double currentDiameter;

    currentDiameter = (*pIt).first;
    currentDiameter = EstimateDiameterBresenham(slice, 
      (*pIt).second.first, (*pIt).second.second, currentDiameter);
    
    // if the current estimate is larger than the distance between the next
    // tuple of points, then we have the largest diameter possible
    if(currentDiameter > diameterEstimate)
      {
      diameterEstimate = currentDiameter;
      maxDiameterTuple = *pIt;
      maxDiameterTuple.first = currentDiameter;
      }

    if(i>tupleCnt-1)
      if(diameterEstimate > (*(pIt+1)).first)
        break;
    }
 
  itk::Vector<double,2> dv0, dv1;
  dv0[0] = maxDiameterTuple.second.first[0]-maxDiameterTuple.second.second[0];
  dv0[1] = maxDiameterTuple.second.first[1]-maxDiameterTuple.second.second[1];
  j = 0;
  double dot, mindot = 1;
  for(std::vector<DistPointPair2D>::const_iterator pIt = distPtVector.begin();
      pIt!=distPtVector.end();++pIt,++i)
    {
    dv1[0] = (*pIt).second.first[0]-(*pIt).second.second[0];
    dv1[1] = (*pIt).second.first[1]-(*pIt).second.second[1];

    // consider only those diameters perpendicular to the first one
    dot = fabs(dv0*dv1/(maxDiameterTuple.first*(*pIt).first));
    if(dot>mindot)
      continue;
    j++;
    double currentDiameter;
    currentDiameter = (*pIt).first;
    currentDiameter = EstimateDiameterBresenham(slice, 
      (*pIt).second.first, (*pIt).second.second, currentDiameter);
    if(dot<mindot)
      {
      diameterEstimate2nd = currentDiameter;
      maxDiameterTuple2nd = *pIt;
      maxDiameterTuple2nd.first = diameterEstimate2nd;
      mindot = dot;
      }
    else if(currentDiameter > diameterEstimate2nd)
      {
        diameterEstimate2nd = currentDiameter;
        maxDiameterTuple2nd = *pIt;
        maxDiameterTuple2nd.first = diameterEstimate2nd;
      }
    /*
    // if the current estimate is larger than the distance between the next
    // tuple of points, then we have the largest diameter possible
    if(i>tupleCnt-1)
      if(diameterEstimate2nd > (*(pIt+1)).first)
        break;
        */
    }
  if(!j)
    {
    std::cerr << "No perpendicular diameters found" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

double PtDistance2D(Point2D p0, Point2D p1)
{
  return sqrt((p0[0]-p1[0])*(p0[0]-p1[0])+
              (p0[1]-p1[1])*(p0[1]-p1[1]));
}

double PtDistance(ImageType::PointType p0, ImageType::PointType p1)
{
  return sqrt((p0[0]-p1[0])*(p0[0]-p1[0])+
              (p0[1]-p1[1])*(p0[1]-p1[1])+
              (p0[2]-p1[2])*(p0[2]-p1[2]));
}

// Use Bresenham line iterator to find the diameter segments that are outside
// the binary segmentation, and update the initial estimate accodingly
double EstimateDiameterBresenham(ImageType2D::Pointer slice, 
                                 Point2D pt0, Point2D pt1, double currentDiameter)
{
  bool outside = false;
  Index2D idx0, idx1;

  slice->TransformPhysicalPointToIndex(pt0, idx0);
  slice->TransformPhysicalPointToIndex(pt1, idx1);

  LineIteratorType2D lit(slice, idx0, idx1);
  ImageType2D::IndexType outsideIdx0, outsideIdx1;
  for(lit.GoToBegin();!lit.IsAtEnd();++lit)
    {
    if(!lit.Get())
      {
      if(!outside)
        {
        outside = true;
        outsideIdx0 = lit.GetIndex();
        }
      }
    if(lit.Get())
      {
      if(outside)
        {
        ImageType2D::PointType pt0, pt1;
        outsideIdx1 = lit.GetIndex();
        slice->TransformIndexToPhysicalPoint(outsideIdx0, pt0);
        slice->TransformIndexToPhysicalPoint(outsideIdx1, pt1);
        currentDiameter -= PtDistance2D(pt0, pt1);
        outside = false;
        }
      }
    }
  return currentDiameter;
}

void UpdateImageWithDiameter(ImageType::Pointer image, ImageType::PointType pt0, ImageType::PointType pt1)
{
  static unsigned char labelId = 0;
  ImageType::IndexType idx0, idx1;

  labelId++;
  pt0[0] = -pt0[0];
  pt0[1] = -pt0[1];
  pt1[0] = -pt1[0];
  pt1[1] = -pt1[1];
  image->TransformPhysicalPointToIndex(pt0, idx0);
  image->TransformPhysicalPointToIndex(pt1, idx1);

  LineIteratorType lit(image, idx0, idx1);
  for(lit.GoToBegin();!lit.IsAtEnd();++lit)
    lit.Set(labelId);
}

void FindLabelBoundingBox(ImageType::Pointer image, 
                          PixelType label, 
                          ImageType::IndexType& bbox0, 
                          ImageType::IndexType& bbox1)
  {
  ConstIterWithIndex inputIt(image, image->GetLargestPossibleRegion());
  ImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();
  bbox1[0] = 0;
  bbox1[1] = 0;
  bbox1[2] = 0;
  bbox0[0] = imageSize[0];
  bbox0[1] = imageSize[1];
  bbox0[2] = imageSize[2];
  for(inputIt.GoToBegin();!inputIt.IsAtEnd();++inputIt)
    {
    if(inputIt.Get())
      {
      ImageType::IndexType idx = inputIt.GetIndex();
      if(bbox0[0]>idx[0])
        bbox0[0] = idx[0];
      if(bbox0[1]>idx[1])
        bbox0[1] = idx[1];
      if(bbox0[2]>idx[2])
        bbox0[2] = idx[2];
      if(bbox1[0]<idx[0])
        bbox1[0] = idx[0];
      if(bbox1[1]<idx[1])
        bbox1[1] = idx[1];
      if(bbox1[2]<idx[2])
        bbox1[2] = idx[2];
      }
    }

  }


double PtDistanceSquared2D(Point2D p0, Point2D p1)
{
  return (p0[0]-p1[0])*(p0[0]-p1[0])+
    (p0[1]-p1[1])*(p0[1]-p1[1]);
}

ImageType2D::PointType SegmentIntersection2D(Point2DPair &p0, Point2DPair &p1)
  {
  double u;
  ImageType2D::PointType intersection;
  double x1 = p0.first[0], x2 = p0.second[0], x3 = p1.first[0], x4 = p1.second[0],
         y1 = p0.first[1], y2 = p0.second[1], y3 = p1.first[1], y4 = p1.second[1];
  u = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
  intersection[0] = x1+u*(x2-x1);
  intersection[1] = y1+u*(y2-y1);
  return intersection;
  }

double FindLabelVolume(ImageType::Pointer image, int label)
  {
  double volume = 0.0;
  ImageType::SpacingType spacing = image->GetSpacing();

  ConstIter it(image, image->GetLargestPossibleRegion());
  for(it.GoToBegin();!it.IsAtEnd();++it){
    if(it.Get() == label)
      volume++;
  }
  volume *= spacing[0]*spacing[1]*spacing[2];
  return volume;
  }

void DoItSimple(ImageType::Pointer binaryImage, DistPointPair3D& diameterA, 
                DistPointPair3D &diameterB, DistPointPair3D& diameterC)
  {
  int i, j;

  // find bounding box region for the label of interest
  ImageType::IndexType bbox0, bbox1;
  FindLabelBoundingBox(binaryImage, 1, bbox0, bbox1);


  ImageType::RegionType bboxRegion;
  ImageType::SizeType bboxSize;

  bboxSize[0] = bbox1[0]-bbox0[0];
  bboxSize[1] = bbox1[1]-bbox0[1];
  bboxSize[2] = bbox1[2]-bbox0[2];
  bboxRegion.SetIndex(bbox0);
  bboxRegion.SetSize(bboxSize);
  
  // Find the orthogonal slice with the maximum area
  ImageType::SpacingType imageSpacing = binaryImage->GetSpacing();
  double *sliceAreaCounters[3];

  sliceAreaCounters[0] = new double[bboxSize[0]];
  sliceAreaCounters[1] = new double[bboxSize[1]];
  sliceAreaCounters[2] = new double[bboxSize[2]];

  for(j=0;j<3;j++)
    {
    for(i=0;i<(int)bboxSize[j];i++)
      {
      sliceAreaCounters[j][i] = 0;
      }
    }

  ImageType::IndexType maxAreaIdx;
  double maxArea = 0.;
  maxAreaIdx[0] = 0;
  maxAreaIdx[1] = 0;
  maxAreaIdx[2] = 0;
  ConstIterWithIndex bboxIt(binaryImage, bboxRegion);
  for(bboxIt.GoToBegin();!bboxIt.IsAtEnd();++bboxIt)
    {
    if(bboxIt.Get())
      {
      ImageType::IndexType idx = bboxIt.GetIndex();
      idx[0] = idx[0]-bbox0[0];
      idx[1] = idx[1]-bbox0[1];
      idx[2] = idx[2]-bbox0[2];
      sliceAreaCounters[0][idx[0]] += imageSpacing[1]*imageSpacing[2];
      sliceAreaCounters[1][idx[1]] += imageSpacing[0]*imageSpacing[2];
      sliceAreaCounters[2][idx[2]] += imageSpacing[0]*imageSpacing[1];

      // update the index of the largest are slice
      if(sliceAreaCounters[0][idx[0]]>maxArea)
        {
        maxArea = sliceAreaCounters[0][idx[0]];
        maxAreaIdx[0] = idx[0]+bbox0[0];
        maxAreaIdx[1] = 0;
        maxAreaIdx[2] = 0;
        }
      if(sliceAreaCounters[1][idx[1]]>maxArea)
        {
        maxArea = sliceAreaCounters[1][idx[1]];
        maxAreaIdx[0] = 0;
        maxAreaIdx[1] = idx[1]+bbox0[1];
        maxAreaIdx[2] = 0;
        }
      if(sliceAreaCounters[2][idx[2]]>maxArea)
        {
        maxArea = sliceAreaCounters[2][idx[2]];
        maxAreaIdx[0] = 0;
        maxAreaIdx[1] = 0;
        maxAreaIdx[2] = idx[2]+bbox0[2];
        }
      }
    }

  delete [] sliceAreaCounters[0];
  delete [] sliceAreaCounters[1];
  delete [] sliceAreaCounters[2];

  std::cout << "Max slice area: " << maxArea << " mm^3" << std::endl;
  // extract the largest area slice -- maxAreaIdx is non-zero for the
  // collapsing dimension
  ExtractorType::Pointer extractor = ExtractorType::New();
  int extractionIdxId;
  ImageType::RegionType largestSliceRegion;
  ImageType::SizeType imageSize = binaryImage->GetLargestPossibleRegion().GetSize();
  for(i=0;i<3;i++)
    if(maxAreaIdx[i])
      {
      imageSize[i] = 0;
      extractionIdxId = i;
      }
  largestSliceRegion.SetSize(imageSize);
  largestSliceRegion.SetIndex(maxAreaIdx);
  extractor->SetInput(binaryImage);
  extractor->SetExtractionRegion(largestSliceRegion);
  extractor->Update();
  ImageType2D::Pointer largestSlice = extractor->GetOutput();
  std::cout << "Extraction dimension: " << extractionIdxId << ", index: " << 
    maxAreaIdx[extractionIdxId] << std::endl;

  // find boundary points for the largest slice
  std::vector<ImageType2D::PointType> boundaryPoints;
  FindBoundaryPoints(largestSlice, boundaryPoints);
  std::cout << boundaryPoints.size() << " boundary points found" << std::endl;
  DistPointPair2D maxDiameterTuple, maxDiameterTuple2nd;
  DistPointPair3D maxDiameterTuple3rd;
  binaryImage->TransformIndexToPhysicalPoint(bbox0, maxDiameterTuple3rd.second.first);
  binaryImage->TransformIndexToPhysicalPoint(bbox1, maxDiameterTuple3rd.second.second);

  FindInSliceDiameters(largestSlice, boundaryPoints, 
    maxDiameterTuple, maxDiameterTuple2nd);
  std::cout << "Diameters: " << maxDiameterTuple.second.first << " " << maxDiameterTuple.second.second << std::endl;
  std::cout << "Diameters: " << maxDiameterTuple2nd.second.first << " " << maxDiameterTuple2nd.second.second << std::endl;
  ImageType2D::PointType intersectionPt = 
    SegmentIntersection2D(maxDiameterTuple.second, maxDiameterTuple2nd.second);
  ImageType::PointType intersectionPt3d;

  // fill in the missing slice id
  ImageType::PointType maxAreaPt;
  binaryImage->TransformIndexToPhysicalPoint(maxAreaIdx, maxAreaPt);
  for(i=0,j=0;i<3;i++)
    {
    if(maxAreaIdx[i])
      {
      diameterA.second.first[i] = maxAreaPt[i];
      diameterA.second.second[i] = maxAreaPt[i];
      diameterB.second.first[i] = maxAreaPt[i];
      diameterB.second.second[i] = maxAreaPt[i];
      intersectionPt3d[i] = maxAreaPt[i];
      }
    else
      {
      diameterA.second.first[i] = maxDiameterTuple.second.first[j];
      diameterA.second.second[i] = maxDiameterTuple.second.second[j];
      diameterB.second.first[i] = maxDiameterTuple2nd.second.first[j];
      diameterB.second.second[i] = maxDiameterTuple2nd.second.second[j];
      intersectionPt3d[i] = intersectionPt[j];
      j++;
      }
    }

  diameterA.first = maxDiameterTuple.first;
  diameterB.first = maxDiameterTuple2nd.first;

  ImageType::IndexType intersectionIdx, curIdx;
  binaryImage->TransformPhysicalPointToIndex(intersectionPt3d, intersectionIdx);
  curIdx = intersectionIdx;
  std::cout << "IntersectionIdx 3d: " << intersectionIdx << std::endl;
  for(i=intersectionIdx[extractionIdxId];i>bbox0[extractionIdxId];i--)
    {
    curIdx[extractionIdxId] = i;
    if(binaryImage->GetPixel(curIdx))
      continue;
    i++;
    break;
    }
  binaryImage->TransformIndexToPhysicalPoint(curIdx, diameterC.second.first);

  for(i=intersectionIdx[extractionIdxId];i<bbox1[extractionIdxId];i++)
    {
    curIdx[extractionIdxId] = i;
    if(binaryImage->GetPixel(curIdx))
      continue;
    i--;
    break;
    }
  binaryImage->TransformIndexToPhysicalPoint(curIdx, diameterC.second.second);
  diameterC.first = PtDistance(diameterC.second.first, diameterC.second.second);
  }

void SaveImage(ImageType::Pointer image, std::string name)
  {
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(name.c_str());
  writer->SetInput(image);
  writer->Update();
  }

/*
void DoItHard(ImageType::Pointer binaryImage, DistPointPair3D& diameterA, 
                DistPointPair3D &diameterB, DistPointPair3D& diameterC)
  {
  std::vector<ImageType::PointType> boundaryPts;
  long boundaryPtsCnt = FindBoundaryPoints3D(binaryImage, boundaryPts);
  std::cout << "Found " << boundaryPtsCnt << " boundary points" << std::endl;

  int numBoundaryPts = boundaryPts.size();
  int i, j;
  std::vector<DistPointPair3D> distPtVector;
  
  // Find all distance combinations, excluding duplicates
  double maxDist = 0;

  ImageType::PointType zeroPoint;
  zeroPoint.Fill(0.0);

  PointPair maxDiameterPair = PointPair(zeroPoint,zeroPoint);
  for(i=0;i<numBoundaryPts;i++)
    {
    for(j=i+1;j<numBoundaryPts;j++)
      {
      double dist;
      dist = PtDistance(boundaryPts[i],boundaryPts[j]);
      distPtVector.push_back(
        DistPointPair3D(dist,PointPair(boundaryPts[i],boundaryPts[j])));
      if(dist>maxDist)
        {
        maxDiameterPair = PointPair(boundaryPts[i],boundaryPts[j]);
        maxDist = dist;
        }
      }
    }

  // sort them by the estimated diameter
  std::sort(distPtVector.begin(), distPtVector.end(), DistPtPairSort3D());

  // Follow the line s for each pair to find intersection points, exclude 
  // outside segments from the diameter estimate
  DistPointPair3D maxDiameterTuple = distPtVector[0];
  maxDist = maxDiameterTuple.first;
  double diameterEstimate = 0;
  i = 0;
  int tupleCnt = distPtVector.size();
  for(std::vector<DistPointPair3D>::const_iterator pIt = distPtVector.begin();
      pIt!=distPtVector.end();++pIt,++i)
    {

    double currentDiameter;

    currentDiameter = (*pIt).first;
    currentDiameter = EstimateDiameterBresenham3D(binaryImage, 
      (*pIt).second.first, (*pIt).second.second, currentDiameter);
    
    // if the current estimate is larger than the distance between the next
    // tuple of points, then we have the largest diameter possible
    if(currentDiameter > diameterEstimate)
      {
      diameterEstimate = currentDiameter;
      maxDiameterTuple = *pIt;
      maxDiameterTuple.first = currentDiameter;
      }

    if(i>tupleCnt-1)
      if(diameterEstimate > (*(pIt+1)).first)
        break;
    }

#if 0
  // extract the contour
  ContourExtractorType::Pointer contourer = ContourExtractorType::New();
  ImageType::SizeType radius;
  radius[0] = 1;
  radius[1] = 1;
  radius[2] = 1;
  contourer->SetInput(binaryImage);
  contourer->SetRadius(radius);
  contourer->SetInputForegroundValue(1);
  contourer->SetInputBackgroundValue(0);
  contourer->Update();
  ImageType::Pointer contour = contourer->GetOutput();
#endif // 0



#if 0
  // fill the space outside the tumor cavity
  ImageType::IndexType seed;
  ConstIterWithIndex it(binaryImage, binaryImage->GetLargestPossibleRegion());
  for(it.GoToBegin();!it.IsAtEnd();++it)
    if(!binaryImage->GetPixel(it.GetIndex()))
      seed = it.GetIndex();
  std::cout << "Connected threshold seed: " << seed << std::endl;
  ConnectedThresholdType::Pointer conthresh = ConnectedThresholdType::New();
  conthresh->SetSeed(seed);
  conthresh->SetLower(0);
  conthresh->SetUpper(1);
  conthresh->SetReplaceValue(255);
  conthresh->SetInput(contourer->GetOutput());
  conthresh->Update();

  // erode to reduce the number of points
  KernelType kernel;
  kernel.SetRadius(1);
  EroderType::Pointer eroder = EroderType::New();
  eroder->SetKernel(kernel);
  eroder->SetForegroundValue(255);
  eroder->SetInput(conthresh->GetOutput());
  eroder->Update();
  
  // do binary addition of the original contour (need to erode only from the
  // inside of the mask)
  AndFilterType::Pointer ander = AndFilterType::New();
  ander->SetInput1(eroder->GetOutput());
  ander->SetInput2(contourer->GetOutput());
  ander->Update();
  SaveImage(eroder->GetOutput(), std::string("eroded.nrrd"));
  SaveImage(ander->GetOutput(), std::string("eroded_contour.nrrd"));
#endif // 0
  }

double EstimateDiameterBresenham3D(ImageType::Pointer image, 
  ImageType::PointType pt0, ImageType::PointType pt1,
  double currentDiameter){
  bool outside = false;
  ImageType::IndexType idx0, idx1;

  image->TransformPhysicalPointToIndex(pt0, idx0);
  image->TransformPhysicalPointToIndex(pt1, idx1);

  LineIteratorType lit(image, idx0, idx1);
  ImageType::IndexType outsideIdx0, outsideIdx1;
  for(lit.GoToBegin();!lit.IsAtEnd();++lit){
    if(!lit.Get()){
      if(!outside){
        outside = true;
        outsideIdx0 = lit.GetIndex();
      }
    }
    if(lit.Get()){
      if(outside){
        ImageType::PointType pt0, pt1;
        outsideIdx1 = lit.GetIndex();
        image->TransformIndexToPhysicalPoint(outsideIdx0, pt0);
        image->TransformIndexToPhysicalPoint(outsideIdx1, pt1);
        currentDiameter -= PtDistance(pt0, pt1);
        outside = false;
      }
    }
  }
  return currentDiameter;
}
*/
