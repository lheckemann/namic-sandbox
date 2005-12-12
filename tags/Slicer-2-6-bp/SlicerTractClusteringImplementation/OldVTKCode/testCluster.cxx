//
// testCluster.cxx: Test code for vtkNormalizedCuts.
// 
// Creates ideal sample input and runs clustering.
// Produces 3 text files:
//   inputClusters.txt   (input cluster indices)
//   embed.txt           (embedding vectors)
//   outputClusters.txt  (output cluster indices)
// Also outputs the normalized weight matrix as an image.
//
// (There is one bug, I can't find the image output on windows.
// Apparently it is not written to the directory where I run the file.)
//

#include <iostream>
#include "vtkNormalizedCuts.h"
#include "vnl/vnl_matrix.h"

// for outputting images
#include "vtkPNGWriter.h"
#include "vtkImageCast.h"
#include "vtkImageMathematics.h"

int main(int argc, char* argv[])
{

  if(argc == 0)
    {
      std::cout << "Usage: testCluster <infile> <outfile> <# Clusters>" << std::endl;
      return 0;
    }

  // create input test data
  // -----------------------

  // number of things to cluster
  int n = 21;

  // matrix W (for weights)
  vnl_matrix<double> W(n,n,0); 
  
  // now make a simulated perfect matrix, block diagonal
  // within-cluster similarity = 1 and all other entries = 0;
  int numberOfClusters = 3;
  int clusterSize = n/numberOfClusters;
  int clusterIdx = 1;
  ofstream fileInputClusters;
  fileInputClusters.open("inputClusters.txt");

  for (int row = 0; row < W.rows(); row++)
    {
      if (row == clusterIdx*clusterSize)
        clusterIdx++;

      // output cluster indices to disk
      fileInputClusters << clusterIdx << endl;

      for (int col = 0; col < W.cols(); col++)
        {
          if (col < clusterIdx*clusterSize)
            {
              W[row][col] = 1;
            }
        }
    }
  fileInputClusters.close();



  // Now create the cluster-er and set its input
  // -----------------------
  vtkNormalizedCuts *spectralCluster = vtkNormalizedCuts::New();
  spectralCluster->SetInputWeightMatrix(&W);
  spectralCluster->SetNumberOfClusters(numberOfClusters);
  // For debug/test, this outputs the embedding vectors in a file (embed.txt)
  spectralCluster->SaveEmbeddingVectorsOn();
  spectralCluster->ComputeClusters();



  // Now print/save the input and output
  // -----------------------
  // print cluster numbers
  vtkNormalizedCuts::OutputClassifierType::OutputType * membershipSample =  
    spectralCluster->GetOutputClassifier()->GetOutput();
  if (membershipSample == NULL)
    {
      cerr << "Error: clusters have not been computed." << endl;
      return 1;      
    }
  // Iterate over all class labels and save the info
  vtkNormalizedCuts::OutputClassifierType::OutputType::ConstIterator iter = 
    membershipSample->Begin();

  ofstream fileOutputClusters;
  fileOutputClusters.open("outputClusters.txt");

  int idx = 0;
  while ( iter != membershipSample->End() )
    {
      cout <<"index = " << idx << "class label = " << iter.GetClassLabel() << endl;
      fileOutputClusters << iter.GetClassLabel() << endl;
      idx++;
      ++iter;
    }
  fileOutputClusters.close();

  // Write normalized matrix to disk as an image
  vtkImageMathematics *scale = vtkImageMathematics::New();
  scale->SetOperationToMultiplyByK();
  scale->SetConstantK(100);
  scale->SetInput1(spectralCluster->GetNormalizedWeightMatrixImage());
  vtkImageCast *cast = vtkImageCast::New();
  cast->SetOutputScalarTypeToUnsignedChar();
  cast->SetInput(scale->GetOutput());
  vtkPNGWriter *writer = vtkPNGWriter::New();
  writer->SetInput(cast->GetOutput());
  writer->SetFileName("NormalizedWeightMatrix.png");
  writer->Write();
  writer->Delete();
  cast->Delete();
  scale->Delete();

  spectralCluster->Delete();

  return 0;
}
