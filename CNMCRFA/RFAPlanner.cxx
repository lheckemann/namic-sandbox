#include <iostream>
#include <sstream>
#include <string>
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"

#include "RFAPlannerCLP.h"

//definition of the segmentation volume type and its reader and writer
typedef unsigned short LabelType;
typedef itk::Image<LabelType ,3> LabeledVolumeType;
typedef itk::ImageFileReader<LabeledVolumeType> LabeledVolumeReaderType;
typedef itk::ImageFileWriter<LabeledVolumeType> LabeledVolumeWriterType;

/**
 * Input: 
 *   3D labeled (unsigned short) volume with the following labels:
 *     background:      0
 *     tumor:           1
 *     entry regions:   2
 *     no pass regions: 3
 *     tumor margin:    4
 *
 * Output: 
 *   ASCII file for optimization software.
 *   New segmentation file with added ablation margin surrounding tumor.
 */
LabelType BACKGROUND = 0;
LabelType TUMOR = 1;
LabelType ENTRY_REGION = 2;
LabelType NO_PASS_REGION = 3;
LabelType TUMOR_MARGIN = 4;



void GetProbeDiameters(double *probeDiameters);
void GetGridSpacing(double *gridSpacing);
template<class T>
void GetNumericValue(const std::string &prompt, T *value);
void GetSegmentationFileName(std::string &fileName);
void GetOutputFileName(std::string &tpsFileName);


LabeledVolumeType::Pointer AddAblationMarginAndResample(double ablationMargin,
                                                        double noPassMargin,
                                                        double gridSpacing[3],
                                                        const std::string &fileName);
LabeledVolumeType::Pointer dilateLabeledVolume(LabeledVolumeType::Pointer inputVolume,
                                               LabeledVolumeType::PixelType dilateValue,
                                               double structuringElementRadius);

int main(int argc, char * argv[])
{
  PARSE_ARGS;

              //default values
  const double DEFAULT_PROBE_DIAMETER = 30.0; //[mm]
  const double DEFAULT_GRID_SPACING = 5.0;  //[mm]
  const double DEFAULT_ANGULAR_RESOLUTION = 0.1; //[radians] 
  const double DEFAULT_MAX_ANGLE = 0.732; //[radians]
  const unsigned int DEFAULT_MAX_ABLATIONS = 8;
  const unsigned int DEFAULT_MAX_RAYS = 4;
  const unsigned int DEFAULT_MAX_PUNCTURES = 8;
  const double DEFAULT_ABLATION_MARGIN = 10.0; //[mm]
  const double DEFAULT_NO_PASS_MARGIN = 5.0; //[mm]
  const std::string DEFAULT_INPUT_FILE_NAME = "segmentation.mhd";
  const std::string DEFAULT_OUTPUT_FILE_NAME = "TPS.txt";

            //initialize all variables using default values

  double maxAngle = DEFAULT_MAX_ANGLE;
  //std::string inputFileName = DEFAULT_INPUT_FILE_NAME;
  //std::string tpsFileName = DEFAULT_OUTPUT_FILE_NAME;

  std::string line;
  std::istringstream strStream;

                    //get input from user
/*  GetProbeDiameters(probeDiameters);
  GetGridSpacing(gridSpacing);
  GetNumericValue<double>(std::string("Please enter angular resolution (radians)"), 
                          &angularResolution);
  GetNumericValue<double>(std::string("Please enter max angle (radians)"), 
                          &maxAngle);
  GetNumericValue<unsigned int>(std::string("Please enter max number of ablations"), 
                                &maxAblations);
  GetNumericValue<unsigned int>(std::string("Please enter max number of rays"), 
                                &maxRays);
  GetNumericValue<unsigned int>(std::string("Please enter max number of punctures"), 
                                &maxPunctures);
  GetNumericValue<double>(std::string("Please enter ablation margin (mm)"), 
                          &ablationMargin);
  GetNumericValue<double>(std::string("Please enter no pass margin (mm)"), 
                          &noPassMargin);
  GetSegmentationFileName(inputFileName);
  GetOutputFileName(tpsFileName);
  */

  double gridSpacingC[3];
  gridSpacingC[0] = gridSpacing[0];
  gridSpacingC[1] = gridSpacing[1];
  gridSpacingC[2] = gridSpacing[2];//DEFAULT_GRID_SPACING;

  LabeledVolumeType::Pointer result = AddAblationMarginAndResample(ablationMargin,
                                                                   noPassMargin,
                                                                   gridSpacingC,  //gridSpacing
                                                                   inputFileName);
           //write the input for the optimization
  std::ofstream file(tpsFileName.c_str());

  file << "! Part 1: Scalar information" << std::endl;
  file << probeDiameters[0] << ',' << probeDiameters[1] << "\t! Ablation diameters [mm]" << std::endl;
  file << gridSpacing[0] << ',' << gridSpacing[1] << ',' << gridSpacing[2] << "\t! Grid spacing [mm]" << std::endl;
  file << angularResolution << "\t! Angular resolution" << std::endl;
  file << maxAngle << "\t! Maximum angle between directions" << std::endl;
  file << maxAblations << "\t! Maximum no. of ablations" << std::endl;
  file << maxRays << "\t! Maximum no. of directions" << std::endl;
  file << maxPunctures << "\t! Maximum no. of punctures" << std::endl;
  file << "! Part 2: Voxel information" << std::endl;

  typedef itk::ImageRegionConstIterator< LabeledVolumeType > ConstIteratorType;
  ConstIteratorType constIterator(result, 
                                  result->GetLargestPossibleRegion());
  LabeledVolumeType::IndexType index;
  LabeledVolumeType::PointType point;

  //Point< TCoordRep, VImageDimension > &point
  for (constIterator.GoToBegin(); !constIterator.IsAtEnd(); ++constIterator) 
  {
    if(constIterator.Get() != BACKGROUND)      
    {
      index = constIterator.GetIndex();
      result->TransformIndexToPhysicalPoint(index, point);
      file << point[0] << ',' << point[1] << ',' << point[2] << ',' << constIterator.Get() << std::endl;
    }
  }



}

