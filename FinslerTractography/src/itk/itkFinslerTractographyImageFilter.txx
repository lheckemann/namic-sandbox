#ifndef _itkFinslerTractographyImageFilter_txx
#define _itkFinslerTractographyImageFilter_txx

#include <iostream>

#include "itkFinslerTractographyImageFilter.h"

namespace itk
{


/**
 *    Constructor
 */
template <class TInputImage,class TOutputImage>
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::FinslerTractographyImageFilter() :
  m_ConvergenceParameter(0),
  m_GradientDiffusionVectorsContainer(0)
{
  this->SetNumberOfRequiredOutputs( 2 );

  OutputImagePointer arrivalTimes = OutputImageType::New();
  this->SetNthOutput( 0, arrivalTimes.GetPointer() );

  VectorImagePointer arrivalVectors = VectorImageType::New();
  this->SetNthOutput( 1, arrivalVectors.GetPointer() );

  this->m_ConvergenceParameter = 10;
  this->m_GradientDiffusionVectorsContainer = NULL;
}


/**
 *  Set the gradient directions
 */
template <class TInputImage,class TOutputImage>
void 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::SetGradientDiffusionVectors( GradientDiffusionVectorsContainerType *gradientDirections )
{
  this->m_GradientDiffusionVectorsContainer = gradientDirections;
}


/**
 *  Return the arrival times Image pointer
 */
template <class TInputImage,class TOutputImage>
typename FinslerTractographyImageFilter<
  TInputImage,TOutputImage>::OutputImageType * 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::GetArrivalTimes(void)
{
  return  dynamic_cast< OutputImageType * >(
    this->ProcessObject::GetOutput(0) );
}


/**
 *  Return the arrival vectors
 */
template <class TInputImage,class TOutputImage>
typename FinslerTractographyImageFilter<
  TInputImage,TOutputImage>::VectorImageType * 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::GetArrivalVectors(void)
{
  return  dynamic_cast< VectorImageType * >(
    this->ProcessObject::GetOutput(1) );
}


/**
 *  Prepare data for computation
 */
template <class TInputImage,class TOutputImage>
void 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::PrepareData(void) 
{
  itkDebugMacro(<< "PrepareData Start");

  InputImagePointer  inputImage  = 
    dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(0) );

  /* Prepare the arrival times */
  OutputImagePointer arrivalTimes = this->GetArrivalTimes();

  arrivalTimes->CopyInformation( inputImage );

  arrivalTimes->SetBufferedRegion( 
    inputImage->GetBufferedRegion() );

  arrivalTimes->SetRequestedRegion( 
    inputImage->GetRequestedRegion() );

  arrivalTimes->Allocate();
  arrivalTimes->FillBuffer( itk::NumericTraits<
                            typename OutputImageType::PixelType >::max() );

  /* Prepare the arrival vectors */
  VectorImagePointer arrivalVectors = GetArrivalVectors();

  arrivalVectors->CopyInformation( inputImage );

  arrivalVectors->SetBufferedRegion( 
    inputImage->GetBufferedRegion() );

  arrivalVectors->SetRequestedRegion( 
    inputImage->GetRequestedRegion() );

  arrivalVectors->Allocate();

  itkDebugMacro(<< "PrepareData End");    
}


/**
 *  Compute Arrival Times and Arrival Vectors
 */
