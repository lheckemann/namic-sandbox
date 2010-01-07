/*
  GAC and shape based segmentation. The feature image is given from a file.
*/

#include "ProstateSegSBCLP.h"


#include <iostream>
#include <string>

//douher
#include "lib/cArrayOp.h"
#include "lib/cArray3D.h"
#include "lib/txtIO.h"


#include "shapeBasedGAC.h"
#include "shapeBasedSeg.h"
#include <ctime>
#include <string>

#include "itkOrientedImage.h"

douher::cArray3D< double >::Pointer connection(douher::cArray3D< double >::Pointer phi)
{
  douher::cArray3D< double >::Pointer bin(new douher::cArray3D< double >(phi));

  long nx = phi->getSizeX();
  long ny = phi->getSizeY();
  long nz = phi->getSizeZ();
  
  long n = nx*ny*nz;

  for (long i = 0; i < n; ++i)
    {
      (*bin)[i] = (*bin)[i]<=0?1:0; // use 2.5, instead of 0, to enclose more
    }

  return bin;
}







// Description:
// This function gives progress feedback to slicer.
//  percentComplete=0 means 0% complete
//  percentComplete=100 means 100% complete
void progress_callback(int percentComplete)
{
  /*
  // TODO: report these information as well to Slicer
        std::cout << "<filter-start>"
                  << std::endl;
        std::cout << "<filter-name>"
                  << (this->Watcher->GetProcess()
                      ? this->Watcher->GetProcess()->GetClassName() : "None")
                  << "</filter-name>"
                  << std::endl;
        std::cout << "<filter-comment>"
                  << " \"" << this->Watcher->GetComment() << "\" "
                  << "</filter-comment>"
                  << std::endl;
        std::cout << "</filter-start>"
                  << std::endl;
        std::cout << std::flush;
  */
  std::cout << "<filter-progress>" << double(percentComplete)/100.0 << "</filter-progress>" << std::endl;
}

