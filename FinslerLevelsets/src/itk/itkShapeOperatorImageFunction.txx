/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGaussianDerivativeImageFunction.txx,v $
  Language:  C++
  Date:      $Date: 2006/02/16 14:17:42 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkShapeOperatorImageFunction_txx
#define __itkShapeOperatorImageFunction_txx

#include "itkShapeOperatorImageFunction.h"
#include "itkDerivativeOperator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhood.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"

namespace itk
{

/** Set the Input Image */
template <class TInputImage, class TOutput>
ShapeOperatorImageFunction<TInputImage,TOutput>
::ShapeOperatorImageFunction()
{
  /*
  typename GaussianFunctionType::ArrayType mean;
  mean[0]=0.0;
  for(unsigned int i=0;i<itkGetStaticConstMacro(ImageDimension2);i++)
    {
    m_Sigma[i] = 1.0;
    m_Extent[i] = 1.0;
    }
  m_UseImageSpacing = true;
  m_GaussianDerivativeFunction = GaussianDerivativeFunctionType::New();
  m_GaussianFunction = GaussianFunctionType::New();
  m_OperatorImageFunction = OperatorImageFunctionType::New();
  m_GaussianFunction->SetMean(mean);
  m_GaussianFunction->SetNormalized(false); // faster
  m_GaussianDerivativeFunction->SetNormalized(false); // faster
  this->RecomputeGaussianKernel();
  */
  m_OperatorImageFunction = OperatorImageFunctionType::New();
}

/** Print self method */
template <class TInputImage, class TOutput>
void
ShapeOperatorImageFunction<TInputImage,TOutput>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
  /*
  os << indent << "UseImageSpacing: " << m_UseImageSpacing << std::endl;

  os << indent << "Sigma: " << m_Sigma << std::endl;
     */
}

/** Set the input image */
template <class TInputImage, class TOutput>
void
ShapeOperatorImageFunction<TInputImage,TOutput>
::SetInputImage( const InputImageType * ptr )
{
  Superclass::SetInputImage(ptr);
  m_OperatorImageFunction->SetInputImage(ptr);
}

