
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <itkTransform.h>
#include <itkTransformFileReader.h>
#include <itkImageFileWriter.h>
#include <itkBSplineDeformableTransform.h>
#include <itkImageFileReader.h>
#include <itkImageRegionIterator.h>
#include <itkIterativeInverseDeformationFieldImageFilter.h>
using namespace std;   

int main( int argc , char * argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputTransform.tfm referenceImage outputDeformationField "<< std::endl;
    return EXIT_FAILURE;
    }

   const int Dimension = 3;

   ///////////read BSpline Transform
   typedef itk::TransformFileReader::Pointer TransformReaderPointer ;
   TransformReaderPointer transformFile = itk::TransformFileReader::New() ;
   transformFile->SetFileName( argv[1] ) ;
   transformFile->Update() ;
   std::cerr << "Find "<< transformFile->GetTransformList() ->size() << " transforms from file" << std::endl;
   transformFile->GetTransformList()->pop_front();
   typedef itk::BSplineDeformableTransform< double, Dimension , Dimension > BSplineDeformableTransformType;
   BSplineDeformableTransformType::Pointer BSplineTransform = BSplineDeformableTransformType::New();  
   if ( transformFile->GetTransformList()->size() == 0  ) 
   { 
     std::cerr << "Error, the second transform does not exist." << std::endl;
     return EXIT_FAILURE;
   }
   if (!dynamic_cast<BSplineDeformableTransformType* > ( transformFile->GetTransformList()->front().GetPointer()))
   {       
      std::cerr << "Error, the second transform is not BSpline." << std::endl;
      return EXIT_FAILURE;
   }
   BSplineTransform = dynamic_cast<BSplineDeformableTransformType* > ( transformFile->GetTransformList()->front().GetPointer());
   std::cerr << "Read BSpline transform successful." << std::endl;
   
   /////read reference volume
   typedef   short    PixelType;
   typedef itk::Image< PixelType, Dimension >   ReferenceImageType;
   typedef itk::ImageFileReader< ReferenceImageType >  ReferenceImageReaderType;
   ReferenceImageReaderType::Pointer imagereader =  ReferenceImageReaderType::New();
   imagereader->SetFileName( argv[2] );
   imagereader->Update();

   /////allocate space for deformation field
   typedef itk::Vector< float, Dimension >  VectorType; 
   typedef itk::Image< VectorType, Dimension >  DeformationFieldType; 
   DeformationFieldType::Pointer defmationField = DeformationFieldType::New(); //Filter L's kernnel form tile to same size as the input image
   DeformationFieldType::SizeType size = imagereader->GetOutput()->GetBufferedRegion().GetSize();
   defmationField->SetRegions( size );
   defmationField->SetOrigin( imagereader->GetOutput()->GetOrigin() );
   defmationField->SetSpacing( imagereader->GetOutput()->GetSpacing() );
   defmationField->Allocate(); 

   ///convert to dense field
   typedef itk::ImageRegionIterator< DeformationFieldType > DeformationFieldIteratorType;
   DeformationFieldIteratorType fieldit( defmationField, defmationField->GetRequestedRegion() );
   
   std::cerr << "Converting BSpline transform to dense field ... " << std::endl;
   long temp = 0;
   for (fieldit.GoToBegin(); !fieldit.IsAtEnd(); ++ fieldit)
   {
       DeformationFieldType::IndexType currentPix = fieldit.GetIndex();
       BSplineDeformableTransformType::InputPointType  fixedPoint;
       BSplineDeformableTransformType::OutputPointType movingPoint;
       defmationField->TransformIndexToPhysicalPoint( currentPix, fixedPoint );
       movingPoint = BSplineTransform->TransformPoint( fixedPoint );
       VectorType displacement;
       displacement = movingPoint - fixedPoint;
       fieldit.Set(displacement);

       if (temp%(size[0]*size[1]*size[2]/10) == 0 )
       {
         cout << float(temp*100)/(size[0]*size[1]*size[2])<<"%" <<endl;         
       }
       ++temp;
   }
   std::cerr << "Converting BSpline transform to dense field successful. " << std::endl;

   typedef itk::IterativeInverseDeformationFieldImageFilter<DeformationFieldType, DeformationFieldType> DeformationInverterType;
   DeformationInverterType::Pointer deformationinvert = DeformationInverterType::New();
   deformationinvert->SetNumberOfIterations(100);
   deformationinvert->SetStopValue(0.5);
   deformationinvert->SetInput(defmationField);
   deformationinvert->Update();

   std::cerr << "Invert dense field done. " << std::endl;

   typedef itk::ImageFileWriter< DeformationFieldType > DeformationFieldWriterType;
   DeformationFieldWriterType::Pointer  deformationFieldWriter =  DeformationFieldWriterType::New();
   deformationFieldWriter->SetFileName( argv[3] );
   deformationFieldWriter->SetInput( deformationinvert -> GetOutput());
   deformationFieldWriter->Update();
}
