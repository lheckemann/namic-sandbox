/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkSymmetricSpaceTensorAnisotropicDiffusionFunction.txx,v $
Language:  C++
Date:      $Date: 2003/09/10 14:28:58 $
Version:   $Revision: 1.6 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSymmetricSpaceTensorAnisotropicDiffusionFunction_txx_
#define __itkSymmetricSpaceTensorAnisotropicDiffusionFunction_txx_
#include "itkSymmetricSpaceTensorAnisotropicDiffusionFunction.h"

#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodAlgorithm.h"

//STL include
#include<vector>



namespace itk {

template<class TImage>
double SymmetricSpaceTensorAnisotropicDiffusionFunction<TImage>
::m_MIN_NORM = 1.0e-10;
  
template<class TImage>
  SymmetricSpaceTensorAnisotropicDiffusionFunction<TImage>
  ::SymmetricSpaceTensorAnisotropicDiffusionFunction()
    {
    unsigned int i, j;
    RadiusType r;

    for (i = 0; i < ImageDimension; ++i)
      {
      r[i] = 1;
      }
    this->SetRadius(r);

    // Dummy neighborhood used to set up the slices.
    Neighborhood<PixelType, ImageDimension> it;
    it.SetRadius(r);

    // Slice the neighborhood
    m_Center =  it.Size() / 2;

    for (i = 0; i< ImageDimension; ++i)
      { m_Stride[i] = it.GetStride(i); }

   /* for (i = 0; i< ImageDimension; ++i)
      { x_slice[i]  = std::slice( m_Center - m_Stride[i], 3, m_Stride[i]); }

    for (i = 0; i< ImageDimension; ++i)
      {
      for (j = 0; j < ImageDimension; ++j)
        {
        // For taking derivatives in the i direction that are offset one
        // pixel in the j direction.
        xa_slice[i][j]
          = std::slice((m_Center + m_Stride[j])-m_Stride[i], 3, m_Stride[i]); 
        xd_slice[i][j]
          = std::slice((m_Center - m_Stride[j])-m_Stride[i], 3, m_Stride[i]);
        }
      }
     */ 
    }

  template <class TImage>
    void
    SymmetricSpaceTensorAnisotropicDiffusionFunction<TImage>
    ::CalculateAverageGradientMagnitudeSquared(TImage *ip)
      {
      typedef ConstNeighborhoodIterator<TImage> RNI_type;
      typedef ConstNeighborhoodIterator<TImage> SNI_type;
      typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TImage> BFC_type;

      unsigned int i, j;
      unsigned int numNeighbors;
      double                                    accumulator;
      double                                    normSquared;
      unsigned long                             counter;
      BFC_type                                  bfc;
      typename BFC_type::FaceListType           faceList;
      typename BFC_type::FaceListType::iterator fit;
      typename RNI_type::RadiusType             radius;

      RNI_type                                    iterator_list[ImageDimension];
      SNI_type                               face_iterator_list[ImageDimension];
      typedef typename PixelType::ValueType PixelValueType;
      typedef typename RNI_type::RadiusType RadiusType;


      //Creates a unit radius for each axial direction
      std::vector<RadiusType> radiusList(ImageDimension);
      for (i = 0; i < ImageDimension; ++i)
        {
        RadiusType temp;  
        //Initialize radius
        temp[0]=0;
        temp[1]=0;
        temp[2]=0;
        temp[i]=1;
        radiusList[i]=temp;
        }

      // Get the various region "faces" that are on the data set boundary.
      faceList = bfc(ip, ip->GetRequestedRegion(), radius);
      fit      = faceList.begin();

      // Now do the actual processing
      accumulator = 0.0;
      counter     = 0;
      numNeighbors=(ImageDimension*2);  //We consider only neighbors along axial directions 
      // First process the non-boundary region

      // Instead of maintaining a single N-d neighborhood of pointers,
      // we maintain a list of 1-d neighborhoods along each axial direction.
      // This is more efficient for higher dimensions.
     
      for (i = 0; i < ImageDimension; ++i)
        {
        iterator_list[i]=RNI_type(radiusList[i], ip, *fit);
        iterator_list[i].GoToBegin();
        }  
      while ( !iterator_list[0].IsAtEnd() )
        {
        counter++;
        normSquared=0;
        for (i = 0; i < ImageDimension; ++i)
          {
          normSquared+=computeGradientSquaredFromNeighbors(iterator_list[i],i);
          ++iterator_list[i];
          }
        //We need to divide the normSquared by the number of 
        //Neighbors used in the computation of the gradient
        //We assume neighhbors only along axial directions
          accumulator +=((ImageDimension* normSquared)/((float)numNeighbors));   
        }

      // Go on to the next region(s).  These are on the boundary faces.
      ++fit; 

      while ( fit != faceList.end() )
        {
        for (i = 0; i < ImageDimension; ++i)
          {
          face_iterator_list[i]=SNI_type(radiusList[i], ip, *fit);
          face_iterator_list[i].GoToBegin();
          }

        while ( ! face_iterator_list[0].IsAtEnd() )
          {
          counter++;
          normSquared=0;
          for (i = 0; i < ImageDimension; ++i)
            {
              normSquared+=computeGradientSquaredFromNeighbors(face_iterator_list[i],i);
            ++face_iterator_list[i];
            }
                accumulator+=(ImageDimension*normSquared)/((float)numNeighbors);
          }
        ++fit;
        }


      this->SetAverageGradientMagnitudeSquared((double) accumulator / counter);
      }