template <class TInputImage,class TOutputImage>
void 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::GenerateData() 
{
  this->PrepareData();

  /** Specify images and regions */
  InputImagePointer speedImage       =  this->GetInput();
  typename OutputImageType::RegionType region = speedImage->GetRequestedRegion();
  InputIteratorType speedImageIt( speedImage, region );

  OutputImagePointer arrivalTimes    =  this->GetArrivalTimes();
  OutputIteratorType arrivalTimesIt( arrivalTimes, region );

  VectorImagePointer arrivalVectors  =  this->GetArrivalVectors();
  VectorIteratorType arrivalVectorsIt( arrivalVectors, region );  
  typename VectorImageType::PixelType vectorPixel;
  
  itkDebugMacro (<< "Region to process " << region);

  /** Instantiate the directional iterator */
  typename InputImageType::IndexType start = region.GetIndex();
  typename InputImageType::SizeType size = region.GetSize();

  typename InputImageType::RegionType region2;
  typename InputImageType::IndexType start2;
  typename InputImageType::SizeType size2;

  const unsigned int radius = 1;

  size2[0] = size[0] - 2 * radius;
  size2[1] = size[1] - 2 * radius;
  size2[2] = size[2] - 2 * radius;

  start2[0] = start[0] + radius;
  start2[1] = start[1] + radius;
  start2[2] = start[2] + radius;

  region2.SetSize( size2 );
  region2.SetIndex( start2 );

  InputDirConstIteratorType speedImageDirIt( speedImage, region2 );
  OutputDirIteratorType arrivalTimesDirIt( arrivalTimes, region2 );
  VectorDirIteratorType arrivalVectorsDirIt( arrivalVectors, region2 );

  /** Initialize the iterations */
  speedImageDirIt.SetRadius( radius );
  speedImageDirIt.GoToBegin();
  arrivalTimesDirIt.SetRadius( radius );
  arrivalTimesDirIt.GoToBegin();
  arrivalVectorsDirIt.SetRadius( radius );
  arrivalVectorsDirIt.GoToBegin();

  InputPixelArrayType inputPixelArray;
  inputPixelArray.resize( 7 );

  typename OutputImageType::PixelType deltaIteration =
    itk::NumericTraits<
    typename OutputImageType::PixelType >::max();

  int dir_sign, dir;
  unsigned int numberOfComponentsPerPixel =
    speedImage->GetNumberOfComponentsPerPixel();
  typename VectorImageType::PixelType::ComponentType speed_pt;

  while( deltaIteration > m_ConvergenceParameter )
    {
    while( !speedImageDirIt.IsAtEnd() )
      {
      while( !speedImageDirIt.IsAtEndOfDirection() )
        {
        InputPixelType neighborValue = speedImageDirIt.Get();

        inputPixelArray[0] = speedImageDirIt.GetNeighborPixel(13);
        inputPixelArray[1] = speedImageDirIt.GetNeighborPixel(4);
        inputPixelArray[2] = speedImageDirIt.GetNeighborPixel(12);
        inputPixelArray[3] = speedImageDirIt.GetNeighborPixel(14);
        inputPixelArray[4] = speedImageDirIt.GetNeighborPixel(10);
        inputPixelArray[5] = speedImageDirIt.GetNeighborPixel(16);
        inputPixelArray[6] = speedImageDirIt.GetNeighborPixel(22);

        /* iterate through all positive directions */
        for ( dir_sign = -1; dir_sign <= 1; dir_sign += 2 )
          {
          for ( dir = 0; dir < numberOfComponentsPerPixel; dir += 1 )
            {
            /* get the local cost and direction for this position and direction */
            speed_pt = inputPixelArray[0][dir];
//             y_comp = ((double)dir_sign)*get_grads(grads,grads_dims,dir,0);
//             x_comp = ((double)dir_sign)*get_grads(grads,grads_dims,dir,1);
//             z_comp = ((double)dir_sign)*get_grads(grads,grads_dims,dir,2);

//             /* choose the neighbors to use */
//             if ( y_comp > 0 )
//               {
//               tstar_y = tstar_vect[1];
//               lstar_y = get_lstar(lstar,lstar_dims,y+1,x,z);
//               }
//             else
//               {
//               tstar_y = tstar_vect[0];
//               lstar_y = get_lstar(lstar,lstar_dims,y-1,x,z);
//               }
//             if ( x_comp > 0 )
//               {
//               tstar_x = tstar_vect[3];
//               lstar_x = get_lstar(lstar,lstar_dims,y,x+1,z);
//               }
//             else
//               {
//               tstar_x = tstar_vect[2];
//               lstar_x = get_lstar(lstar,lstar_dims,y,x-1,z);
//               }
//             if ( z_comp > 0 )
//               {
//               tstar_z = tstar_vect[5];
//               lstar_x = get_lstar(lstar,lstar_dims,y,x,z+1);
//               }
//             else
//               {
//               tstar_z = tstar_vect[4];
//               lstar_x = get_lstar(lstar,lstar_dims,y,x,z-1);
//               }

//             /* take absolute value of directional components*/
//             abs_y_comp = abs_double(y_comp);
//             abs_x_comp = abs_double(x_comp);
//             abs_z_comp = abs_double(z_comp);

//             /* check if the tstar values are inf values */
//             /* don't use tstar values if they are inf */
//             num_inf_neighbors = 0;
//             if( tstar_y >= INF )
//               {
//               abs_y_comp = 0;
//               num_inf_neighbors = num_inf_neighbors + 1;
//               }
//             if( tstar_x >= INF )
//               {
//               abs_x_comp = 0;
//               num_inf_neighbors = num_inf_neighbors + 1;
//               }
//             if( tstar_z >= INF )
//               {
//               abs_z_comp = 0;
//               num_inf_neighbors = num_inf_neighbors + 1;
//               }

//             /* compute the new arrival time */
//             if( num_inf_neighbors < 3 )
//               {
//               /* renormalize the directional weights */
//               if( num_inf_neighbors > 0 )
//                 {
//                 denom = sqrt( y_comp * y_comp +
//                               x_comp * x_comp +
//                               z_comp * z_comp );
//                 y_comp = y_comp / denom;
//                 x_comp = x_comp / denom;
//                 z_comp = z_comp / denom;
//                 abs_y_comp = abs_double(y_comp);
//                 abs_x_comp = abs_double(x_comp);
//                 abs_z_comp = abs_double(z_comp);
//                 }

//               /* compute the new arrival time */
//               tmp = (tstar_y * abs_y_comp + tstar_x * abs_x_comp
//                      + tstar_z * abs_z_comp + fs_pt)
//                 / (abs_y_comp + abs_x_comp + abs_z_comp);

//               /* update if new arrival time is lower than the old time */
//               if ( tmp < get_tstar(tstar,tstar_dims,y,x,z) )
//                 {
//                 if( tmp < 0 )
//                   printf("ERROR: Negative arrival times were computed!\n");

//                 /* compute the new euclidean length */
//                 lstar_val = (lstar_y * abs_y_comp + lstar_x * abs_x_comp
//                              + lstar_z * abs_z_comp + 1)
//                   / (abs_y_comp + abs_x_comp + abs_z_comp);

//                 /* write results to the data structures */
//                 set_tstar(tstar,tstar_dims,y,x,z,tmp);
//                 set_lstar(lstar,lstar_dims,y,x,z,lstar_val);
//                 set_dstar(dstar,dstar_dims,y,x,z,0,y_comp);
//                 set_dstar(dstar,dstar_dims,y,x,z,1,x_comp);
//                 set_dstar(dstar,dstar_dims,y,x,z,2,z_comp);

//                 /* update the convergence parameter */
//                 delta_iteration = delta_iteration + tstar_vect[6]-tmp;
//                 }
//              }
            } /* End dir for loop */
          } /* End dir_sign for loop */


        // Access the neighbor pixels
        for(unsigned int k=0; k < 27; k++)
          {
          InputPixelType neighborValue = speedImageDirIt.GetNeighborPixel(k);
          }
        arrivalTimesDirIt.Set( 25 );
        arrivalVectorsDirIt.Set( vectorPixel );

        ++speedImageDirIt;
        ++arrivalTimesDirIt;
        ++arrivalVectorsDirIt;
        }
      speedImageDirIt.NextDirection();
      arrivalTimesDirIt.NextDirection();
      arrivalVectorsDirIt.NextDirection();
      }
    }
  itkDebugMacro(<< "GenerateData");
  
} // end GenerateData()


/**
 *  Print Self
 */
template <class TInputImage,class TOutputImage>
void 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "FinslerTractography: " << std::endl;
  os << indent << "  ConvergenceParameter: " << m_ConvergenceParameter << std::endl;
  os << indent << "  GradientDiffusionVectorsContainer: "
     << m_GradientDiffusionVectorsContainer << std::endl;
}


} // end namespace itk

#endif
