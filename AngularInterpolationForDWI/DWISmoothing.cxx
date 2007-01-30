/*=========================================================================

Program:   DWISmoothing
Module:    $RCSfile: DWISmoothing.cxx,v $
Language:  C++
Date:      $Date: 2006/02/08 18:21:06 $
Version:   $Revision: 1.8 $

Copyright (c) General Electric Global Research. All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <math.h>

#include "itkNrrdImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkMetaDataDictionary.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"

#include "itkArray2D.h"
#include "itkVector.h"

#include "itkImage.h"
#include "itkVectorImage.h"
#include "vnl/vnl_math.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionConstIteratorWithIndex.h"

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/algo/vnl_svd.h"
#include "itkRawImageIO.h"
#include "itkNrrdImageIO.h"

#define DEBUG_MODE 

typedef float ValueType;

const double PI = 3.14159265358979323846;

int nNbr = 6;

void BuildNeighborTable( const std::vector< vnl_vector_fixed<double, 3> > & ,
                         itk::Array2D<int> & , itk::Array2D<double> &, double );

void BuildInterpolationTable( const std::vector< vnl_vector_fixed<double, 3> > & ,
                              const std::vector< vnl_vector_fixed<double, 3> > & ,
                         itk::Array2D<int> & , itk::Array2D<double> &, double );

bool Bidirection = false;   // default is unidirection
bool Smoothing = false;   // default is interpolating
// The difference between smoothing and interpolation is that smoothing uses
// the measurement in the direction, while interpolation uses only measurements
// in *other* directions
bool NewDirections = false;

int main(int ac, char* av[])
{

  if(ac < 6)
    {
    std::cerr << "Usage: " << av[0] << " smooth/interpolate[s/i] unidirection/bidirection[u/b] \n";
    std::cerr << "     NrrdFileName.nhdr Smoothed_RAW_Image Sigma <FileOfNewDirections>\n";
    std::cerr << "Version:   $Revision: 1.8 $" << std::endl;

    return EXIT_FAILURE;
    }
  
  if (av[1][0] == 's')
    {
    Smoothing = true;
    }
  else if (av[1][0] == 'i')
    {
    Smoothing = false;
    }

  if (av[2][0] == 'b')
    {
    Bidirection = true;
    }
  else if (av[2][0] == 'u')
    {
    Bidirection = false;
    }

  std::vector< vnl_vector_fixed<double, 3> > NewVectors;
  int nNewVectors = 0;
  if ( ac >6 ) // new directions are given, read them in
    {
    FILE *fp = fopen(av[6], "r");
    if (fp != NULL)
      {
      NewDirections = true;
      double x,y,z;
      vnl_vector_fixed<double, 3> vect3d;
      while (!feof(fp))
        {
        fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
        vect3d[0] = x; vect3d[1] = y; vect3d[2] = z;
        double mag = vect3d.magnitude();
        vect3d /= mag;
        NewVectors.push_back(vect3d);
        }
      fclose(fp);
      nNewVectors = NewVectors.size();
      }
    }

  const unsigned int dimVolume = 3;
//  const unsigned int dimVector = 35;
  unsigned int nMeasurement;

//  typedef itk::Vector<ValueType, dimVector> PixelType;
  typedef itk::VectorImage<ValueType, dimVolume> ImageType;
  typedef ImageType::PixelType PixelType;

  double sigma;
  sigma = atof(av[5]);

  itk::ImageFileReader<ImageType>::Pointer reader 
    = itk::ImageFileReader<ImageType>::New();

  reader->SetFileName(av[3]);
  ImageType::Pointer img;
  
  // Read in the nrrd data
  try
    {
    reader->Update();
    img = reader->GetOutput();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }
  
  int dimVector = img->GetPixel(img->GetLargestPossibleRegion().GetIndex()).Size();

  //   parse diffusion vectors
  itk::MetaDataDictionary imgMetaDictionary = img->GetMetaDataDictionary();    
  std::vector<std::string> imgMetaKeys = imgMetaDictionary.GetKeys();
  std::vector<std::string>::iterator itKey = imgMetaKeys.begin();
  std::string metaString;
  
  int k = 0;
  vnl_vector_fixed<double, 3> vect3d;
  std::vector< vnl_vector_fixed<double, 3> > DiffusionVectors;

  bool Baseline = true;
  int nBaseline;
  for ( ; itKey != imgMetaKeys.end(); itKey ++)
    {

    int pos;
    double x,y,z;

    ExposeMetaData (imgMetaDictionary, *itKey, metaString);

    pos = itKey->find("DWMRI_NEX_0000");  // find the start point of the diffusion weighted measures
                                          // get number of baselineimages;
    if (Baseline && pos == -1)
      {
      continue;
      }
    else if (Baseline && pos != -1) 
      {
      Baseline = false;
      std::sscanf(metaString.c_str(), "%d\n", &nBaseline);
      continue;
      }
    if (Baseline)
      {
      continue;
      }

    pos = itKey->find("DWMRI_gradient");
    if (pos == -1)
      {
      continue;
      }
      
    std::sscanf(metaString.c_str(), "%lf %lf %lf\n", &x, &y, &z);
    vect3d[0] = x; vect3d[1] = y; vect3d[2] = z;
    if (x == 0 && y == 0 && z == 0)  // indicating baseline image
      {
      continue;
      }
    double mag = vect3d.magnitude();
    vect3d /= mag;

    DiffusionVectors.push_back(vect3d);
    k ++;
    }
  nMeasurement = DiffusionVectors.size();

  if (nMeasurement < nNbr)
    {
    nNbr = nMeasurement;
    }

  // build neighborhood table and compute interpolation coefficients
  itk::Array2D<int> nbrTable; 
  itk::Array2D<double> nbrCoeff; 
  // storage for smoothed measurements
  PixelType tempArray;
  if (NewDirections)
    {
    std::cout << "NewDirections\n";
    dimVector = nNewVectors + nBaseline;
    nbrTable.SetSize(nNewVectors, nNbr);
    nbrCoeff.SetSize(nNewVectors, nNbr);
    BuildInterpolationTable ( NewVectors, DiffusionVectors, nbrTable, nbrCoeff, sigma );
    }
  else
    {
    nNewVectors = nMeasurement;
    nbrTable.SetSize(nNewVectors, nNbr);
    nbrCoeff.SetSize(nNewVectors, nNbr);
    BuildNeighborTable ( DiffusionVectors, nbrTable, nbrCoeff, sigma );
    }

  tempArray.SetSize(dimVector);

  // iterate through all voxels
  typedef itk::VectorImage<ValueType, dimVolume> VolumeType;
  VolumeType::Pointer dummyImage = VolumeType::New();

  ImageType::RegionType nrrdRegion = img->GetLargestPossibleRegion();
  ImageType::SizeType nrrdSize = nrrdRegion.GetSize();
  ImageType::IndexType nrrdIndex = nrrdRegion.GetIndex();

  VolumeType::RegionType volRegion;

  for (int k = 0; k < dimVolume; k++)
    {
    volRegion.SetIndex(k, nrrdIndex[k]);
    volRegion.SetSize(k, nrrdSize[k]);
    }
  dummyImage->SetRegions(volRegion);
  //dummyImage->Allocate();

  itk::ImageRegionConstIteratorWithIndex<VolumeType> it(img, img->GetLargestPossibleRegion());

  for ( it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    ImageType::IndexType volIndex = it.GetIndex();

    double averageBaseline = 0;
    for (int k = 0; k < nBaseline; k++)
      {
      tempArray[k] = static_cast<double>(img->GetPixel(volIndex)[k]);
      averageBaseline += tempArray[k];
      }
    averageBaseline /= static_cast<double>( nBaseline );
    if (volIndex[0] == 64 && volIndex[1] == 64 && volIndex[2] == 30)
      {
      std::cout << volIndex << tempArray << std::endl;
      }
 
    // if Baseline value is too small, set everything to the Baseline value and continue.
    if (averageBaseline < 10)
      {
      for (int k = 0; k < nNewVectors; k++)
        {
        tempArray[k+nBaseline] = static_cast<ValueType> (averageBaseline);
        }
      img->SetPixel(volIndex, tempArray);
      continue;
      }

    for (int k = 0; k < nNewVectors; k++)
      {
      double weight;
      double value;
      double gvalue;
      if (Smoothing && !NewDirections)
        {
        weight = 1.0;
        gvalue = static_cast<double>(img->GetPixel(volIndex)[k+nBaseline]);
        if (gvalue < 1)
          {
          value = 0;
          }
        else
          {
          value = -log(gvalue/averageBaseline);
          }
        }
      else
        {
        weight = 0.0;
        value = 0;
        }

      for (int m = 0; m < nNbr; m ++)
        {
        if (volIndex[0] == 64 && volIndex[1] == 64 && volIndex[2] == 31)
          {
          //std::cout << weight << ": " <<  nbrCoeff(k,m) << "---" << tempArray[k+nBaseline] << std::endl;
          }
        weight += nbrCoeff(k,m);
        gvalue = static_cast<double>(img->GetPixel(volIndex)[nBaseline + nbrTable(k, m)]);
        if (gvalue < 1)
          {
          continue;
          }
        else
          {
          value -= nbrCoeff(k,m) * log(gvalue/averageBaseline);
          }
        }
      value /= weight;
      value = exp(-value)*averageBaseline;
      tempArray[k+nBaseline] = static_cast<ValueType> (value);
      if (volIndex[0] == 64 && volIndex[1] == 64 && volIndex[2] == 30)
        {
        std::cout << volIndex << tempArray << std::endl;
        }

      }
      img->SetPixel(volIndex, tempArray);
    }
    
  // vector image writer still not working
  FILE *fp = fopen( av[4], "wb");
  for (unsigned int k = 0; k < dimVector; k ++)
    {
    for ( it.GoToBegin(); !it.IsAtEnd(); ++it)
      {
      fwrite(&it.Get()[k],1,sizeof(ValueType), fp);
      }
    }
  fclose(fp);

  exit (0);

}

void BuildInterpolationTable( const std::vector< vnl_vector_fixed<double, 3> > & newVecs,
                         const std::vector< vnl_vector_fixed<double, 3> > & vecs,
                         itk::Array2D<int> & nbrtable, itk::Array2D<double> & nbrcoeff , double sigma )
{

  // Get the number of neighboring points
  int nNbr = nbrtable.cols(); 
  int nVecs = vecs.size();
  int nNewVecs = newVecs.size();

  // compute angles between each pair of vectors
  // sort the angles in ascending order
  // compute the interpolation coefficients
  itk::Array<double> angle(nVecs);
 
  angle.Fill(PI);
  nbrtable.Fill(0);
  nbrcoeff.Fill(PI);
  
  sigma = 2*sigma*sigma;
  
  for (int k = 0; k < nNewVecs; k++)
    {
    for (int m = 0; m < nVecs; m++)
      {
      angle(m) = std::acos(dot_product(newVecs[k], vecs[m]));
      if (Bidirection)
        {
        if (angle(m) > PI/2)
          {
          angle(m) = PI-angle(m);
          }
        }
      }

#ifdef DEBUG_MODE
    std::cout << angle << std::endl;
#endif

    // find the nNbr neighbors with smallest angular distance
    for (int m = 0; m < nNewVecs; m++)
      {
      int n;
      for (n = 0; n < nNbr; n++)
        {
        if (angle[m] < nbrcoeff(k,n))
          {
          break;
          }
        }
#ifdef DEBUG_MODE
      //std::cout << "k = " << k << " m =  "<< m << " n = " << n << std::endl; 
#endif
      for (int q = nNbr-1 ; q >= n+1; q--) // move data backwards
        {
        nbrcoeff(k,q) = nbrcoeff(k, q-1);
        nbrtable(k,q) = nbrtable(k, q-1);
        }
      if (n < nNbr)
        {
        nbrcoeff(k,n) = angle[m];
        nbrtable(k,n) = m;
        }
      }
    }

  for (int k = 0; k < nNewVecs; k++)
    {
    for (int m = 0; m < nNbr; m ++)
      {
      nbrcoeff(k,m) = exp(-nbrcoeff(k,m)*nbrcoeff(k,m)/sigma);
      }
    }

#ifdef DEBUG_MODE
  std::cout << nbrtable << std::endl;
  std::cout << nbrcoeff << std::endl;
#endif
  return;
}


void BuildNeighborTable( const std::vector< vnl_vector_fixed<double, 3> > & vecs,
                         itk::Array2D<int> & nbrtable, itk::Array2D<double> & nbrcoeff , double sigma )
{
  // Get the number of neighboring points
  int nNbr = nbrtable.cols();
    int nVecs = vecs.size();

  // compute angles between each pair of vectors
  // sort the angles in ascending order
  // compute the interpolation coefficients
  itk::Array<double> angle(nVecs);
  angle.Fill(PI);
  nbrtable.Fill(0);
  nbrcoeff.Fill(PI);
  
  sigma = 2*sigma*sigma;
 
  for (int k = 0; k < nVecs; k++)
    {
    for (int m = 0; m < nVecs; m++)
      {
      if (k == m)
        {
        angle(m) = PI;
        }
      else
        {
        angle(m) = std::acos(dot_product(vecs[k], vecs[m]));
        if (Bidirection)
          {
          if (angle(m) > PI/2)
            {
            angle(m) = PI-angle(m);
            }
          }
        }
      }

    // find the nNbr neighbors with smallest angular distance
    for (int m = 0; m < nVecs; m++)
      {
      int n;
      for (n = 0; n < nNbr; n++)
        {
        if (angle[m] < nbrcoeff(k,n))
          {
          break;
          }
        }
      for (int q = nNbr-1 ; q >= n+1; q--) // move data backwards
        {
        nbrcoeff(k,q) = nbrcoeff(k, q-1);
        nbrtable(k,q) = nbrtable(k, q-1);
        }
      if (n < nNbr)
        {
        nbrcoeff(k,n) = angle[m];
        nbrtable(k,n) = m;
        }
      }
    }

  for (int k = 0; k < nVecs; k++)
    {
    for (int m = 0; m < nNbr; m ++)
      {
      nbrcoeff(k,m) = exp(-nbrcoeff(k,m)*nbrcoeff(k,m)/sigma);
      }
    }

#ifdef DEBUG_MODE
  std::cout << nbrtable << std::endl;
  std::cout << nbrcoeff << std::endl;
#endif
  return ;
}