void GetProbeDiameters(double *probeDiameters)
{
  std::string line;
  std::istringstream strStream;

  std::cout<<"Please input probe diameters, lognitudinal and axial (mm) [";
  std::cout<<probeDiameters[0]<<" "<<probeDiameters[1]<<"]: ";
  std::getline(std::cin,line);
  if(!line.empty())
  {
    strStream.str(line);
    strStream>>probeDiameters[0]>>probeDiameters[1];
  }
  if (strStream.fail()) 
  {
    throw ("Input includes non-numeric values.");
  }
}


void GetGridSpacing(double *gridSpacing)
{
  std::string line;
  std::istringstream strStream;

  std::cout<<"Please enter sampling grid spacing (mm) [";
  std::cout<<gridSpacing[0]<<" "<<gridSpacing[1]<<" "<<gridSpacing[2]<<"]: ";
  std::getline(std::cin,line);
  if(!line.empty())
  {
    strStream.str(line);
    strStream>>gridSpacing[0]>>gridSpacing[1]>>gridSpacing[2];
  }
  if (strStream.fail()) 
  {
    throw ("Input includes non-numeric values.");
  }
}

template<class T>
void GetNumericValue(const std::string &prompt, T *value)
{
  std::string line;
  std::istringstream strStream;

  std::cout<<prompt<<" ["<<*value<<"]: ";
  std::getline(std::cin,line);
  if(!line.empty())
  {
    strStream.str(line);
    strStream>>(*value);
  }
  if (strStream.fail()) 
  {
    throw ("Input includes non-numeric values.");
  }
}


void GetSegmentationFileName(std::string &fileName)
{
  std::string line;
  std::istringstream strStream;

  std::cout<<"Please enter labeled volume file name"<<" ["<<fileName<<"]: ";
  std::getline(std::cin,line);
  if(!line.empty())
    fileName = line;
}


void GetOutputFileName(std::string &tpsFileName)
{
  std::string line;
  std::istringstream strStream;

  std::cout<<"Please enter output file name"<<" ["<<tpsFileName<<"]: ";
  std::getline(std::cin,line);
  if(!line.empty())
    tpsFileName = line;
}


