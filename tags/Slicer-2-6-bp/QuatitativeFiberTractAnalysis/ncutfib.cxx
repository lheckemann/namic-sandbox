#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>

#include "itkImage.h"
#include "itkTubeSpatialObject.h"
#include "itkArray2D.h"
#include "itkImageRegionConstIteratorWithIndex.h"

#include "itkSpatialObjectReader.h"
#include "itkSpatialObjectWriter.h"
#include "itkSpatialObjectProperty.h"

#include "itkColorTable.h"

#include "itkAddImageFilter.h"
#include "itkNaryMaximumImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"

#include "itkSimpleDataObjectDecorator.h"


// My header files
#include "itkDistanceToAffinityFilter.h"
#include "itkTubeSpatialObjectToDistanceFilter.h"
#include "itkNormalizedCut.h"


int main(int argc, char* argv[])
{
  const int Dim = 2;
  typedef unsigned char PixelType;
  typedef double DistanceType;

  typedef itk::Image<PixelType,Dim> ImageType;
  typedef itk::Image<DistanceType,Dim> DistanceImageType;

  typedef itk::TubeSpatialObject<3> TubeType;

  typedef itk::SpatialObjectReader<3,double> ReaderType;
  typedef itk::GroupSpatialObject<3> GroupType;
  typedef GroupType::ChildrenListType ListOfChildrenType;

  if(argc != 5)
    {
    std::cout << "Usage: ncutfib <infile> <outfile> <# Clusters> <scale>" << std::endl;
    return 0;
    }

  const char* filename = argv[1];
  std::cout << "Reading: " << filename << std::endl;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename);
  reader->Update();

  typedef itk::TubeSpatialObjectToDistanceFilter<itk::SimpleDataObjectDecorator<ListOfChildrenType>,DistanceImageType> TubesToDistType;
  TubesToDistType::Pointer ttod = TubesToDistType::New();

  itk::SimpleDataObjectDecorator<ListOfChildrenType>::Pointer  childrenWrapper = itk::SimpleDataObjectDecorator<ListOfChildrenType>::New();
  childrenWrapper->Set(*reader->GetGroup()->GetChildren());

  ttod->SetInput(childrenWrapper);
  ttod->Update();

  // Find maximum of distance matrix to get scale
  typedef itk::MinimumMaximumImageCalculator<DistanceImageType> MinMaxCalcType;
  MinMaxCalcType::Pointer minMaxCalc = MinMaxCalcType::New();
  minMaxCalc->SetImage(ttod->GetOutput());
  minMaxCalc->ComputeMaximum();
  std::cout << "Max Dist: "  << minMaxCalc->GetMaximum() << std::endl;
  

  typedef itk::DistanceToAffinityFilter<DistanceImageType, DistanceImageType> DistanceToAffinityFilterType;

  DistanceToAffinityFilterType::Pointer dtoa = DistanceToAffinityFilterType::New();
  
  dtoa->SetInput(ttod->GetOutput());
  dtoa->SetOrder(2);
  dtoa->SetScale(atof(argv[4])*minMaxCalc->GetMaximum());

  std::cout << "Scale: " << atof(argv[4]) << std::endl;

  dtoa->Update();
  dtoa->UpdateLargestPossibleRegion();

  typedef itk::NormalizedCut<DistanceType> NormalizedCutType;

  NormalizedCutType::Pointer ncutCalculator = NormalizedCutType::New();

  DistanceImageType::SizeType sze = dtoa->GetOutput()->GetLargestPossibleRegion().GetSize();

  itk::Array2D<DistanceType> affinity(sze[0],sze[1]);

  // Iterate over the output image and fill up the affinity matrix
  typedef itk::ImageRegionConstIteratorWithIndex<DistanceImageType> IteratorType;
  IteratorType input(dtoa->GetOutput(), dtoa->GetOutput()->GetLargestPossibleRegion());

  for(input.GoToBegin(); !input.IsAtEnd(); ++input)
    {
    DistanceImageType::IndexType index = input.GetIndex();
    DistanceType d = input.Get();
    affinity(index[0],index[1]) = d;
    }

  int nclust = atoi(argv[3]);

  ncutCalculator->SetAffinityMatrix(affinity);
  ncutCalculator->SetNumberOfClusters(nclust);

  ncutCalculator->Evaluate();

  typedef itk::SpatialObjectWriter<3,double> ObjectWriter;
  ObjectWriter::Pointer objwriter = ObjectWriter::New();
  
  typedef itk::GroupSpatialObject<3> GroupObjectType;
  GroupObjectType::Pointer newgroup = GroupObjectType::New();

  typedef itk::ColorTable<float> ColorTableType;
  ColorTableType::Pointer ct = ColorTableType::New();
  ct->useDiscrete();

  std::vector<unsigned int>::const_iterator it;
  ListOfChildrenType::iterator fib_it = reader->GetGroup()->GetChildren()->begin();
  for(it = ncutCalculator->GetClasses().begin(); it != ncutCalculator->GetClasses().end(); ++it, ++fib_it)
    {
    // Make a new fiber and fill in the relevant details
    // Change the color to whatever is appropriate
    typedef itk::DTITubeSpatialObject<3> TubeType;
    TubeType::Pointer newTube = TubeType::New();
    TubeType* ref = static_cast<TubeType*>(fib_it->GetPointer());

    newTube->GetProperty()->SetRed(ct->GetColor(*it)->GetRed());
    newTube->GetProperty()->SetGreen(ct->GetColor(*it)->GetGreen());
    newTube->GetProperty()->SetBlue(ct->GetColor(*it)->GetBlue());
    newTube->GetProperty()->SetAlpha(1.0);

    newTube->SetPoints(ref->GetPoints());
    newgroup->AddSpatialObject(newTube);

    }

  

  // Get the new classes and modify the fibers appropriately
  objwriter->SetInput(newgroup);
  objwriter->SetFileName(argv[2]);
  objwriter->Update();


  return 0;
}
