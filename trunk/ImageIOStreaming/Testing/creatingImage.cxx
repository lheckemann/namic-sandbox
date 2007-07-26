#include <itkBoxSpatialObject.h>
#include <itkEllipseSpatialObject.h>

// Software Guide : BeginCodeSnippet
#include "itkSpatialObjectWriter.h"
#include "itkSpatialObjectReader.h"
// Software Guide : EndCodeSnippet

#include <itkImageRegionIterator.h>
#include <itkImageFileWriter.h>

typedef itk::Image<unsigned char, 2> UC2ImageType;
static void makeRectangle(UC2ImageType::Pointer image, const UC2ImageType::IndexType &center, const UC2ImageType::SizeType &size)
{
  for(int i = center[0]-size[0]/2; i< center[0] + size[0]/2; i++)
  {
    for(int j = center[1] - size[1]/2; j < center[1] + size[1]/2; j++)
    {
      const UC2ImageType::IndexType currentIndex = {i, j};
      image->SetPixel(currentIndex,200);
    }
  }
}
static bool IsInsideEllipse(const UC2ImageType::IndexType &currentLocation, const UC2ImageType::IndexType &center, const UC2ImageType::SizeType &size)
{
  const float x = static_cast<float>(currentLocation[0] - center[0]) / static_cast<float>(size[0]);
  const float y = static_cast<float>(currentLocation[1] - center[1]) / static_cast<float>(size[1]);
  if((x*x + y*y)<1)
  {
    return true;
  }
  else
  {
    return false;
  }
}
static void makeEllipse(UC2ImageType::Pointer image, const UC2ImageType::IndexType &center, const UC2ImageType::SizeType &size)
{
  for(int i = center[0]-size[0]/2; i< center[0] + size[0]/2; i++)
  {
    for(int j = center[1] - size[1]/2; j < center[1] + size[1]/2; j++)
    {
      const UC2ImageType::IndexType currentIndex = {i, j};
      if(IsInsideEllipse(currentIndex, center, size))
      {

        image->SetPixel(currentIndex,200);
      }

    }
  }
}


int main( int argc, char ** argv )
{

  if ( argc != 3 )
    {
    std::cerr << "USAGE: " << argv[0] << "<OutputNiftiFileName> <OutputAnalzyeObjectLabelMapFileName>" << std::endl;
    }
  const std::string OutputNiftiFileName(argv[1]);
  const std::string OutputAnalyzeObjectLabelMapFileName(argv[2]);
    std::cout << "" << argv[0] << " " << OutputNiftiFileName << " " << OutputAnalyzeObjectLabelMapFileName << std::endl;
// Software Guide : BeginLatex
//
// Next, we create a SpatialObjectWriter that is templated over the dimension
// of the object(s) we want to write.
//
// Software Guide : EndLatex


  UC2ImageType::Pointer image = UC2ImageType::New();
  const UC2ImageType::SizeType size = {{20,20}};
  const UC2ImageType::IndexType orgin = {{0,0}};

  UC2ImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(orgin);
  image->SetRegions(region);
  image->Allocate();

  //const UC2ImageType::SizeType Square = {{4,4}};
  //const UC2ImageType::IndexType SquareOrgin = {3,15};
  //makeRectangle(image, SquareOrgin, Square );

  //const UC2ImageType::SizeType Circle = {{4,4}};
  //const UC2ImageType::IndexType CircleOrgin = {15,14};
  //makeEllipse(image, CircleOrgin, Circle);




  typedef itk::ImageFileWriter<UC2ImageType> Writer;

  Writer::Pointer writer = Writer::New();

  writer->SetInput(image);
  writer->SetFileName(OutputNiftiFileName);
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
