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
#include "StochasticTractographyFilterCLP.h"
#include "itkTensorFractionalAnisotropyImageFilter.h"
#include "itkPathIterator.h"
#include <string>
#include "itkShiftScaleImageFilter.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h" 
#include "vtkZLibDataCompressor.h"

int main(int argc, char* argv[]){
  PARSE_ARGS;
  //define the input/output types
  typedef itk::Image< short int, 3 > ROIImageType;

  //define an iterator for the ROI image
  typedef itk::ImageRegionConstIterator< ROIImageType > ROIImageIteratorType;
  
  //define reader and writer
  typedef itk::ImageFileReader< ROIImageType > ROIImageReaderType;

  //setup the ROI image reader
  ROIImageReaderType::Pointer roireaderPtr = ROIImageReaderType::New();
  roireaderPtr->SetFileName(roifilename);
  roireaderPtr->Update();
  
  //Load in the vtk tracts
  vtkXMLPolyDataReader* tractsreader = vtkXMLPolyDataReader::New();
  tractsreader->SetFileName( inputtractsfilename.c_str() );
  tractsreader->Update();
  vtkCellArray* loadedtracts = tractsreader->GetOutput()->GetLines();
  
  vtkIdType npts;
  vtkIdType* pts;
  vtkPoints* points = tractsreader->GetOutput()->GetPoints();
  loadedtracts->InitTraversal();
  while( loadedtracts->GetNextCell( npts, pts ) ){
    bool goodtract=false;
    int currentpointID=0;
    for(int currentlabel=0; currentlabel<roilabels.size(); currentlabel++){
      while(currentpointID<npts){
        double* vertex = points->GetPoint( pts[currentpointID] );
        ROIImageType::PixelType& roiimagepix = roiimagePtr->GetPixel( index );
        if(roiiamgepix==roilabels[currentlabel]){
          goodtract=true;
          break;
        }
      } 
    }
    
    if(goodtract){
      //save in new container
    }
  }
  
  while( loadedtracts->GetNextCell( npts, pts ) ){
    for( int i=0; i<npts; i++ ){
      double* vertex = points->GetPoint( pts[i] );
                
      index[0]=static_cast<long int>(vertex[0]);
      index[1]=static_cast<long int>(vertex[1]);
      index[2]=static_cast<long int>(vertex[2]);
     
      ROIImageType::PixelType& roiimagepix = roiimagePtr->GetPixel( index );
      if(bimagepix == false){ 
        bimagepix=true;
        cimagepix++;
      }
    }
    //clear boolean image
    for( int i=0; i<npts; i++ ){
      double* vertex = points->GetPoint( pts[i] );
     
      index[0]=static_cast<long int>(vertex[0]);
      index[1]=static_cast<long int>(vertex[1]);
      index[2]=static_cast<long int>(vertex[2]);
     
      BooleanImageType::PixelType& bimagepix = bimagePtr->GetPixel( index );
      bimagepix = false;
    }
  }
  
  //allocate the VTK Polydata to output the tracts
  vtkPolyData* vtktracts = vtkPolyData::New();
  vtkPoints* points = vtkPoints::New();
  vtkCellArray* vtktractarray = vtkCellArray::New();
  
  for(int i=0; i<tractcontainer->Size(); i++ ){
    STFilterType::TractContainerType::Element tract =
      tractcontainer->GetElement(i);

    STFilterType::TractContainerType::Element::ObjectType::VertexListType::ConstPointer vertexlist = 
      tract->GetVertexList();
        
    //create a new cell
    vtktractarray->InsertNextCell(vertexlist->Size());

    for(int j=0; j<vertexlist->Size(); j++){
      STFilterType::TractContainerType::Element::ObjectType::VertexListType::Element vertex =
        vertexlist->GetElement(j);
          
      points->InsertNextPoint(vertex[0], vertex[1], vertex[2]);
      vtktractarray->InsertCellPoint(points->GetNumberOfPoints()-1);
    }
  }        
  
  //finish up the vtk polydata
  vtktracts->SetPoints( points );
  vtktracts->SetLines( vtktractarray );
  
  //output the vtk tract container
  vtkZLibDataCompressor* compressor = vtkZLibDataCompressor::New();
  vtkXMLPolyDataWriter* tractswriter = vtkXMLPolyDataWriter::New();
  tractswriter->SetCompressor( compressor );
  tractswriter->SetDataModeToBinary();
  tractswriter->SetInput( vtktracts );
  tractswriter->SetFileName( tractsfilename.c_str() );
  tractswriter->Write();
  
  //cleanup vtk stuff
  vtktracts->Delete();
  points->Delete();
  vtktractarray->Delete();
  tractswriter->Delete();
  compressor->Delete();

  return EXIT_SUCCESS;
}