  //Returns the squaredGradient along an axial direction in the  neighborHood
  //Assumes that the Neighborhood has a radius 1.
  template <class TImage>
    double
    SymmetricSpaceTensorAnisotropicDiffusionFunction<TImage>
    :: computeGradientSquaredFromNeighbors(ConstNeighborhoodIterator<TImage> it,long dimension)
      {

      PixelType referencePixel,forwardNeighbor,backwardNeighbor;
      referencePixel= it.GetCenterPixel();
      forwardNeighbor=it.GetNext(dimension);
      backwardNeighbor=it.GetPrevious(dimension);
      
/*       std::cout<<"Forward Neighbor:"<<forwardNeighbor<<std::endl; */
/*       std::cout<<"Backward  Neighbor:"<<backwardNeighbor<<std::endl; */
/*       std::cout<<"Reference:"<<referencePixel<<std::endl; */


      TensorType refTensor,logTensor; 
      std::vector<TensorType> neighboringTensors(2);
      double gradientSquared=0; 
      
      for(int i=0;i< VectorDimension;i++)
        {
        refTensor[i]=referencePixel[i];
        neighboringTensors[0][i]=forwardNeighbor[i];
        neighboringTensors[1][i]=backwardNeighbor[i];
        }
     
      //Create a SymmetricSpaceTensorGeometry Object
         SymmetricSpaceTensorGeometry<float, 3>  ssTensorGeometry;

        
       //Sum up the squared gradient over both neighbors
//         std::cout<<"PRINTING TENSORS BEFORE NORMSQUARED"<<std::endl;
//         std::cout<<refTensor<<std::endl;
      for(int i=0;i<neighboringTensors.size();i++)
        {
//         std::cout<<neighboringTensors[i]<<std::endl;
         neighboringTensors[i]=ssTensorGeometry.LogMap(refTensor,neighboringTensors[i]);  
         gradientSquared+=ssTensorGeometry.NormSquared(refTensor,neighboringTensors[i]);
//         std::cout<<"GRADIENT SQUARED CURRENT="<<gradientSquared<<std::endl;
        }

//      std::cout<<"GRADIENT SQUARED _RIEMANNIAN SPACE:"<<std::endl;
//      std::cout<<gradientSquared<<std::endl;
      return(gradientSquared);
      }

//Computes the update for each tensor in every iteration  
template<class TImage>
typename SymmetricSpaceTensorAnisotropicDiffusionFunction<TImage>::PixelType
SymmetricSpaceTensorAnisotropicDiffusionFunction<TImage>
::ComputeUpdate(const NeighborhoodType &it, void *,
                const FloatOffsetType&)
{
  unsigned int i, j, k;
  PixelType delta;

  double Cx_forward[ImageDimension];
  double Cx_backward[ImageDimension];
  double gradientNormSquared_forward[ImageDimension];
  double gradientNormSquared_backward[ImageDimension];

  // Remember: PixelType is a Vector of length VectorDimension.
  PixelType dx_forward[ImageDimension];
  PixelType dx_backward[ImageDimension];
  PixelType reference,forward_neighbor,backward_neighbor;
  
  //std::cout<<"PRINTING GRADIENT"<<endl;
  
  for (i = 0; i < ImageDimension; i++)
    {
     reference = it.GetPixel(m_Center);
     forward_neighbor= it.GetPixel(m_Center + m_Stride[i]);
     backward_neighbor=it.GetPixel(m_Center - m_Stride[i]);

  // std::cout<<"Forward Nbr"<<endl;
  // std::cout<<forward_neighbor<<std::endl;
  // std::cout<<"Backward Nbr"<<endl;
  // std::cout<<backward_neighbor<<std::endl;
 
    // Calculate the derivative with respect to the forward and backward neighbor.
    dx_forward[i] = GetSymmetricSpaceDerivative(reference,forward_neighbor);
    dx_backward[i]=  GetSymmetricSpaceDerivative(reference,backward_neighbor);


  // std::cout<<"DERIVATIVE FORWARD"<<endl;
  // std::cout<<dx_forward[i]<<std::endl;
  // std::cout<<"DERIVATIVE BACKWARD"<<endl;
  // std::cout<<dx_backward[i]<<std::endl;
 
    
    gradientNormSquared_forward[i]=GetSymmetricSpaceNormSquared(reference,forward_neighbor);
//    std::cout<<gradientNormSquared_forward[i]<<std::endl;
    gradientNormSquared_backward[i]=GetSymmetricSpaceNormSquared(reference,backward_neighbor);
    //---------------Gradient Magnitude calculations--------
    // Calculate gradient magnitude approximation in this
    // dimension linked (summed) across the vector components.
    
/*     double sum=0.0; */
/*     for(int z=0;z<3;z++) */
/*       { */
/*            sum+=gradientNormSquared_backward[z]; */
/*            sum+=gradientNormSquared_forward[z]; */
/*       } */
/*     std::cout<<"NET GRADIENT SQUARED="<<sum<<endl;  */
    
    if (m_K == 0.0)
      {       
      Cx_forward[i] = 0.0;
      Cx_backward[i] = 0.0;
      }
    else
      {
      Cx_forward[i]  = ::exp( gradientNormSquared_forward[i]/ m_K );
      Cx_backward[i] = ::exp( gradientNormSquared_backward[i] / m_K ); 
      }
    }
  // Compute update value  
  for (k = 0; k < VectorDimension; k++)
    {
    delta[k] = NumericTraits<ScalarValueType>::Zero;
    for (i = 0; i < ImageDimension; ++i)
      {
      dx_forward[i][k]  *= Cx_forward[i];
      dx_backward[i][k] *= Cx_backward[i];
 
      delta[k] += (dx_forward[i][k] + dx_backward[i][k]);
      }
    }

 // std::cout<<"DELTA (VECTOR)"<<endl;
 // std::cout<<delta<<std::endl;
  return delta;
}

/*******************************************************************
 * This method uses finite difference in the symmetric space to
 * return an approximation to the derivative
 * *****************************************************************/
template <class TImage>
typename SymmetricSpaceTensorAnisotropicDiffusionFunction<TImage>::PixelType
 SymmetricSpaceTensorAnisotropicDiffusionFunction<TImage>
::GetSymmetricSpaceDerivative(PixelType reference,PixelType neighbor)
{
//   std::cout<<"REFERENCE"<<endl;
//   std::cout<<reference<<std::endl;
//   std::cout<<"neighbor"<<endl;
//   std::cout<<neighbor<<std::endl;
 
  
   TensorType refTensor,neighborTensor;
   TensorType derivativeTensor;
   for(int i=0;i<6;i++)
     {
     refTensor[i]=reference[i];
     neighborTensor[i]=neighbor[i];
     }
   //Create a SymmetricSpaceTensorGeometry Object
   SymmetricSpaceTensorGeometry<float, 3>  ssTensorGeometry;
   derivativeTensor=ssTensorGeometry.LogMap(refTensor,neighborTensor);
   
   PixelType derivativePixel;
   for(int i=0;i<6;i++)
           derivativePixel[i]=derivativeTensor[i];

//   std::cout<<"AFTER TAKING LOG MAP"<<endl;
//   std::cout<<derivativePixel<<endl;
   return(derivativePixel);
}

/*******************************************************************
 * This method uses the symmetric space to
 * return the norm of a neighboring Tensor with respect to a 
 * reference Tensor
 * *****************************************************************/
template <class TImage>
double   SymmetricSpaceTensorAnisotropicDiffusionFunction<TImage>
::GetSymmetricSpaceNormSquared(PixelType reference,PixelType neighbor)
{
   double normSquared=0;
   TensorType refTensor,neighborTensor;
   for(int i=0;i<6;i++)
     {
     refTensor[i]=reference[i];
     neighborTensor[i]=neighbor[i];
     }
   
   //Create a SymmetricSpaceTensorGeometry Object
   SymmetricSpaceTensorGeometry<float, 3>  ssTensorGeometry;
   neighborTensor=ssTensorGeometry.LogMap(refTensor,neighborTensor);  
   normSquared=ssTensorGeometry.NormSquared(refTensor,neighborTensor);
   return(normSquared);
}

 
}// end namespace itk
#endif
