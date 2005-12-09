/*=========================================================================

Program:   ReconstructTensor
Module:    $RCSfile: ReconstructTensor.cxx,v $
Language:  C++
Date:      $Date: 2005/04/29 15:13:22 $
Version:   $Revision: 1.5 $

Copyright (c) General Electric Global Research. All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include "itkNrrdImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkMetaDataDictionary.h"

#include "itkImage.h"
#include "vnl/vnl_math.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/algo/vnl_svd.h"

#include <typeinfo.h>

const bool TENSOR_RECON_FAILURE = false;
const bool TENSOR_RECON_SUCCESS = true;

typedef vnl_vector_fixed<double, 3> VectorType;
typedef itk::Vector<float, 3> OutputVectorType;

bool ReconstructTensor(const vnl_vector<double> &,       // DWI measurements
                       const vnl_svd<double> &,                 // svd solver computed from the coeffecients
                       vnl_matrix_fixed<double, 3, 3> &);       // return tensor

void ComputeTensorQuantities(const vnl_matrix_fixed<double, 3, 3> &,    // input tensor
                             double &,                                  // return fractional anisotropy
                             VectorType &,             // return eigenvalues in descending order
                             VectorType &,             // return primary direction
                             VectorType &,             // return secondary direction
                             VectorType &);            // return least important direction


// a templated image writing function
template <typename W, typename I> 
int _OutputImage(typename W::Pointer writer, typename I::Pointer image, const char* filename)
{
  writer->SetFileName(filename);
  writer->SetInput( image );
      
  try 
    { 
    writer->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
    return EXIT_FAILURE;
    } 
  return EXIT_SUCCESS;
}

template <typename W, typename I> 
int OutputImage(typename W::Pointer writer, typename I::Pointer  image, std::string filename)
{
  int Good = _OutputImage<W, I>(writer, image, filename.c_str());
  return (Good);
}

template <typename W, typename I> 
int OutputImage(typename W::Pointer writer, typename I::Pointer image, const char* filename)
{
  int Good = _OutputImage<W, I>(writer, image, filename);
  return (Good);
}

int main(int ac, char* av[])
{
  
  if(ac < 4)
    {
    std::cerr << "Usage: " << av[0] << " NrrdFileName(.nhdr) FAFileName(.img) threshold(on B0)\n";
    std::cerr << "Version:   $Revision: 1.5 $" << std::endl;

    return EXIT_FAILURE;
    }
  
  const unsigned int dNrrd = 4;
  const unsigned int dVolume = 3;
  unsigned int nMeasurement;
  
  typedef itk::Image<unsigned short, dNrrd> ImageType;
  
  itk::ImageFileReader<ImageType>::Pointer reader 
    = itk::ImageFileReader<ImageType>::New();
  
  ImageType::Pointer img;
  
  // Set the properties for NrrdReader
  reader->SetFileName(av[1]);

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

  //   parse diffusion vectors
  itk::MetaDataDictionary imgMetaDictionary = img->GetMetaDataDictionary();    
  std::vector<std::string> imgMetaKeys = imgMetaDictionary.GetKeys();
  std::vector<std::string>::iterator itKey = imgMetaKeys.begin();
  std::string metaString;
  
  int k = 0;
  vnl_vector_fixed<double, 3> vect3d;
  std::vector< vnl_vector_fixed<double, 3> > DiffusionVectors;

  for ( ; itKey != imgMetaKeys.end(); itKey ++)
    {
    int pos;
    double x,y,z;

    ExposeMetaData (imgMetaDictionary, *itKey, metaString);
    pos = itKey->find("DW_gradient");
    if (pos == -1)
      {
      continue;
      }
      
    std::cout << *itKey << " ---> " << metaString << std::endl;      
    std::sscanf(metaString.c_str(), "%lf %lf %lf\n", &x, &y, &z);
    vect3d[0] = x; vect3d[1] = y; vect3d[2] = z;
    
    DiffusionVectors.push_back(vect3d);
    k ++;
    }
  nMeasurement = k;
  
  if (nMeasurement < 6)
    {
    std::cout << "Error: \n";
    std::cout << "\t Not enough angles. The problem is under constrained.\n";
    return EXIT_FAILURE;
    }
  
  vnl_matrix<double> Coeff(nMeasurement, 6);
  for (int m = 0; m < nMeasurement; m++)
    {
    Coeff[m][0] =     DiffusionVectors[m][0] * DiffusionVectors[m][0];
    Coeff[m][1] = 2 * DiffusionVectors[m][0] * DiffusionVectors[m][1];
    Coeff[m][2] = 2 * DiffusionVectors[m][0] * DiffusionVectors[m][2];
    Coeff[m][3] =     DiffusionVectors[m][1] * DiffusionVectors[m][1];
    Coeff[m][4] = 2 * DiffusionVectors[m][1] * DiffusionVectors[m][2];
    Coeff[m][5] =     DiffusionVectors[m][2] * DiffusionVectors[m][2];
    }
    
  vnl_matrix<double> Matrix4Solve(6,6);

  // if the number of angles equals 6, we will have a unique solution (hopefully)
  // otherwise, we use pseudo-inverse
  if (nMeasurement > 6)
    {
    Matrix4Solve = Coeff.transpose() * Coeff;
    }
  else 
    {
    Matrix4Solve = Coeff;
    }

  Coeff.inplace_transpose();

  vnl_svd<double> svdSolver(Matrix4Solve);

  bool AllDone = false;
  
  std::string nameBase(av[2]);

  std::size_t PosExtName = nameBase.find_last_of('.');
  nameBase = nameBase.substr(0, PosExtName);

  ImageType::IndexType nrrdIdx;
  nrrdIdx.Fill(0);

  vnl_vector<double> B(nMeasurement);
 
  ImageType::RegionType inputRegion =
    reader->GetOutput()->GetLargestPossibleRegion();

  ImageType::SizeType nrrdSize = inputRegion.GetSize();
  ImageType::SpacingType nrrdSpacing = img->GetSpacing();
  ImageType::PointType nrrdOrigin = img->GetOrigin();
  
  ImageType::SizeType sizeIncrement;
  sizeIncrement [0] = 1;
  for (int k = 1; k < dNrrd; sizeIncrement [k++] = 0);
  
  vnl_matrix_fixed<double, 3, 3> tensor;
  VectorType EigenValues;
  VectorType PVec1;
  VectorType PVec2;
  VectorType PVec3;
 
  double dblFA;

  typedef itk::RGBPixel< unsigned char >    PixelType;
  typedef itk::Image<PixelType, dVolume> FAImageType;
  FAImageType::Pointer faImage = FAImageType::New();

  typedef itk::Image<unsigned char, dVolume> FAGrayType;
  FAGrayType::Pointer faGrayImage = FAGrayType::New();

  typedef itk::Image<float, dVolume> ADCImageType;
  ADCImageType::Pointer adcImage = ADCImageType::New();

  typedef itk::Image<OutputVectorType, dVolume> VectorImageType;
  VectorImageType::Pointer vectorImage1 = VectorImageType::New();
  VectorImageType::Pointer vectorImage2 = VectorImageType::New();
  VectorImageType::Pointer vectorImage3 = VectorImageType::New();
  VectorImageType::Pointer eigenValueImage = VectorImageType::New();

  FAImageType::SpacingType faSpacing;
  FAImageType::PointType faOrigin;
  FAImageType::SizeType faSize;
  FAImageType::IndexType faIndex;
  FAImageType::RegionType faRegion;

  FAImageType::SizeType faIncrement;

  faIndex.Fill(0);
  faIncrement[0] = 1;
  for (int k = 1; k < dVolume; k++)
    {
    faIncrement[k] = 0;
    }

  for (int k = 0; k < dVolume; k++)
    {
    faSpacing[k] = nrrdSpacing[k];
    faOrigin[k] = nrrdOrigin[k];
    faSize[k] = nrrdSize[k];
    }

  faRegion.SetSize(faSize);
  faImage->SetOrigin(faOrigin);
  faImage->SetRegions(faRegion);
  faImage->SetSpacing(faSpacing);

  faGrayImage->SetOrigin(faOrigin);
  faGrayImage->SetRegions(faRegion);
  faGrayImage->SetSpacing(faSpacing);
  
  adcImage->SetOrigin(faOrigin);
  adcImage->SetRegions(faRegion);
  adcImage->SetSpacing(faSpacing);

  vectorImage1->SetOrigin(faOrigin);
  vectorImage1->SetRegions(faRegion);
  vectorImage1->SetSpacing(faSpacing);

  vectorImage2->SetOrigin(faOrigin);
  vectorImage2->SetRegions(faRegion);
  vectorImage2->SetSpacing(faSpacing);

  vectorImage3->SetOrigin(faOrigin);
  vectorImage3->SetRegions(faRegion);
  vectorImage3->SetSpacing(faSpacing);

  eigenValueImage->SetOrigin(faOrigin);
  eigenValueImage->SetRegions(faRegion);
  eigenValueImage->SetSpacing(faSpacing);

  faImage->Allocate();
  faGrayImage->Allocate();
  adcImage->Allocate();
  vectorImage1->Allocate();
  vectorImage2->Allocate();
  vectorImage3->Allocate();
  eigenValueImage->Allocate();

  double thresholdvalue = atof(av[3]);

  typedef itk::ImageRegionIteratorWithIndex<FAImageType> IteratorWithIndexType;
  IteratorWithIndexType iiFa ( faImage, faImage->GetLargestPossibleRegion() );

  for (iiFa.GoToBegin(); !iiFa.IsAtEnd(); ++iiFa)
    {
    double b0;

    faIndex = iiFa.GetIndex();
    for (int k = 0; k < dVolume; k++)
      {
      nrrdIdx[k] = faIndex[k];
      }
    nrrdIdx[dNrrd-1] = 0;  
    
    b0 = static_cast<double>(img->GetPixel(nrrdIdx));
    
    if (b0 < thresholdvalue)
      {
      // If b0 is small enough, it makes sense to assume
      // the diffusion at the point is isotropic.

      faImage->GetPixel(faIndex)[0] = 0;
      faImage->GetPixel(faIndex)[1] = 0;
      faImage->GetPixel(faIndex)[2] = 0;

      faGrayImage->GetPixel(faIndex) = 0;
      adcImage->GetPixel(faIndex) = 0;
      
      eigenValueImage->GetPixel(faIndex)[0] = 1;
      eigenValueImage->GetPixel(faIndex)[1] = 1;
      eigenValueImage->GetPixel(faIndex)[2] = 1;
      
      vectorImage1->GetPixel(faIndex)[0] = 0.01;
      vectorImage1->GetPixel(faIndex)[1] = 0;
      vectorImage1->GetPixel(faIndex)[2] = 0;
      
      vectorImage2->GetPixel(faIndex)[0] = 0;
      vectorImage2->GetPixel(faIndex)[1] = 0.01;
      vectorImage2->GetPixel(faIndex)[2] = 0;
      
      vectorImage3->GetPixel(faIndex)[0] = 0;
      vectorImage3->GetPixel(faIndex)[1] = 0;
      vectorImage3->GetPixel(faIndex)[2] = 0.01;
      
      }
    else
      {
      for (k = 1; k <= nMeasurement; k++)
        {
        double b;
        nrrdIdx [dNrrd-1] = k;
        b = static_cast<double>(img->GetPixel(nrrdIdx));
        if (b == 0)
          {
          B[k-1] = 0;
          }
        else
          {
          B[k-1] = -log(b/b0);
          }
        }
      
      if (nMeasurement > 6)
        {
        ReconstructTensor(Coeff*B, svdSolver, tensor);
        }
      else
        {
        ReconstructTensor(B, svdSolver, tensor);
        }
      
      ComputeTensorQuantities(tensor, dblFA, EigenValues, PVec1, PVec2, PVec3);
      
      // the fa values are scaled to have unsigned char type
      faImage->GetPixel (faIndex)[0] = static_cast<unsigned char>(fabs(dblFA * 255 * PVec1[0])); // RGB weighted by
      faImage->GetPixel (faIndex)[1] = static_cast<unsigned char>(fabs(dblFA * 255 * PVec1[1])); // directions
      faImage->GetPixel (faIndex)[2] = static_cast<unsigned char>(fabs(dblFA * 255 * PVec1[2])); 
      
      faGrayImage->GetPixel (faIndex) = static_cast<unsigned char>( dblFA * 255 );

      adcImage->GetPixel(faIndex) = static_cast<float> (EigenValues.sum());
      
      eigenValueImage->GetPixel(faIndex)[0] = EigenValues[0];
      eigenValueImage->GetPixel(faIndex)[1] = EigenValues[1];
      eigenValueImage->GetPixel(faIndex)[2] = EigenValues[2];
      
      vectorImage1->GetPixel(faIndex)[0] = PVec1[0];
      vectorImage1->GetPixel(faIndex)[1] = PVec1[1];
      vectorImage1->GetPixel(faIndex)[2] = PVec1[2];
      
      vectorImage2->GetPixel(faIndex)[0] = PVec2[0];
      vectorImage2->GetPixel(faIndex)[1] = PVec2[1];
      vectorImage2->GetPixel(faIndex)[2] = PVec2[2];
      
      vectorImage3->GetPixel(faIndex)[0] = PVec3[0];
      vectorImage3->GetPixel(faIndex)[1] = PVec3[1];
      vectorImage3->GetPixel(faIndex)[2] = PVec3[2];
      }    
    }
  // Write out the FA image using an image writer  
  typedef itk::ImageFileWriter<FAImageType> FAWriterType;
  FAWriterType::Pointer faWriter = FAWriterType::New();
  std::string fileNameString = nameBase + "_fa.mha";
  OutputImage<FAWriterType, FAImageType>(faWriter, faImage, fileNameString);

  // Write out eigenvalues and eigenvectors using image writers 
  typedef itk::ImageFileWriter<VectorImageType> VectorImageWriterType;
  VectorImageWriterType::Pointer vectorImageWriter = VectorImageWriterType::New();
    
  fileNameString = nameBase + "_eval.mha";
  OutputImage<VectorImageWriterType, VectorImageType>(vectorImageWriter, eigenValueImage, fileNameString);
     
  fileNameString = nameBase + "_evec1.mha";
  OutputImage<VectorImageWriterType, VectorImageType>(vectorImageWriter, vectorImage1, fileNameString);

  fileNameString = nameBase + "_evec2.mha";
  OutputImage<VectorImageWriterType, VectorImageType>(vectorImageWriter, vectorImage2, fileNameString);

  fileNameString = nameBase + "_evec3.mha";
  OutputImage<VectorImageWriterType, VectorImageType>(vectorImageWriter, vectorImage3, fileNameString);

  typedef itk::ImageFileWriter<FAGrayType> FAGrayWriterType;
  FAGrayWriterType::Pointer faGrayWriter = FAGrayWriterType::New();
  fileNameString = nameBase + "_BW.hdr";
  OutputImage<FAGrayWriterType, FAGrayType>(faGrayWriter, faGrayImage, fileNameString);

  typedef itk::ImageFileWriter<ADCImageType> ADCWriterType;
  ADCWriterType::Pointer adcWriter = ADCWriterType::New();
  fileNameString = nameBase + "_adc.hdr";
  OutputImage<ADCWriterType, ADCImageType>(adcWriter, adcImage, fileNameString);

  //  use the fractional anisotropy to segment the white matter.
  //  Needs:
  //      -- a randomness measurement on FA. White matter should
  //         be more determinant;
  //      -- some mathematical morphology operations to make the 
  //         segmented white matter a connected volume with only
  //         the "right" holes (subcortical gray matter and CSF);
  //      


  return EXIT_SUCCESS;  
}

bool ReconstructTensor(const vnl_vector<double> & dwivector,      // DWI measurements
                       const vnl_svd<double> & svdSolver,                     // DWI directions
                       vnl_matrix_fixed<double, 3, 3> & tensor)               // return tensor
{
  
  vnl_vector<double> D(6);
  D = svdSolver.solve(dwivector);
  
  tensor[0][0] = D[0]; tensor[0][1] = D[1]; tensor[0][2] = D[2];
  tensor[1][0] = D[1]; tensor[1][1] = D[3]; tensor[1][2] = D[4];
  tensor[2][0] = D[2]; tensor[2][1] = D[4]; tensor[2][2] = D[5];

  //std::cout << tensor << std::endl;
  
  return TENSOR_RECON_SUCCESS;
}

void ComputeTensorQuantities(
  const vnl_matrix_fixed<double, 3, 3> & tensor,   // input tensor
  double & FA,                                     // return
                                                   // fractional
                                                   // anisotropy
  VectorType & EigenValues,       // return eigenvalues in descending
                                  // order
  VectorType & PVec1,             // return primary direction
  VectorType & PVec2,             // return secondary direction
  VectorType & PVec3)             // return least important direction
{

  vnl_svd<double> svdTensor(tensor);

  PVec1 = svdTensor.U().get_column(0);
  PVec2 = svdTensor.U().get_column(1);
  PVec3 = svdTensor.U().get_column(2);

  EigenValues = svdTensor.W().diagonal();

  double meanEvalue = (EigenValues[0] + EigenValues[1] + EigenValues[2])/3;
  if (meanEvalue > 0.2)
    {
    FA = dot_product(EigenValues, EigenValues);
    EigenValues -= meanEvalue;
    FA = dot_product(EigenValues, EigenValues)/FA;
    FA = sqrt(FA*1.5);
    EigenValues += meanEvalue;
    }
  else
    {
    FA = 0;
    }
  return;
}
