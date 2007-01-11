//we should only get the images once, casting may be expensive
  
  //create an iterator to iterate over the input images
    typedef itk::ImageRegionConstIterator< TDWIImage > DWIImageIteratorType;
    
  //Create an array of iterators to the input images
  typename VectorContainer< unsigned int, DWIImageIteratorType >::Pointer  inputitarray = 
    VectorContainer< unsigned int, DWIImageIteratorType>::New();

  //create a pointer to the input images
  typename TDWIImage::ConstPointer inputPtr;
  
  //fill in the iterator array
  for(int j=0; j<this->GetNumberOfInputs(); j++){
    inputPtr = dynamic_cast<const TDWIImage *>(
      (ProcessObject::GetInput(j)));
    inputitarray->InsertElement( j, 
      DWIImageIteratorType( inputPtr, inputPtr->GetBufferedRegion())); 
  }
  std::cout <<"Sizejunk: " <<  inputitarray->Size()<<std::endl;
  //the input is of variable length so we should put it into a VectorImage
  typedef VectorImage< typename TDWIImage::PixelType , 3 > DWIVectorImageType;
  typename DWIVectorImageType::Pointer dwi = DWIVectorImageType::New();
    
  //set the information for the VectorImage using the last input image
  dwi->CopyInformation( inputPtr );
  dwi->SetBufferedRegion( inputPtr->GetBufferedRegion() );
   dwi->SetRequestedRegion( inputPtr->GetRequestedRegion() );
   dwi->Allocate();
   
  //create an iterator to iterate over the DWI Vector image
   typedef itk::ImageRegionIterator< DWIVectorImageType  >   DWIVectorImageIteratorType;
    DWIVectorImageIteratorType dwivectorIt( dwi, dwi->GetBufferedRegion() ); 
    
    dwivectorIt.GoToBegin();
    //loop through telling all iterators to go to the beginning
        //create a variable length vector
    typename DWIVectorImageType::PixelType vectpix(this->GetNumberOfInputs());
    
      //vectpix.AllocateElements(3);
    //DEBUG: need to allocate elements of vectpix first, it is an itkVariableLengthVector
  while(!dwivectorIt.IsAtEnd()){

    //fill the pixel with values from the inputs
    for(int j=0; j<this->GetNumberOfInputs(); j++){
      vectpix.SetElement(j, (inputitarray->GetElement(j)).Get());
      ++(inputitarray->ElementAt(j));
    }
    
    dwivectorIt.Set(vectpix);
    ++dwivectorIt;
  }
