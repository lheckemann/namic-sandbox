
/* CLI to apply B-spline transform to a fiducial list. B-spline transform must
 * include bulk affine transform as the second transform.
 *
 * Andrey Fedorov / fedorov@bwh.harvard.edu
 */

#include <itkTransform.h>
#include <itkTransformFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageFileReader.h>
#include <itkBSplineDeformableTransform.h>
#include <itkAffineTransform.h>
#include <itkImageFileReader.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkIterativeInverseDeformationFieldImageFilter.h>
#include "TransformFiducialListCLP.h"
#include "itkOrientedImage.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include <vector>
#include <map>

const int Dimension = 3;
typedef itk::OrientedImage<int, 3> ImageType;
typedef itk::TransformFileReader::Pointer TransformReaderPointer;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageRegionConstIterator<ImageType> ConstIterType;
typedef itk::ImageRegionConstIteratorWithIndex<ImageType> ConstIterWithIndexType;
typedef itk::BSplineDeformableTransform< double, Dimension , Dimension > 
BSplineDeformableTransformType;
typedef itk::AffineTransform< double, Dimension > AffineTransformType;
typedef itk::ResampleImageFilter<ImageType,ImageType,double> ResamplerType;
typedef itk::NearestNeighborInterpolateImageFunction<ImageType,double> NNType;

double DistanceBwPoints(ImageType::PointType, ImageType::PointType);

