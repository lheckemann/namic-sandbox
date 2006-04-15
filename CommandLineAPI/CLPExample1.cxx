#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "GenerateCLP.h"

int main (int argc, char *argv[])
{
#include "CLPExample1.clp"
  std::cout << "HistogramBins: " << HistogramBins << std::endl;
  std::cout << "SpatialSamples: " << SpatialSamples << std::endl;
  std::cout << "InitializeTransform: " << InitializeTransform << std::endl;
  std::cout << "Iterations: ";
  for (unsigned int i =0; i < Iterations.size(); i++)
    {
    std::cout << Iterations[i] << ", ";
    }
  std::cout << std::endl;
  std::cout << "LearningRate: ";
  for (unsigned int i =0; i < LearningRate.size(); i++)
    {
    std::cout << LearningRate[i] << ", ";
    }
  std::cout << std::endl;
  std::cout << "TranslationScale: " << TranslationScale << std::endl;
  std::cout << "xmlSwitch: " << xmlSwitch << std::endl;
  std::cout << "FixedImage: " << FixedImage << std::endl;
  std::cout << "MovingImage: " << MovingImage << std::endl;
  std::cout << "OutputImage: " << OutputImage << std::endl;

  return 0;
}
