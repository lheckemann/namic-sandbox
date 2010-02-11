/*================================================================================
  This code read in some images and do pca. Output *double* mean and
  eigen images, for segmentation use
  
  input: 
        file containing all binary files
        int iEigen

  output:
         (iEigen-th eigen image)*sqrt(iEigen-th eigen value)

  Yi Gao
  20090716
*/

#include "imagePCAAnalysis.h"

#include <vector>
#include <string>
#include <fstream>

//douher
#include "cArrayOp.h"
//#include "aux/txtIO.h"
#include "lib/txtIO.h"

// itk
#include "itkImage.h"
#include "itkImageRegionIterator.h"




int main( int argc, char * argv[] )
{
  if (argc != 3)
    {
      std::cerr<<"Usage: "<<argv[0]<<" fileList.txt ith\n";
      exit(-1);
    }

  char* fileContainingFileNames = argv[1];
  int ith = atoi(argv[2]);


  // read the image file names
  std::vector< std::string > listOfLines = douher::readTextLineToListOfString<char>(fileContainingFileNames);


  typedef double pixel_t;
  typedef itk::Image<pixel_t, 3> image_t;
  typedef image_t::Pointer imagePointer_t;


  std::vector<imagePointer_t> inputImageSeries = douher::readImageSeries3<pixel_t>(listOfLines);

  int n = listOfLines.size();
  printf("Has %d files.\n", n);

  if (ith > n)
    {
      // check in advance so don't have to compute in vain
      std::cerr<<"requested eigen image larger than # of images. abort\n";
      raise(SIGABRT);
    }



  typedef douher::CImagePCAAnalysis3D<pixel_t> CImagePCAAnalysis3D_t;
  CImagePCAAnalysis3D_t imageAnalizer;


  // feed
  imageAnalizer.setImageSeries(inputImageSeries);
  // do pca
  imageAnalizer.gogogo();

  
  // output ith eigen image
  imagePointer_t eigenImg = imageAnalizer.getEigenImage(ith);
  
  // multiply sqrt(ith eigen value)
  // double ev = sqrt(imageAnalizer.getEigenValue(ith));

  typedef itk::ImageRegionIterator< image_t > imageRegionIterator_t;
  imageRegionIterator_t eigenImIter(eigenImg, eigenImg->GetLargestPossibleRegion());
  for (eigenImIter.GoToBegin(); !eigenImIter.IsAtEnd(); ++eigenImIter)
    {
      double a = eigenImIter.Get();
      //      eigenImIter.Set(ev*a);
      eigenImIter.Set(a);
    }
  

  // output mean
  imagePointer_t meanImg = imageAnalizer.getMeanImage();
  douher::writeImage3< double >( meanImg, "meanImage.nrrd" );


  // output eigen
  char name[500];
  sprintf(name, "eigenImg_%d.nrrd", ith);
  douher::writeImage3< pixel_t >( eigenImg, name );

  // output all eigen values
  std::ofstream eigenValueFile("eigenValueFile.txt");
  eigenValueFile<<imageAnalizer.m_eigenValues<<std::endl;
  eigenValueFile.close();


  return EXIT_SUCCESS;
}