LabeledVolumeType::Pointer AddAblationMarginAndResample(double ablationMargin,
                                                        double noPassMargin,
                                                        double gridSpacing[3],
                                                        const std::string &fileName)
{
  
  LabeledVolumeReaderType::Pointer reader = LabeledVolumeReaderType::New();
  reader->SetFileName(fileName);
  reader->Update();

  LabeledVolumeType::Pointer labeledVolume = reader->GetOutput();

  LabeledVolumeType::Pointer dilatedTumorVolume = dilateLabeledVolume(labeledVolume,
                                                                      TUMOR,
                                                                      ablationMargin);

  LabeledVolumeType::Pointer dilatedNoPassVolume = dilateLabeledVolume(labeledVolume,
                                                                       NO_PASS_REGION,
                                                                       noPassMargin);

       //go over the dilated tumor and no pass volumes update the original data so that we don't 
       //overwrite other labels except background and keep a margin from the no pass zones
  typedef itk::ImageRegionConstIterator< LabeledVolumeType > ConstIteratorType;
  typedef itk::ImageRegionIterator< LabeledVolumeType > IteratorType;
  ConstIteratorType constIteratorTumor(dilatedTumorVolume, 
                                       dilatedTumorVolume->GetLargestPossibleRegion());
  ConstIteratorType constIteratorNoPass(dilatedNoPassVolume, 
                                        dilatedNoPassVolume->GetLargestPossibleRegion());
  IteratorType iterator(labeledVolume, 
                        labeledVolume->GetLargestPossibleRegion());  
  
  for (constIteratorNoPass.GoToBegin(), constIteratorTumor.GoToBegin(), iterator.GoToBegin(); 
       !constIteratorTumor.IsAtEnd(); 
        ++constIteratorNoPass, ++constIteratorTumor, ++iterator) 
  {
    if(constIteratorTumor.Get() == TUMOR && 
       constIteratorNoPass.Get()!=NO_PASS_REGION && 
       iterator.Get() == BACKGROUND)      
      iterator.Set(TUMOR_MARGIN);
  }
           //write the results of "corrected" dilation so we can view them
  LabeledVolumeWriterType::Pointer writer = LabeledVolumeWriterType::New();
  writer->SetFileName("afterDilation.mhd");
  writer->SetInput(labeledVolume);
  writer->Update();

            //resample according to the requested sampling. we use nearest neighbor because
            //we are interpolating a discrete set of values
  itk::ResampleImageFilter<LabeledVolumeType,LabeledVolumeType>::Pointer resampleFilter =
    itk::ResampleImageFilter<LabeledVolumeType,LabeledVolumeType>::New();

  itk::NearestNeighborInterpolateImageFunction<LabeledVolumeType, double >::Pointer nnInterpolator =
    itk::NearestNeighborInterpolateImageFunction<LabeledVolumeType, double >::New();

  LabeledVolumeType::SizeType resampledSize, originalSize;
  originalSize = labeledVolume->GetLargestPossibleRegion().GetSize();
  LabeledVolumeType::SpacingType originalSpacing = labeledVolume->GetSpacing();

  resampledSize[0] = static_cast<unsigned int> ((originalSpacing[0]/gridSpacing[0])*originalSize[0]);
  resampledSize[1] = static_cast<unsigned int> ((originalSpacing[1]/gridSpacing[1])*originalSize[1]);
  resampledSize[2] = static_cast<unsigned int> ((originalSpacing[2]/gridSpacing[2])*originalSize[2]);

  resampleFilter->SetOutputSpacing(gridSpacing);
  resampleFilter->SetSize(resampledSize);
  resampleFilter->SetOutputOrigin(labeledVolume->GetOrigin());
  resampleFilter->SetOutputDirection(labeledVolume->GetDirection());
  resampleFilter->SetInterpolator(nnInterpolator);
  resampleFilter->SetInput(labeledVolume);
  
  resampleFilter->Update();
  return resampleFilter->GetOutput();
}


LabeledVolumeType::Pointer dilateLabeledVolume(LabeledVolumeType::Pointer inputVolume,
                                               LabeledVolumeType::PixelType dilateValue,
                                               double structuringElementRadius)
{
  typedef itk::BinaryBallStructuringElement<LabelType, 3> KernelType;
  typedef itk::BinaryDilateImageFilter<LabeledVolumeType, LabeledVolumeType, KernelType> DilationFilterType;

  DilationFilterType::Pointer dilationFilter = DilationFilterType::New();

  KernelType structuringElement;
  KernelType::SizeType elementSize;
  LabeledVolumeType::SpacingType imageSpacing = inputVolume->GetSpacing();
  elementSize[0] = (unsigned int)(structuringElementRadius/(double)imageSpacing[0] + 0.5);
  elementSize[1] = (unsigned int)(structuringElementRadius/(double)imageSpacing[1] + 0.5);
  elementSize[2] = (unsigned int)(structuringElementRadius/(double)imageSpacing[2] + 0.5);
  structuringElement.SetRadius(elementSize);
  structuringElement.CreateStructuringElement();

  dilationFilter->SetInput(inputVolume);
  dilationFilter->SetKernel(structuringElement);
                  //indicate the forground value, all other values are background
                  //this enables the use of a binary dilation filter on a labeled image
  dilationFilter->SetDilateValue(dilateValue);
  dilationFilter->Update();
  return dilationFilter->GetOutput();
}

