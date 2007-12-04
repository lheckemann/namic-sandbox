#include "itkVectorImage.h"
#include "itkImageSeriesReader.h"  //this is needed for itk::ExposeMetaData()
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaDataDictionary.h"
#include <iostream>
#include <vector>
#include "itkImageRegionConstIterator.h"
#include "CalculateTractAveragedFACLP.h"
#include "itkTensorFractionalAnisotropyImageFilter.h"
#include "itkPathIterator.h"
#include <string>
#include "itkShiftScaleImageFilter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h" 

int main(int argc, char* argv[]){
  PARSE_ARGS;
  //define the input/output types
  typedef itk::Image< float, 3 > FAImageType;

  //define reader and writer
  typedef itk::ImageFileReader< FAImageType > FAImageReaderType;
  
  //read in the DWI image
  FAImageReaderType::Pointer fareaderPtr = FAImageReaderType::New();
  fareaderPtr->SetFileName( faimagefilename.c_str() );
  fareaderPtr->Update();
  
  //Load in the vtk tracts
  vtkXMLPolyDataReader* tractsreader = vtkXMLPolyDataReader::New();
  tractsreader->SetFileName( tractsfilename.c_str() );
  tractsreader->Update();
  vtkCellArray* loadedtracts = tractsreader->GetOutput()->GetLines();
      
   
  //setup output stat files
  
  //open these files
  std::ofstream fafile( tractfafilename.c_str() );
  if(!fafile.is_open()){
    std::cerr<<"Could not open FA file!\n";
    return EXIT_FAILURE;
  }
  
  FAImageType::IndexType index;  //preallocate for efficiency
  std::cout<<"Writing Tracts to Image, Total Tracts: "<<loadedtracts->GetNumberOfCells()<<std::endl;
  
  vtkIdType npts;
  vtkIdType* pts;
  vtkPoints* points = tractsreader->GetOutput()->GetPoints();
  
  loadedtracts->InitTraversal();
  while( loadedtracts->GetNextCell( npts, pts ) ){
    FAImageType::PixelType accumFA=0;
    for( int i=0; i<npts; i++ ){

      double* vertex = points->GetPoint( pts[i] );
                
      index[0]=static_cast<long int>(vertex[0]);
      index[1]=static_cast<long int>(vertex[1]);
      index[2]=static_cast<long int>(vertex[2]);
     
      accumFA+=fareaderPtr->GetOutput()->GetPixel( index );
      //std::cout<<accumFA<<std::endl;
    }
    fafile<<(double)accumFA/(double)npts<<std::endl;
  }
  fafile.close();
  //clean up vtk stuff
  tractsreader->Delete();
  
  return EXIT_SUCCESS;
}