/** Evaluate the function at the specifed index */
template <class TInputImage, class TOutput>
typename ShapeOperatorImageFunction<TInputImage,TOutput>::OutputType
ShapeOperatorImageFunction<TInputImage, TOutput>
::EvaluateAtIndex(const IndexType& index) const
{
  OutputType shapeOperator;
  IndexType startingIndexForNeighborhood = index;
  // add an offset of -2 to each dim to get starting index from center index
  for(int itmp=0;itmp<itkGetStaticConstMacro(ImageDimension2);itmp++)
  {
    startingIndexForNeighborhood[itmp] = index[itmp] - 2;
  }
  // Neighborhood iterator. Send this to the index passed in
  unsigned int radius = 2; // because we later need 2nd order derivatives
  itk::ConstNeighborhoodIterator<InputImageType>::SizeType size;
  itk::ConstNeighborhoodIterator<InputImageType>::OffsetType offset;
  int i1,i2,i3;
  for (i1=0;i1<itkGetStaticConstMacro(ImageDimension2);i1++)
  {
    size[i1] = radius;
    offset[i1] = 0;
  }

  itk::ConstNeighborhoodIterator< InputImageType>  //, itk::ZeroFluxNeumannBoundaryCondition<InputImageType> > 
     iterator(size, (ShapeOperatorImageFunction<TInputImage, TOutput>::InputImageType *) m_OperatorImageFunction->GetInputImage(), m_OperatorImageFunction->GetInputImage()->GetRequestedRegion());
  //  iterator(size, this->GetInputImage(), this->GetInputImage()->GetRequestedRegion());
  
  
  // Set boundary condition for the iterator
  //itk::ZeroFluxNeumannBoundaryCondition<TInputImage> bcForNeighborhoodIter; //( this->GetInputImage() );
  //iterator.SetBoundaryCondition(bcForNeighborhoodIter);
  iterator.SetLocation(startingIndexForNeighborhood); //  iterator.SetLocation(index);
  
  VectorType gradientVector; // = VectorType::New();
    
  // build a vector image which holds the gradient vectors for all pixels in the neighborhood
  // then pick out scalar images (w.r.t each dimension) and run deriv op on them to get the rows of the hessian 
  // at each pixel location
  // ConstNeighborhoodIterator<InputImageType>::NeighborhoodType neighborhoodsHoldingFirstOrdDiffs[itkGetStaticConstMacro(ImageDimension2)];

   unsigned int sizeofNeighborhood = iterator.Size(); // this is the total number of pixels in the neighborhood
   ConstNeighborhoodIterator<InputImageType>::NeighborhoodType neighborhood = iterator.GetNeighborhood();
   iterator.SetLocation(index);
   for (i1=0;i1<itkGetStaticConstMacro(ImageDimension2);i1++)
   {
       NeighborhoodIterator<InputImageType>::PixelType rightNeighbor = iterator.GetNext(i1);
       NeighborhoodIterator<InputImageType>::PixelType leftNeighbor = iterator.GetPrevious(i1);
       gradientVector[i1] = (double(rightNeighbor) - leftNeighbor)/2; // central difference
       //(iteratorArray[i1]).GoToBegin(); // setting all the iterators to beginnings of corresponding neighborhoods
       // std::cout << (0.0 + rightNeighbor) << " " << (0.0 + leftNeighbor) << " " << gradientVector[i1] << "\n";
   }
      
   // Create image to hold first derivatives
   typedef itk::Image<double,(itkGetStaticConstMacro(ImageDimension2)+1)> FirstDiffStorageImageType;
   FirstDiffStorageImageType::Pointer tmpImage = FirstDiffStorageImageType::New();
   std::cout << "Im ptr initialized \n";
   // Set image start and size
   FirstDiffStorageImageType::IndexType regionStart;
   FirstDiffStorageImageType::SizeType regionSize;
   for (i1=0; i1<(itkGetStaticConstMacro(ImageDimension2) + 1) ;i1++)
   {
    regionSize[i1] = 2*2+1; // this is fixed for all dimensions except the last one (extra one) which will be the number of dimensions...
    regionStart[i1] = 0.0;
   }
   regionSize[itkGetStaticConstMacro(ImageDimension2)] = itkGetStaticConstMacro(ImageDimension2);
   FirstDiffStorageImageType::RegionType region;
   region.SetSize(regionSize);
   region.SetIndex(regionStart);
   tmpImage->SetRegions(region);
   tmpImage->Allocate();
   std::cout << " tmpImage allocated \n";
   // COMPUTING FIRST ORDER DIFFERENCES
   // For all of the 5 by 5... by 5 neighborhood of the current index passed in, 
   // at each point, find the gradient and store it in tmpImage
   //iterator.SetLocation(index);
   //iterator.NeedToUseBoundaryConditionOn();
   //iterator.GoToBegin();// reinit the iterator to beginning of the neighborhood
   FirstDiffStorageImageType::IndexType imgIndex;
   i1 = 0; 
   VectorType gradientTmp;
   //while(!iterator.IsAtEnd())
   iterator.SetLocation(startingIndexForNeighborhood); 
   IndexType indexForImageNeighIter = index;
   for(i1=0; (i1<sizeofNeighborhood) ;i1++)
   {
       itk::ConstNeighborhoodIterator<InputImageType>::OffsetType offsetForNeighborFromCenter = neighborhood.GetOffset(i1);
       for(i2=0;i2<itkGetStaticConstMacro(ImageDimension2);i2++)
       {
      imgIndex[i2] = offsetForNeighborFromCenter[i2]   +  size[i2]; // adding 'size' makes index non-negative as needed
      indexForImageNeighIter[i2] = index[i2] + offsetForNeighborFromCenter[i2];
       } //'offset' is the position/index within the neighborhood if added to the extent/radius
       // imgIndex[itkGetStaticConstMacro(ImageDimension2)] needs to be modified for every component of the gradient
       iterator.SetLocation(indexForImageNeighIter);
       for (i2=0;i2<itkGetStaticConstMacro(ImageDimension2);i2++)
       {
           NeighborhoodIterator<InputImageType>::PixelType rightNeighbor = iterator.GetNext(i2);
           NeighborhoodIterator<InputImageType>::PixelType leftNeighbor = iterator.GetPrevious(i2);
           gradientTmp[i2] = (rightNeighbor - leftNeighbor)/2; // central difference
           imgIndex[itkGetStaticConstMacro(ImageDimension2)] = i2;
       // set value at this index in tmpImage as this gradient component
       //std::cout << "\n offset for " << i1 << " = " << offsetForNeighborFromCenter << " and val = "  << iterator.GetCenterPixel();
       std::cout << "\n      Posi = " << imgIndex << ", curr = " << iterator.GetCenterPixel() << "  RN = " << rightNeighbor << ", LN = " << leftNeighbor<< " Val = " << gradientTmp[i2] ;       
       tmpImage->SetPixel(imgIndex, gradientTmp[i2]);
       }
       //++iterator; // 'iterator' incremented to point to next element in neighborhood
       //i1++;
   }
   
   // Reset iterator to center pixel and compute the needed second order derivatives
   VectorType tmpStoreForSecondDerivsAtCenterPixel;
   //IndexType neighbordhoodCenterIndex = iterator.GetIndex(12); // the neighborhood is always 5-by-5, and hence in a fixed array format, the center pixel is the index 12 element (13th by count)
   iterator.SetLocation(index);
   NeighborhoodIterator<InputImageType>::PixelType centerPixel = iterator.GetCenterPixel();
   for (i2=0;i2<itkGetStaticConstMacro(ImageDimension2);i2++)
   {
       NeighborhoodIterator<InputImageType>::PixelType rightNeighbor = iterator.GetNext(i2);
       NeighborhoodIterator<InputImageType>::PixelType leftNeighbor = iterator.GetPrevious(i2);
       tmpStoreForSecondDerivsAtCenterPixel[i2] = (rightNeighbor + leftNeighbor - 2*centerPixel);
   }

   std::cout << " \n tmpImage populated with first order differences \n";
   
   TensorType hessianMatrix;
   // Create a neighborhood iterator for the tmpImage
   // centered on the center pixel (which will be index [2,2....2])
   itk::ConstNeighborhoodIterator<FirstDiffStorageImageType>::SizeType size_iterOnFirstDiffImg;
   size_iterOnFirstDiffImg.Fill(1); // a radius of '1' is sufficient
   itk::ConstNeighborhoodIterator<FirstDiffStorageImageType> iterOnFirstDiffImg(size_iterOnFirstDiffImg, tmpImage, tmpImage->GetRequestedRegion());
   itk::ConstNeighborhoodIterator<FirstDiffStorageImageType>::IndexType centerOfTmpImage;
   centerOfTmpImage.Fill(2); // 
   //iterOnFirstDiffImg.SetLocation(centerOfTmpImage);
   
   //VectorType gradientTmp;
   std::cout << "\n Hessian from inside loop =   "; 
   for (i2=0;i2<itkGetStaticConstMacro(ImageDimension2);i2++)
   {
       centerOfTmpImage[itkGetStaticConstMacro(ImageDimension2)] = i2; // centering iterator on each hyperplane within tmpImage
       iterOnFirstDiffImg.SetLocation(centerOfTmpImage);   
       for (i3=0;i3<itkGetStaticConstMacro(ImageDimension2);i3++)
       {
          if (i2==i3)
          {
              // double derivatives along a dimension... 
              // computed ahead and stored in a tmp array
              hessianMatrix(i2,i2) = tmpStoreForSecondDerivsAtCenterPixel[i2];
          }
          else
          { // mixed derivatives
              itk::ConstNeighborhoodIterator<FirstDiffStorageImageType>::PixelType rightNeighbor = iterOnFirstDiffImg.GetNext(i3);
              itk::ConstNeighborhoodIterator<FirstDiffStorageImageType>::PixelType leftNeighbor = iterOnFirstDiffImg.GetPrevious(i3);
              gradientTmp[i3] = (rightNeighbor - leftNeighbor)/2; // central difference
              //if (i2<=i3)
              //{
        hessianMatrix(i2,i3) = gradientTmp[i3];
        //}
        //else
        //{
      //  hessianMatrix(i2,i3) = 0.5*(hessianMatrix(i3,i2) + gradientTmp[i3]);
        //}
              std::cout<< "\n For index (" << i2 << "," << i3 << "), RN = " << rightNeighbor << ", LN = " << leftNeighbor;
              std::cout << " \n " << hessianMatrix(i2,i3);
          }
       }
   }
   std::cout << "\n Hessian = " << hessianMatrix << " \n";
   // COMPUTE SHAPE OPERATOR
   //   find gradient norm and normalize to get normal
   double gradientNorm = 0.0;
   VectorType normal;
   for(i1=0;i1<itkGetStaticConstMacro(ImageDimension2);i1++)
   {
       gradientNorm+= (gradientVector[i1])*(gradientVector[i1]);
   }
   gradientNorm = sqrt(gradientNorm);
   std::cout<< "\n\n Gradient norm = " << gradientNorm;
   std::cout<< "\n normal = ";
   for(i1=0;i1<itkGetStaticConstMacro(ImageDimension2);i1++)
   {
       normal[i1] = gradientVector[i1]/gradientNorm;
       std::cout << normal[i1] << " ";
   }
   std::cout<<"\n";

   // compute (I-NN') term * gradientNorm
   // find outer product of normal with itself and subtract from identity matrix
   TensorType tmpMatrix;
   for(i1=0;i1<itkGetStaticConstMacro(ImageDimension2);i1++)
   {
       for(i2=0;i2<itkGetStaticConstMacro(ImageDimension2);i2++)
       {
           tmpMatrix(i1,i2) = - normal[i1]*normal[i2];
           if(i1==i2)
               tmpMatrix(i1,i2) += 1.0;
           tmpMatrix(i1,i2) /= gradientNorm;
       }
   }
   
   // Multiple the Hessian, (I-NN') and the gradientNorm to get the shape operator
   //   double shapeOperator[itkGetStaticConstMacro(ImageDimension2)][itkGetStaticConstMacro(ImageDimension2)];
   for(i1=0;i1<itkGetStaticConstMacro(ImageDimension2);i1++)
   {
       for(i2=0;i2<itkGetStaticConstMacro(ImageDimension2);i2++)
       {
           // i1-th row of (I-NN') and i2-th column of hessian to be multipled
           //  shapeOperator(i1,i2) = 0.0;
           shapeOperator[i1][i2] = 0.0;
           for(i3=0;i3<itkGetStaticConstMacro(ImageDimension2);i3++)
           {
               //  shapeOperator(i1,i2) += tmpMatrix(i1,i3)*hessianMatrix(i3,i2);
               shapeOperator[i1][i2] += tmpMatrix(i1,i3)*hessianMatrix(i3,i2);
           }
       }
   }// SHAPE OP CAN'T BE RETURNED AS SYM TENSOR hence returning as a matrix
   return shapeOperator;

}

/** Evaluate the function at the specifed point */
template <class TInputImage, class TOutput>
typename ShapeOperatorImageFunction<TInputImage,TOutput>::OutputType
ShapeOperatorImageFunction<TInputImage,TOutput>
::Evaluate(const PointType& point) const
{
  IndexType index;
  this->ConvertPointToNearestIndex( point , index );
  return this->EvaluateAtIndex ( index );
}

/** Evaluate the function at specified ContinousIndex position.*/
template <class TInputImage, class TOutput>
typename ShapeOperatorImageFunction<TInputImage,TOutput>::OutputType
ShapeOperatorImageFunction<TInputImage,TOutput>
::EvaluateAtContinuousIndex(const ContinuousIndexType & cindex ) const
{
  IndexType index;
  this->ConvertContinuousIndexToNearestIndex( cindex, index  ); 
  return this->EvaluateAtIndex( index );
}

} // end namespace itk

#endif
