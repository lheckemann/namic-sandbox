/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkShapeLabelObjectAccessorsTest1.cxx,v $
  Language:  C++
  Date:      $Date: 2009-08-07 15:16:20 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkLabelObject.h"
#include "itkShapeLabelObject.h"
#include "itkShapeLabelObjectAccessors.h"
#include "itkLabelMap.h"

#include "itkShapeKeepNObjectsLabelMapFilter.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"


int main(int argc, char * argv[])
{
  if( argc != 2 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " input ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int dim = 3;
 
  typedef unsigned char                           PixelType;
  typedef itk::Image< PixelType, dim >            ImageType;
  typedef itk::ShapeLabelObject< PixelType, dim > ShapeLabelObjectType;
  typedef itk::LabelMap< ShapeLabelObjectType >   LabelMapType;
  typedef itk::ImageFileReader< ImageType >       ReaderType;

  // Exercise the attribute translation code and verify that
  // translations are correct
  int status = EXIT_SUCCESS;

  std::vector<std::string> attributes;
  attributes.push_back("Label");
  attributes.push_back("Size");
  attributes.push_back("PhysicalSize");
  attributes.push_back("RegionElongation");
  attributes.push_back("SizeRegionRatio");
  attributes.push_back("Centroid");
  attributes.push_back("Region");
  attributes.push_back("SizeOnBorder");
  attributes.push_back("PhysicalSizeOnBorder");
  attributes.push_back("FeretDiameter");
  attributes.push_back("BinaryPrincipalMoments");
  attributes.push_back("BinaryPrincipalAxes");
  attributes.push_back("BinaryElongation");
  attributes.push_back("Perimeter");
  attributes.push_back("Roundness");
  attributes.push_back("EquivalentRadius");
  attributes.push_back("EquivalentPerimeter");
  attributes.push_back("EquivalentEllipsoidSize");
  attributes.push_back("BinaryFlatness");
  for (size_t a = 0; a < attributes.size(); a++)
    {
    if (ShapeLabelObjectType::GetNameFromAttribute(ShapeLabelObjectType::GetAttributeFromName(attributes[a])) != attributes[a])
      {
      std::cout << "Attribute translation for " << attributes[a] << " failed." << std::endl;
      std::cout << "   Received " << ShapeLabelObjectType::GetNameFromAttribute(ShapeLabelObjectType::GetAttributeFromName(attributes[a])) << " but expected " << attributes[a] << std::endl; 
      status = EXIT_FAILURE;
      }
    }
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
 
  typedef itk::LabelImageToShapeLabelMapFilter< ImageType, LabelMapType> I2LType;
  I2LType::Pointer i2l = I2LType::New();
  i2l->SetInput( reader->GetOutput() );
  i2l->SetComputePerimeter(true);
  i2l->Update();

  typedef LabelMapType::LabelObjectContainerType LabelObjectContainerType;
  LabelMapType *labelMap = i2l->GetOutput();
  LabelObjectContainerType container = labelMap->GetLabelObjectContainer();
  std::cout << "File " << argv[1] << " has " << labelMap->GetNumberOfLabelObjects() << " labels." << std::endl;
 
  // Retrieve all attributes
  for (unsigned int n = 0; n < labelMap->GetNumberOfLabelObjects(); n++)
    {
    ShapeLabelObjectType *labelObject = labelMap->GetNthLabelObject(n);
    std::cout << "Label: " 
              << itk::NumericTraits<LabelMapType::LabelType>::PrintType(labelObject->GetLabel()) << std::endl;
//    std::cout << "    Region: "
//              << labelObject->GetRegion() << std::endl;
    std::cout << "    Size in pixels: "
              << labelObject->GetSize() << std::endl;
    std::cout << "    Physical Size: "
              << labelObject->GetPhysicalSize() << std::endl;
//    std::cout << "    Centroid center of shape: "
//              << labelObject->GetCentroid() << std::endl;
//    std::cout << "    RegionElongation : "
//              << labelObject->GetRegionElongation() << std::endl;
//    std::cout << "    SizeRegionRatio: "
//              << labelObject->GetSizeRegionRatio() << std::endl;
//    std::cout << "    SizeOnBorder: "
//              << labelObject->GetSizeOnBorder() << std::endl;
//    std::cout << "    PhysicalSizeOnBorder: "
//              << labelObject->GetPhysicalSizeOnBorder() << std::endl;
//    std::cout << "    FeretDiameter: "
//              << labelObject->GetFeretDiameter() << std::endl;
//    std::cout << "    BinaryPrincipalMoments: "
//              << labelObject->GetBinaryPrincipalMoments() << std::endl;
//    std::cout << "    BinaryPrincipalAxes: "
//              << labelObject->GetBinaryPrincipalAxes() << std::endl;
//    std::cout << "    BinaryElongation: "
//              << labelObject->GetBinaryElongation() << std::endl;
    std::cout << "    Perimeter: "
              << labelObject->GetPerimeter() << std::endl;
    std::cout << "    Roundness: "
              << labelObject->GetRoundness() << std::endl;
//    std::cout << "    EquivalentRadius: "
//              << labelObject->GetEquivalentRadius() << std::endl;
//    std::cout << "    EquivalentPerimeter: "
//              << labelObject->GetEquivalentPerimeter() << std::endl;
//    std::cout << "    EquivalentEllipsoidSize: "
//              << labelObject->GetEquivalentEllipsoidSize() << std::endl;
//    std::cout << "    BinaryFlatness: "
//              << labelObject->GetBinaryFlatness() << std::endl;
    }
  return status;
}