int main( int argc, char * argv [] )
{
   PARSE_ARGS; 
   
  std::vector< std::string > shapeModelFileList = douher::readTextLineToListOfString(shapeModelFileName.c_str());
  int numOfEigenShapes=shapeModelFileList.size()-1; // the first file is the mean shape, followed by eigen shapes
  std::cout << "number of eigen shapes=" << numOfEigenShapes << std::endl;

  std::string modelPath = itksys::SystemTools::GetFilenamePath(shapeModelFileName.c_str()) + std::string("/");
  std::cout << "modelPath=" << modelPath.c_str() << std::endl;

  std::string meanShapeName = modelPath + shapeModelFileList[0];
  std::cout << "meanShapeName=" << meanShapeName.c_str() << std::endl;

  std::cout << std::flush;

  const unsigned int Dimension = 3;

  typedef unsigned char InternalPixelType;  
  typedef itk::OrientedImage< InternalPixelType, Dimension >  InternalImageType;

  typedef unsigned char OutputPixelType;
  typedef itk::OrientedImage< OutputPixelType, Dimension > OutputImageType;

//  typedef itk::CastImageFilter< InternalImageType, OutputImageType > CastingFilterType;
//  CastingFilterType::Pointer caster = CastingFilterType::New();
                        
  typedef itk::ImageFileReader< InternalImageType > ReaderType;
  typedef itk::ImageFileWriter<  OutputImageType  > WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( inputImageFileName.c_str() );
  reader->Update(); 

InternalImageType::IndexType leftPointIJK;
InternalImageType::IndexType rightPointIJK;

  if (prostate_side_points.size() == 2)
    {
    InternalImageType::PointType rasPoint;

    rasPoint[0] = prostate_side_points[1][0];  
    rasPoint[1] = prostate_side_points[1][1];
    rasPoint[2] = prostate_side_points[1][2];
    reader->GetOutput()->TransformPhysicalPointToIndex(rasPoint, leftPointIJK);

    rasPoint[0] = prostate_side_points[0][0];  
    rasPoint[1] = prostate_side_points[0][1];
    rasPoint[2] = prostate_side_points[0][2];
    reader->GetOutput()->TransformPhysicalPointToIndex(rasPoint, rightPointIJK);
    }
  else
    {
    std::cerr << "Exactly 2 seed points shall be defined." << std::endl;
    return -1;
    }

  std::cout << "Left point IJK: " << leftPointIJK << std::endl;
  std::cout << "Left point IJK: " << rightPointIJK << std::endl;

  //////
  double t1, t2;
  t1 = clock();

  std::cout<<"reading input volume...."<<std::flush;  


  typedef double PixelType;
  typedef douher::cArray3D< PixelType > ImageType;
  typedef douher::cArray3D< unsigned char > MaskImageType;

  // 1. GAC
  ImageType::Pointer img = douher::readImageToArray3< PixelType >(inputImageFileName.c_str());

  //  ImageType::Pointer featureImg = douher::readImageToArray3< PixelType >("feature.nrrd");

  CShapeBasedGAC::Pointer gac = CShapeBasedGAC::New();
  gac->setImage(img);
  gac->setLeftAndRightPointsInTheMiddleSlice(
    leftPointIJK[0],leftPointIJK[1],leftPointIJK[2],
    rightPointIJK[0],rightPointIJK[1],rightPointIJK[2]);

  // mean shape
  gac->setMeanShape(douher::readImageToArray3< double >( meanShapeName.c_str() ) );

  std::cout<<"done\n";


  std::cout<<"doShapeBasedGACSegmenation...."<<std::flush;  

  long numiter = 10;
  gac->setNumIter(numiter);

  // do segmentation
  gac->doShapeBasedGACSegmenation();

  //debug//
  douher::saveAsImage3< double >(gac->mp_phi, "gacResult.nrrd");
  //DEBUG//

  //debug//
  douher::saveAsImage3< double >(connection(gac->mp_phi), "gacResultBin.nrrd");
  //DEBUG//

  std::cout<<"done\n";

  std::cout<<"Read shape model...."<<std::flush;  

  // 2. Tsai's shape based
  CShapeBasedSeg c;
  c.setInputImage(douher::cArray3ToItkImage< double >(connection(gac->mp_phi)) );

  c.setMeanShape(douher::readImage3< double >( meanShapeName.c_str() ));

  // eigen shapes
  long numEigen = 8;
  if (numEigen > numOfEigenShapes)
    {
      numEigen = numOfEigenShapes;
    }

  std::vector< douher::cArray3D< double >::Pointer > eigenShapes;

  for (int i = 0; i < numEigen; ++i) 
    {
      std::string filename=modelPath + shapeModelFileList[i+1]; // start from 1, the 0th file is the mean shape
      std::cout<<"Add eigen shape: "<<filename.c_str()<<"\n";
      c.addEigenShape(douher::readImage3< double >( filename.c_str() ));
    }

//   for (int i = 0; i < numEigen; ++i)
//     {
//       c.addEigenShape( douher::cArray3ToItkImage< double >(gac->m_eigenShapes[i]) );
//     }


  //debug//
  //  c.setMaxIteration(0);
  //DEBUG//

  std::cout<<"done\n";


  std::cout<<"compute Tsai's shape based...."<<std::flush;   

  c.gogogo();

  std::cout<<"current param = "<<c.getParameters()<<std::endl;

  std::cout<<"current cost = "<<c.getCost()<<std::endl;

  std::cout<<"done\n";


  std::cout<<"write output image to file...."<<std::flush;   

  typedef CShapeBasedSeg::DoubleImageType DoubleImageType;
  DoubleImageType::Pointer segRslt = c.getSegmentation();

  // Only voxel values are correct, image metadata (origin, spacing, orientation) is not
  // set correctly for the output volume. Copy that from the input volume.
  // :TODO: fix orientation as well
  segRslt->SetOrigin(reader->GetOutput()->GetOrigin());
  segRslt->SetSpacing(reader->GetOutput()->GetSpacing());

  douher::writeImage3< double >(segRslt, segmentedImageFileName.c_str());

  std::cout<<"done\n";


  t2 = clock();
  t1 = (t2 - t1)/CLOCKS_PER_SEC;
  std::cout<<"New Elapsed time = "<<t1<<std::endl<<std::flush;
  // end timing
  ////////////////////////////////////////////////////////////

  return EXIT_SUCCESS ;
}
