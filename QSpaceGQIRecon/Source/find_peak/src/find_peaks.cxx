#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageLinearConstIteratorWithIndex.h"
#include "itkImageLinearIteratorWithIndex.h"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_file_matrix.h>
#include <vnl/vnl_vector.h>

typedef vnl_matrix< float >    TMatrix;
typedef vnl_vector< float >    TVector;
typedef vnl_file_matrix<float>  VNLFile;
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
peak_find(TVector odf, TMatrix odf_faces)
{
  TVector is_peak = odf;
  
  if(odf_faces.min_value() == 0)
    odf_faces = odf_faces + 1;
  if(odf_faces.rows() != odf.size())
    odf_faces = odf_faces - 


---------------------------------------------------------------------------------------------------

struct SearchLocalMaximum
{
    std::vector<std::vector<unsigned short> > neighbor;
    std::map<float,unsigned short,std::greater<float> > max_table;
    void init(void)
    {
        process_position = __FUNCTION__;
        size_t half_odf_size = ti_vertices_count() >> 1;
        size_t faces_count = ti_faces_count();
        neighbor.resize(ti_vertices_count() >> 1);
        for (size_t index = 0;index < faces_count;++index)
        {
            short* i = ti_faces(index);
            short i1 = i[0];
            short i2 = i[1];
            short i3 = i[2];
            if (i1 >= half_odf_size)
                i1 -= half_odf_size;
            if (i2 >= half_odf_size)
                i2 -= half_odf_size;
            if (i3 >= half_odf_size)
                i3 -= half_odf_size;
            neighbor[i1].push_back(i2);
            neighbor[i1].push_back(i3);
            neighbor[i2].push_back(i1);
            neighbor[i2].push_back(i3);
            neighbor[i3].push_back(i1);
            neighbor[i3].push_back(i2);
        }
    }
    void search(const std::vector<float>& old_odf)
    {
        max_table.clear();
        for (size_t index = 0;index < neighbor.size();++index)
        {
            float value = old_odf[index];
            bool is_max = true;
            std::vector<unsigned short>& nei = neighbor[index];
            for (size_t j = 0;j < nei.size();++j)
            {
                if (value < old_odf[nei[j]])
                {
                    is_max = false;
                    break;
                }
            }
            if (is_max)
                max_table[value] = (unsigned short)index;
        }
    }
};

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main( int argc, char * argv [] )
{
  if( argc < 4 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " input_filename odf_faces index_output_filename mask_filename" << std::endl;
    return EXIT_FAILURE;
  }

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
/*    std::cout << "Rows: " << BTable.rows() << vcl_endl;
    std::cout << "Cols: " << BTable.cols() << vcl_endl;
*/
  DWIImageType::ConstPointer dwi_image = reader->GetOutput();
  typedef DWIImageType::IndexType    DWIIndexType;
    typedef DWIImageType::SizeType     DWISizeType;
    typedef DWIImageType::RegionType   DWIRegionType;
    typedef DWIImageType::SpacingType  DWISpacingType;
    typedef DWIImageType::PointType    DWIOriginType;

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

    TVector voxel_dwi(BTable.rows(), 0.0), voxel_odf(BTable.cols(), 0.0), dummy;

    typedef itk::ImageLinearConstIteratorWithIndex< DWIImageType > DWIIteratorType;
    DWIIteratorType dwi_it( dwi_image, dwi_region );
    dwi_it.SetDirection( 3 );

  typedef itk::ImageLinearIteratorWithIndex< ODFImageType > ODFIteratorType;
    ODFIteratorType odf_it( odf_image, odf_region );
  odf_it.SetDirection( 3 );

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
  int i = 0;

    dwi_it.GoToBegin();
  odf_it.GoToBegin();
  mask_it.GoToBegin();

    while( !dwi_it.IsAtEnd() )
    {
        dwi_it.GoToBeginOfLine();
        odf_it.GoToBeginOfLine();

        if( mask_it.Value() > 0.0)
        {
            i = 0;
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
        }

        ++mask_it;
        dwi_it.NextLine();
        odf_it.NextLine();
    }

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
