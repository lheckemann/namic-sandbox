#include "itkStochasticTractographyFilter.h"
#include "itkVectorImage.h"
#include "itkImageSeriesReader.h"  //this is needed for itk::ExposeMetaData()
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaDataDictionary.h"
#include "itkAddImageFilter.h"
#include <iostream>
#include <vector>
#include "itkImageRegionConstIterator.h"
#include "CalculateStatisticsCLP.h"
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
  typedef itk::VectorImage< unsigned short int, 3 > DWIVectorImageType;
  typedef itk::Image< unsigned int, 3 > CImageType;
  typedef itk::Image< float, 3 > NormalizedCImageType;

  //define reader and writer
  typedef itk::ImageFileReader< DWIVectorImageType > DWIVectorImageReaderType;
  typedef itk::ImageFileWriter< CImageType > CImageWriterType;
  typedef itk::ImageFileWriter< NormalizedCImageType > NormalizedCImageWriterType;
  
  //define AddImageFilterType to accumulate the connectivity maps of the pixels in the ROI
  typedef itk::AddImageFilter< CImageType, CImageType, CImageType> AddImageFilterType;
  
  //define a filter to normalize the connectivity map
  typedef itk::ShiftScaleImageFilter< CImageType, NormalizedCImageType >
    NormalizeCImageFilterType;

  //read in the DWI image
  DWIVectorImageReaderType::Pointer dwireaderPtr = DWIVectorImageReaderType::New();
  dwireaderPtr->SetFileName(dwifilename);
  dwireaderPtr->Update();
  
  //Load in the vtk tracts
  vtkXMLPolyDataReader* tractsreader = vtkXMLPolyDataReader::New();
  tractsreader->SetFileName( tractsfilename.c_str() );
  tractsreader->Update();
  vtkCellArray* loadedtracts = tractsreader->GetOutput()->GetLines();
  
  //Setup the AddImageFilter
  AddImageFilterType::Pointer addimagefilterPtr = AddImageFilterType::New();

  //Create a temporary Connectivity Image
  CImageType::Pointer tempcimagePtr = CImageType::New();
  tempcimagePtr->CopyInformation( dwireaderPtr->GetOutput() );
  tempcimagePtr->SetBufferedRegion( dwireaderPtr->GetOutput()->GetBufferedRegion() );
  tempcimagePtr->SetRequestedRegion( dwireaderPtr->GetOutput()->GetRequestedRegion() );
  tempcimagePtr->Allocate();
  tempcimagePtr->FillBuffer(0);
   
  //Create a zeroed accumulated Connectivity Image
  CImageType::Pointer accumulatedcimagePtr = CImageType::New();
  accumulatedcimagePtr->CopyInformation( dwireaderPtr->GetOutput() );
  accumulatedcimagePtr->SetBufferedRegion( dwireaderPtr->GetOutput()->GetBufferedRegion() );
  accumulatedcimagePtr->SetRequestedRegion( dwireaderPtr->GetOutput()->GetRequestedRegion() );
  accumulatedcimagePtr->Allocate();
  accumulatedcimagePtr->FillBuffer(0);
      
  //graft this onto the output of the addimagefilter
  addimagefilterPtr->GraftOutput( accumulatedcimagePtr );
  addimagefilterPtr->SetInput1( tempcimagePtr );
  addimagefilterPtr->SetInput2( addimagefilterPtr->GetOutput() );
   
  //write tracts to connectivity image
  CImageType::IndexType index;  //preallocate for efficiency
  std::cout<<"Writing Tracts to Image, Total Tracts: "<<loadedtracts->GetNumberOfCells()<<std::endl;
  
  vtkIdType npts;
  vtkIdType* pts;
  vtkPoints* points = tractsreader->GetOutput()->GetPoints();
  
  loadedtracts->InitTraversal();
  while( loadedtracts->GetNextCell( npts, pts ) ){
    for( int i=0; i<npts; i++ ){
      double* vertex = points->GetPoint( pts[i] );
                
      index[0]=static_cast<long int>(vertex[0]);
      index[1]=static_cast<long int>(vertex[1]);
      index[2]=static_cast<long int>(vertex[2]);
     
      CImageType::PixelType& tempcimagepix = tempcimagePtr->GetPixel( index );
      if(tempcimagepix == 0) tempcimagepix++;
    }
    addimagefilterPtr->Update();
    for( int i=0; i<npts; i++ ){
      double* vertex = points->GetPoint( pts[i] );
     
      index[0]=static_cast<long int>(vertex[0]);
      index[1]=static_cast<long int>(vertex[1]);
      index[2]=static_cast<long int>(vertex[2]);
     
      CImageType::PixelType& tempcimagepix = tempcimagePtr->GetPixel( index );
      tempcimagepix = 0;
    }
  }

  //Write the normalized connectivity map
  NormalizeCImageFilterType::Pointer ncifilterPtr = NormalizeCImageFilterType::New();
  ncifilterPtr->SetInput( accumulatedcimagePtr );
  ncifilterPtr->SetScale( 1.0f/static_cast< double >(loadedtracts->GetNumberOfCells()) );

  NormalizedCImageWriterType::Pointer nciwriterPtr = NormalizedCImageWriterType::New();
  nciwriterPtr->SetInput( ncifilterPtr->GetOutput() );
  nciwriterPtr->SetFileName( ncmapfilename.c_str() );
  nciwriterPtr->SetUseCompression( true );
  nciwriterPtr->Update();

  //clean up vtk stuff
  tractsreader->Delete();
  
  return EXIT_SUCCESS;
}