int main( int argc , char * argv[] )
{
  PARSE_ARGS;


  TransformReaderPointer transformFile = itk::TransformFileReader::New() ;
  transformFile->SetFileName(transformationFile.c_str()) ;
  transformFile->Update() ;
  BSplineDeformableTransformType::Pointer BSplineTransform = BSplineDeformableTransformType::New();  
  AffineTransformType::Pointer affineTfm = AffineTransformType::New();


  ReaderType::Pointer mReader = ReaderType::New();
  mReader->SetFileName(MovingImage.c_str());
  mReader->Update();

  ReaderType::Pointer rReader = ReaderType::New();
  rReader->SetFileName(ReferenceImage.c_str());
  rReader->Update();

  ImageType::Pointer mImage = mReader->GetOutput();
  ImageType::Pointer rImage = rReader->GetOutput();

  rImage->FillBuffer(0);
  mImage->FillBuffer(0);

  if ( transformFile->GetTransformList()->size() == 0  ) 
    { 
    std::cerr << "Error, the transform does not exist." << std::endl;
    return EXIT_FAILURE;
    }

  if (!dynamic_cast<BSplineDeformableTransformType* > ( transformFile->GetTransformList()->front().GetPointer()))
    {       
    std::cerr << "Error, the first transform is not BSpline." << std::endl;
    return EXIT_FAILURE;
    }
  BSplineTransform = dynamic_cast<BSplineDeformableTransformType* > ( transformFile->GetTransformList()->front().GetPointer());
  transformFile->GetTransformList()->pop_front();
  if(!dynamic_cast<AffineTransformType*>(transformFile->GetTransformList()->front().GetPointer()))
    {
    std::cerr << "Error: the second transform is not affine" << std::endl;
    return EXIT_FAILURE;
    }
  affineTfm = dynamic_cast<AffineTransformType*> (transformFile->GetTransformList()->front().GetPointer());

  BSplineTransform->SetBulkTransform(affineTfm);
  std::vector<ImageType::PointType> inputFiducialList;
  std::vector<ImageType::PointType> outputFiducialList;
  std::map<int,double> fiducialId2Distance;
  ::size_t nFiducials = inputFiducialList.size(), i;

  if(FiducialsFileName != ""){
    fiducials.clear();
    std::ifstream fIn(FiducialsFileName.c_str());
    while(fIn.good()){
      ImageType::PointType pIn;
      char line[255];
      fIn.getline(line,255);
      if(line[0] == '#')
        continue;
      if(!strlen(line))
        break;
      char* item = strtok(line, ",");
      if(item==NULL){
        std::cerr << "Corrupted fiducials file!" << std::endl;
        return -1;
      }
      ImageType::PointType pt;
      pt[0] = -1.*atof(strtok(NULL,","));
      pt[1] = -1.*atof(strtok(NULL,","));
      pt[2] = atof(strtok(NULL,","));
      inputFiducialList.push_back(pt);
      outputFiducialList.push_back(pt);
    }
  } else {
    if(!fiducials.size()){
      std::cerr << "fiducials not specified!" << std::endl;
      return -1;
    }
    nFiducials = fiducials.size();
    for(i=0;i<nFiducials;++i){
      ImageType::PointType pIn;
      ImageType::IndexType idx;

      pIn[0] = -1.*fiducials[i][0];
      pIn[1] = -1.*fiducials[i][1];
      pIn[2] = fiducials[i][2];

      inputFiducialList.push_back(pIn);
      outputFiducialList.push_back(pIn);
    }
  }

  nFiducials = inputFiducialList.size();

  std::cout << "Total input fiducials: " << nFiducials << std::endl;
  for(i=0;i<nFiducials;++i){
    ImageType::PointType pIn;
    ImageType::IndexType idx;

    pIn = inputFiducialList[i];

    fiducialId2Distance[i] = 10000.;

    int searchRadius = 3;
    mImage->TransformPhysicalPointToIndex(pIn, idx);
    for(int ii=-searchRadius;ii<searchRadius+1;ii++){
      for(int jj=-searchRadius;jj<searchRadius+1;jj++){
        for(int kk=-searchRadius;kk<searchRadius;kk++){
          ImageType::IndexType idx1;
          idx1[0] = idx[0]+ii;
          idx1[1] = idx[1]+jj;
          idx1[2] = idx[2]+kk;
          if(mImage->GetBufferedRegion().IsInside(idx1))
            mImage->SetPixel(idx1, i+1);
        }
      }
    }
  }

  NNType::Pointer nnInterp = NNType::New();
  ResamplerType::Pointer resampler = ResamplerType::New();
  resampler->SetInput(mImage);
  resampler->SetTransform(BSplineTransform);
  resampler->SetOutputParametersFromImage(rImage);
  resampler->SetInterpolator(nnInterp);  
  resampler->Update();

  ImageType::Pointer resampledImage = resampler->GetOutput();
  ConstIterWithIndexType iter(resampledImage, resampledImage->GetBufferedRegion());

  // iterate over all non-zero voxels
  for(iter.GoToBegin();!iter.IsAtEnd();++iter){

    if(!iter.Get())
      continue;

    ImageType::PointType pPtFixed, pPtMoving;
    ImageType::IndexType pIdxFixed;

    pIdxFixed = iter.GetIndex();
    resampledImage->TransformIndexToPhysicalPoint(pIdxFixed, pPtFixed);

    // if a voxel is non-zero, find which of the input fiducials is closest to
    // it
    pPtMoving = BSplineTransform->TransformPoint(pPtFixed);
    for(i=0;i<nFiducials;i++){
      double thisDistance = DistanceBwPoints(pPtMoving, inputFiducialList[i]);
      if(thisDistance<fiducialId2Distance[i]){
        outputFiducialList[i] = pPtFixed;
        fiducialId2Distance[i] = thisDistance;
      }
    }
  }

  std::ofstream outFiducialFile(outputFile.c_str());
  for(i=0;i<nFiducials;i++){
    outFiducialFile << "F" << i << ", " << -1.*outputFiducialList[i][0] << ", " 
      << -1.*outputFiducialList[i][1] << ", " << outputFiducialList[i][2] << ", 1, 1" << std::endl;
    std::cout << "F" << i << ", " << -1.*outputFiducialList[i][0] << ", " 
      << -1.*outputFiducialList[i][1] << ", " << outputFiducialList[i][2] << ", 1, 1" << std::endl;
  }
  outFiducialFile.close();
  return EXIT_SUCCESS;
}

double DistanceBwPoints(ImageType::PointType p0, ImageType::PointType p1){
  return sqrt((p0[0]-p1[0])*(p0[0]-p1[0])+
              (p0[1]-p1[1])*(p0[1]-p1[1])+
              (p0[2]-p1[2])*(p0[2]-p1[2]));
}
