#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageLinearConstIteratorWithIndex.h"
#include "itkImageLinearIteratorWithIndex.h"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_file_matrix.h>
#include <vnl/vnl_vector.h>
#include "itkTimeProbesCollectorBase.h"
#include "itkPermuteAxesImageFilter.h"

typedef vnl_matrix< float >    TMatrix;
typedef vnl_vector< float >    TVector;
typedef vnl_file_matrix<float>  VNLFile;

int main( int argc, char * argv [] )
{
  if( argc < 4 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " input_filename recon_matrix output_filename mask_filename" << std::endl;
    return EXIT_FAILURE;
  }

  itk::TimeProbesCollectorBase chronometer;


    const unsigned int ImageDimension = 4;

  typedef short DWIPixelType;
  typedef itk::Image< DWIPixelType, ImageDimension > DWIImageType;

    typedef float ODFPixelType;
    typedef itk::Image< ODFPixelType, ImageDimension > ODFImageType;

    typedef float MaskPixelType;
    typedef itk::Image< MaskPixelType, 3 > MaskImageType;

    typedef itk::ImageFileReader< DWIImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
  {
    reader->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  typedef itk::ImageFileReader< MaskImageType > MaskReaderType;
  MaskReaderType::Pointer mask_reader = MaskReaderType::New();
  mask_reader->SetFileName( argv[4] );
  try
  {
    mask_reader->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  char        *btable_filename  = argv[2];
    VNLFile     BTable(btable_filename);
    std::cout << "Rows: " << BTable.rows() << vcl_endl;
    std::cout << "Cols: " << BTable.cols() << vcl_endl;

  DWIImageType::ConstPointer dwi_image = reader->GetOutput();
  typedef DWIImageType::IndexType    DWIIndexType;
    typedef DWIImageType::SizeType     DWISizeType;
    typedef DWIImageType::RegionType   DWIRegionType;
    typedef DWIImageType::SpacingType  DWISpacingType;
    typedef DWIImageType::PointType    DWIOriginType;

  typedef itk::PermuteAxesImageFilter< DWIImageType > PermuteFilterType;
  PermuteFilterType::Pointer permuter = PermuteFilterType::New();

  PermuteFilterType::PermuteOrderArrayType  permuteOrder;

  permuteOrder[0] = 3;
  permuteOrder[1] = 0;
  permuteOrder[2] = 1;
  permuteOrder[3] = 2;

  permuter->SetInput( dwi_image );

  permuter->SetOrder( permuteOrder );

  chronometer.Start("Permute1");
  permuter->Update();
  chronometer.Stop("Permute1");

  chronometer.Report( std::cout );

  DWIImageType::ConstPointer dwi_image_t = permuter->GetOutput();


    DWISizeType        dwi_size    = dwi_image->GetLargestPossibleRegion().GetSize();
    DWISpacingType     dwi_spacing = dwi_image->GetSpacing();
    DWIOriginType      dwi_origin  = dwi_image->GetOrigin();
  DWIRegionType      dwi_region  = dwi_image->GetBufferedRegion();
  DWIIndexType       dwi_index   = dwi_region.GetIndex();

/*  std::cout << "DWI Origin: ";
  std::cout << dwi_origin[0] << ", " << dwi_origin[1] << ", " << dwi_origin[2] << ", " << dwi_origin[2] << std::endl;
  std::cout << "DWI Space: ";
  std::cout << dwi_spacing[0] << ", " << dwi_spacing[1] << ", " << dwi_spacing[2] << ", " << dwi_spacing[3] << std::endl;
  std::cout << "DWI Size: ";
  std::cout << dwi_size[0] << ", " << dwi_size[1] << ", " << dwi_size[2] << ", " << dwi_size[3] << std::endl;
  std::cout << "DWI Index: ";
  std::cout << dwi_index[0] << ", " << dwi_index[1] << ", " << dwi_index[2] << ", " << dwi_index[3] << std::endl;
*/
  ODFImageType::Pointer odf_image = ODFImageType::New();
  typedef ODFImageType::IndexType    ODFIndexType;
  typedef ODFImageType::SizeType     ODFSizeType;
  typedef ODFImageType::RegionType   ODFRegionType;
  typedef ODFImageType::SpacingType  ODFSpacingType;
  typedef ODFImageType::PointType    ODFOriginType;

  ODFIndexType       odf_index;
  ODFSizeType        odf_size;
  ODFSpacingType     odf_spacing;
  ODFOriginType      odf_origin;

  for( unsigned int i = 0; i < 4; i++)
  {
    odf_size[i]    = dwi_size[i];
    odf_index[i]   = dwi_index[i];
    odf_spacing[i] = dwi_spacing[i];
    if(i == 3)
      continue;
    else
      odf_origin[i]  = dwi_origin[i];
  }

  odf_size[3]    = BTable.cols();

/*  std::cout << "ODF Origin: ";
  std::cout << odf_origin[0] << ", " << odf_origin[1] << ", " << odf_origin[2] << ", " << odf_origin[2] << std::endl;
  std::cout << "ODF Space: ";
  std::cout << odf_spacing[0] << ", " << odf_spacing[1] << ", " << odf_spacing[2] << ", " << odf_spacing[3] << std::endl;
  std::cout << "ODF Size: ";
  std::cout << odf_size[0] << ", " << odf_size[1] << ", " << odf_size[2] << ", " << odf_size[3] << std::endl;
  std::cout << "ODF Index: ";
  std::cout << odf_index[0] << ", " << odf_index[1] << ", " << odf_index[2] << ", " << odf_index[3] << std::endl;
*/
  odf_image->SetSpacing( odf_spacing );
  odf_image->SetOrigin(  odf_origin  );

  ODFRegionType odf_region;
  odf_region.SetIndex( odf_index );
  odf_region.SetSize( odf_size );

  odf_image->SetRegions( odf_region  );
  odf_image->Allocate();
  odf_image->FillBuffer(0.0);

    TVector voxel_dwi(BTable.rows(), 0.0), voxel_odf(BTable.cols(), 0.0), dummy;

    typedef itk::ImageLinearConstIteratorWithIndex< DWIImageType > DWIIteratorType;
    DWIIteratorType dwi_it( dwi_image, dwi_region );
    dwi_it.SetDirection( 3 );

  typedef itk::ImageLinearIteratorWithIndex< ODFImageType > ODFIteratorType;
    ODFIteratorType odf_it( odf_image, odf_region );
  odf_it.SetDirection( 0 );

  MaskImageType::ConstPointer mask_image = mask_reader->GetOutput();
  typedef MaskImageType::IndexType    MaskIndexType;
    typedef MaskImageType::SizeType     MaskSizeType;
    typedef MaskImageType::RegionType   MaskRegionType;
    typedef MaskImageType::SpacingType  MaskSpacingType;
    typedef MaskImageType::PointType    MaskOriginType;

    MaskSizeType        mask_size    = mask_image->GetLargestPossibleRegion().GetSize();
    MaskSpacingType     mask_spacing = mask_image->GetSpacing();
    MaskOriginType      mask_origin  = mask_image->GetOrigin();
  MaskRegionType      mask_region  = mask_image->GetBufferedRegion();
  MaskIndexType       mask_index   = mask_region.GetIndex();

  typedef itk::ImageLinearConstIteratorWithIndex< MaskImageType > MaskIteratorType;
    MaskIteratorType mask_it( mask_image, mask_region );

/*  std::cout << "Mask Origin: ";
  std::cout << mask_origin[0] << ", " << mask_origin[1] << ", " << mask_origin[2] << std::endl;
  std::cout << "Mask Space: ";
  std::cout << mask_spacing[0] << ", " << mask_spacing[1] << ", " << mask_spacing[2] << std::endl;
  std::cout << "Mask Size: ";
  std::cout << mask_size[0] << ", " << mask_size[1] << ", " << mask_size[2] << std::endl;
  std::cout << "Mask Index: ";
  std::cout << mask_index[0] << ", " << mask_index[1] << ", " << mask_index[2] << std::endl;
*/
  int i = 0, j = 0;
    BTable.inplace_transpose();
    std::cout << "Rows: " << BTable.rows() << vcl_endl;
    std::cout << "Cols: " << BTable.cols() << vcl_endl;
    dwi_it.GoToBegin();
  odf_it.GoToBegin();
  mask_it.GoToBegin();

  std::cout << "BEGIN of multiplication " << std::endl;
  std::cout << "dwi_region = " << dwi_region << std::endl; 

  chronometer.Start("Multiplication");
    while( !dwi_it.IsAtEnd() )
    {
 //       dwi_it.GoToBeginOfLine();
 //       odf_it.GoToBeginOfLine();

        if( mask_it.Value() > 0.0)
        {
/*            i = 0;
            while( !dwi_it.IsAtEndOfLine() )
            {
                voxel_dwi(i++) = dwi_it.Get();
                ++dwi_it;
            }

            dummy = BTable.transpose()*voxel_dwi;
            voxel_odf = dummy/dummy.max_value();

            i = 0;
            while( !odf_it.IsAtEndOfLine() )
            {
                odf_it.Set( voxel_odf(i++) );
                ++odf_it;
            }
*/
            //i = 0; j = 0;


            for(i = 0, odf_it.GoToBeginOfLine();!odf_it.IsAtEndOfLine();++odf_it, ++i)
            {
              double sum = 0.0;
                for(j = 0, dwi_it.GoToBeginOfLine();!dwi_it.IsAtEndOfLine();++dwi_it, ++j)
                {
                    //std::cout << i << " , " << j << " ";
                   sum += BTable(i,j)*dwi_it.Value();
                }
              odf_it.Value()  = sum;
                //std::cout << std::endl;
            }

        }

        ++mask_it;
        dwi_it.NextLine();
        odf_it.NextLine();
    }
  chronometer.Stop("Multiplication");
  std::cout << "END of multiplication " << std::endl;
  chronometer.Report( std::cout );

  typedef itk::ImageFileWriter< ODFImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( odf_image );
  try
  {
    writer->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
