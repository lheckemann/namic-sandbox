/*This is a simple example for reading in an Analyze object map and
then use vtk to display the object map.  The user will need ITK,
Insight Applications (for itkImageToVTKImageFilter), and VTK.
*/

#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "vtkRenderer.h"

#include "itkImageToVTKImageFilter.h"

#include "vtkRenderWindowInteractor.h"
#include <vtkImageViewer2.h>


#include <ImageToObjectmap.h>
#include "objectmap.h"



typedef unsigned char PixelType;
const unsigned int Dimension = 3;
typedef itk::Image< PixelType, Dimension > ImageType;
typedef itk::RGBPixel<PixelType> RGBPixelType;
typedef itk::Image< RGBPixelType, Dimension > RGBImageType;

typedef itk::ImageFileReader< ImageType > ReaderType;

typedef itk::ImageToVTKImageFilter<RGBImageType> ConnectorType;

int main(int argc, char * argv [] )
{
  if(argc != 3)
  {
    std::cerr << "USAGE: " << argv[0] << "<inputFileName>" << std::endl;
  }
  const char *DisplayImage = argv[1];
  const int SliceNumber = argv[2]

  vtkRenderWindowInteractor *windowInteractor = vtkRenderWindowInteractor::New();

  ReaderType::Pointer reader  = ReaderType::New();

  //The input should be an Anaylze Object Map file
  reader->SetFileName( DisplayImage );
  

  try
    {

    reader->Update();
  
    //This will convert the output of the reader into an object map
    itk::ImageToObjectMap<ImageType>::Pointer ConvertImage = itk::ImageToObjectMap<ImageType>::New();;
    itk::AnalyzeObjectMap::Pointer Objectmap = ConvertImage->TransformImage(reader->GetOutput());

    //Set the input, to the connector connecting itk with vtk, with an RGB image of the specific
    //colors that corresponds to each entry in the object map.
    ConnectorType::Pointer connector= ConnectorType::New();
    connector->SetInput( Objectmap->ObjectMapToRGBImage() );

    connector->Update();
     
   //Display a two dimensional view of the object map that was read in
   vtkImageViewer2 * twodimage = vtkImageViewer2::New();

   twodimage->SetInput(connector->GetOutput());
   twodimage->SetSlice(sliceNumber);
   twodimage->SetSliceOrientationToXY();

   //Set the background of the renderer to a grayish color so that it is easier to see
   //the outline of the object map since it is usually black
   twodimage->GetRenderer()->SetBackground(0.4392, 0.5020, 0.5647);


   twodimage->SetupInteractor(windowInteractor);

   twodimage->Render();
   windowInteractor->Start();

    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Exception catched !! " << e << std::endl;
    }
}
