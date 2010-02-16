/*
  GAC and shape based segmentation. The feature image is given from a file.
*/

#include <iostream>
#include <string>
#include "itkBinaryThresholdImageFilter.h"
#include "itkCastImageFilter.h"

#include "arrays/cArrayOp.h"
#include "arrays/cArray3D.h"
#include "txtio/txtIO.h"
#include "shapeBasedGAC.h"
#include "shapeBasedSeg.h"


int main(int argc, char** argv)
{
  /* Assume the mean and eigen shapes are stored in the _seg/shapeSpaceZflip/ */

  if (argc != 9)
    {
      std::cerr<<"Parameters: inputImage outputImage lx ly lz rx ry rz\n";
      exit(-1);
    }

  const char* inputImage = argv[1];  
  const char* outputImage = argv[2];

  const char* meanShapeName = "../shapeSpaceZflip/meanImage.nrrd";
  const char* eigenList = "../shapeSpaceZflip/eigenShapeWithStdList.txt";


  long lx = atol(argv[3]);
  long ly = atol(argv[4]);
  long lz = atol(argv[5]);

  long rx = atol(argv[6]);
  long ry = atol(argv[7]);
  long rz = atol(argv[8]);


  typedef double PixelType;
  typedef douher::cArray3D< PixelType > ImageType;
  typedef douher::cArray3D< unsigned char > MaskImageType;

  // 1. GAC

  typedef itk::Image<PixelType, 3> InputImageType;
  InputImageType::Pointer inputImagePtr;
  typedef itk::ImageFileReader< InputImageType > InputImageReaderType;
  InputImageReaderType::Pointer reader = InputImageReaderType::New();
  reader->SetFileName(inputImage);
  try
    {
      reader->Update();
      inputImagePtr = reader->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cerr<< "ExceptionObject caught !" << std::endl; 
      std::cerr<< err << std::endl; 
      raise(SIGABRT);
    }

  CShapeBasedGAC::Pointer gac = CShapeBasedGAC::New();
  gac->setImage(inputImagePtr);
  gac->setLeftAndRightPointsInTheMiddleSlice(lx, ly, lz, rx, ry, rz);

  // mean shape
  gac->setMeanShape(douher::readImageToArray3< double >( meanShapeName ) );

  long numiter = 10;
  gac->setNumIter(numiter);

  // do segmentation
  gac->doShapeBasedGACSegmenation();

  //debug//
  typedef CShapeBasedGAC::LSImageType floatImage_t;
  typedef itk::ImageFileWriter< floatImage_t > writer_t;
  writer_t::Pointer outputWriter = writer_t::New();
  outputWriter->SetFileName("gacResult.nrrd");
  outputWriter->SetInput(gac->mp_phi);
  outputWriter->Update();

  typedef itk::BinaryThresholdImageFilter<CShapeBasedGAC::LSImageType, CShapeBasedGAC::LSImageType>  FilterType;
  FilterType::Pointer filter = FilterType::New();
  outputWriter->SetInput( filter->GetOutput() );
  filter->SetInput(gac->mp_phi);
  filter->SetOutsideValue(0);
  filter->SetInsideValue(1);
  filter->SetLowerThreshold(-1e6);
  filter->SetUpperThreshold(0);
  filter->Update();
  outputWriter->SetFileName("gacResultBin.nrrd");
  outputWriter->Update(); 
  //DEBUG//


  // 2. Tsai's shape based
  CShapeBasedSeg c;
  typedef itk::CastImageFilter< CShapeBasedGAC::LSImageType, CShapeBasedSeg::DoubleImageType> CastFromLSToDoubleFilterType;
  CastFromLSToDoubleFilterType::Pointer toDouble = CastFromLSToDoubleFilterType::New(); 
  toDouble->SetInput(filter->GetOutput());
  
  c.setInputImage(toDouble->GetOutput());  

  c.setMeanShape(douher::readImage3< double >( meanShapeName ));


  // eigen shapes
  long numEigen = 8;
  std::vector< std::string > nameOfShapes = douher::readTextLineToListOfString(eigenList);

  if (numEigen > (long)nameOfShapes.size())
    {
      numEigen = nameOfShapes.size();
    }

  std::vector< douher::cArray3D< double >::Pointer > eigenShapes;

  for (int i = 0; i < numEigen; ++i)
    {
      c.addEigenShape(douher::readImage3< double >( nameOfShapes[i].c_str() ));
    }

//   for (int i = 0; i < numEigen; ++i)
//     {
//       c.addEigenShape( douher::cArray3ToItkImage< double >(gac->m_eigenShapes[i]) );
//     }


  //debug//
  //  c.setMaxIteration(0);
  //DEBUG//

  c.gogogo();

  std::cout<<"current param = "<<c.getParameters()<<std::endl;
  std::cout<<"current cost = "<<c.getCost()<<std::endl;

  typedef CShapeBasedSeg::DoubleImageType DoubleImageType;
  DoubleImageType::Pointer segRslt = c.getSegmentation();

  douher::writeImage3< double >(segRslt, outputImage);

  return 0;
}
