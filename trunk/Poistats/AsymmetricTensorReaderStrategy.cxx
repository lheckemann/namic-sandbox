#include <itkImageSeriesReader.h>

#include "AsymmetricTensorReaderStrategy.h"

AsymmetricTensorReaderStrategy::AsymmetricTensorReaderStrategy(){
  
}
  
AsymmetricTensorReaderStrategy::~AsymmetricTensorReaderStrategy(){
  
}

AsymmetricTensorReaderStrategy::TensorImageType::Pointer
AsymmetricTensorReaderStrategy::GetTensors(){
  m_Observer->PostMessage( "not stored symmetrically\n" );
  
  typedef itk::Image< float, 4 > FullTensorImageType;
  typedef itk::ImageFileReader< FullTensorImageType > FullTensorReaderType;
  FullTensorReaderType::Pointer tensorReader = FullTensorReaderType::New();
      
  tensorReader->SetFileName( m_TensorFileName );
  m_Observer->PostMessage( "reading tensors...\n" );
  try { 
    tensorReader->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::ostringstream output;
    output << "Error reading the series." << std::endl << excp << std::endl;
    m_Observer->PostErrorMessage( output.str() );
  }
  
  FullTensorImageType::Pointer fullTensors = tensorReader->GetOutput();
      
  // TODO:
  std::cerr << fullTensors << std::endl;
  std::cerr << "\ndirection: \n" << fullTensors->GetDirection();
  
  // convert the full 9 component tensors to 6 component tensors
  //  - create and allocate a new DiffusionTensor3D image
  FullTensorImageType::RegionType dtiRegion = 
    fullTensors->GetLargestPossibleRegion();
  TensorImageType::SizeType size;
  double origin[ TensorImageType::RegionType::GetImageDimension() ];
  TensorImageType::IndexType start;
  TensorImageType::SpacingType spacing;

  for( int cDim=0; cDim<TensorImageType::RegionType::GetImageDimension(); cDim++ ) {    
    size[ cDim ] = dtiRegion.GetSize()[ cDim ];
    origin[ cDim ] = fullTensors->GetOrigin()[ cDim ];
    start[ cDim ] = 0;
    spacing[ cDim ] = fullTensors->GetSpacing()[ cDim ];
  }
      
  TensorImageType::RegionType region;
  region.SetSize( size );    
  region.SetIndex( start );
  
  TensorImageType::Pointer tensors = TensorImageType::New();
  tensors->SetRegions( region );
  tensors->SetOrigin( origin );
  tensors->SetSpacing( spacing );      
  tensors->Allocate();  
  tensors->FillBuffer( 0.0 );

  //  - copy the data into the right areas
  const int nTensorRows = 3;
  const int nTensorCols = 3;

  m_Observer->PostMessage( "converting tensors to symmetric format\n" );
  for( int cImageRow=0; cImageRow<size[ 0 ]; cImageRow++ ) {
    for( int cImageCol=0; cImageCol<size[ 1 ]; cImageCol++ ) {
      for( int cImageSlice=0; cImageSlice<size[ 2 ]; cImageSlice++ ) {
      
        TensorImageType::IndexType symmetricIndex;
        symmetricIndex[ 0 ] = cImageRow;
        symmetricIndex[ 1 ] = cImageCol;
        symmetricIndex[ 2 ] = cImageSlice;

        TensorPixelType symmetricTensor;

        FullTensorImageType::IndexType fullTensorIndex;
        fullTensorIndex[ 0 ] = cImageRow;
        fullTensorIndex[ 1 ] = cImageCol;
        fullTensorIndex[ 2 ] = cImageSlice;
        
        FullTensorImageType::PixelType pixel = 
          fullTensors->GetPixel( fullTensorIndex );
      
        int cContinuousIndex = 0;
        
        // iterate through each component
        // TODO: actually we'll only need to iterate through some of them
        for( int cTensorRow = 0; cTensorRow<nTensorRows; cTensorRow++ ) {
          
          for( int cTensorCol = 0; cTensorCol<nTensorCols; cTensorCol++ ) {            
            fullTensorIndex[ 3 ] = cContinuousIndex;
            
            symmetricTensor( cTensorRow, cTensorCol ) = 
              fullTensors->GetPixel( fullTensorIndex );
          }
          
        }
        
        tensors->SetPixel( symmetricIndex, symmetricTensor );

      }
    }
  }
  
  return tensors;
}
