#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageLinearConstIteratorWithIndex.h"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_file_matrix.h>
#include <vnl/vnl_vector.h>

typedef vnl_matrix< float >    TMatrix;
typedef vnl_matrix< double >    DMatrix;
typedef vnl_vector< float >    TVector;
typedef vnl_file_matrix<float> VNLFile;

void readMatFile(char *mat_filename, double mij[515][4],bool print_flag = false)
{
    std::ifstream fin(mat_filename);

    for(int i = 0; i < 515; i++)
        for(int j = 0; j < 4; j++)
            fin >> mij[i][j];

    if(print_flag)
    {
        for(int i = 0; i < 515; i++)
        {
            for(int j = 0; j < 4; j++)
                std::cout << mij[i][j] << " ";
            std::cout << std::endl;
        }
    }
}

TMatrix VectorToMatrix(const TVector A)
{
    TMatrix M(A.size(), 1);
    for(int i = 0; i < A.size(); i++)
        M(i,0) = A(i);
    return M;
}

TMatrix repmat( TMatrix M , int r, int c)
{
  TMatrix M_out = TMatrix(M.rows()*r, M.cols()*c, 1.0);
  for(int i = 0; i < r; i++)
  {
      for(int j = 0; j < c; j++)
      {
          M_out.update(M,M.rows()*i, M.cols()*j);
      }
  }
  return M_out;
}

TMatrix repmat( TVector MV , int r, int c)
{
    TMatrix M = VectorToMatrix(MV);
  return repmat(M,r,c);
}

inline float sinc(float x)
{
    if (x==0)
        return 1;
    return sin(x)/x;
}

int main( int argc, char * argv [] )
{
  if( argc < 4 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile btable_filename odf_vertices_filename" << std::endl;
    return EXIT_FAILURE;
  }

  typedef short PixelType;
  const unsigned int ImageDimension = 4;
  typedef itk::Image< PixelType, ImageDimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
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

    ImageType::ConstPointer image = reader->GetOutput();

  const ImageType::PointType& origin = image->GetOrigin();
  std::cout << "Origin: ";
  std::cout << origin[0] << ", " << origin[1] << ", " << origin[2] << std::endl;
  ImageType::SpacingType space = image->GetSpacing();
  std::cout << "Space: ";
  std::cout << space[0] << ", " << space[1] << ", " << space[2] << std::endl;
  ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();;
  std::cout << "Size: ";
  std::cout << size[0] << ", " << size[1] << ", " << size[2] << ", " << size[3] << std::endl;

    ImageType::RegionType region = image->GetBufferedRegion();
    typedef ImageType::IndexType     IndexType;
    IndexType       index   = region.GetIndex();
    index[0] = 47;index[1] = 37;index[2] = 27;


    typedef itk::ImageLinearConstIteratorWithIndex<ImageType > IteratorType;

    IteratorType it( image, region );
    it.SetDirection( 3 ); // Walk along time dimension
    it.SetIndex(index);
    while( !it.IsAtEndOfLine() )
    {
        //std::cout << it.Get() << std::endl;
        ++it;
    }

    char * btable_filename  = argv[2];
    VNLFile BTable(btable_filename);
    std::cout << "Rows: " << BTable.rows() << std::endl;
    std::cout << "Cols: " << BTable.cols() << std::endl;
    //std::cout << "B Table: " << std::endl << BTable << std::endl;
    char * odf_vetices_filename  = argv[3];
    VNLFile ODFvertices(odf_vetices_filename);
    std::cout << "Rows: " << ODFvertices.rows() << std::endl;
    std::cout << "Cols: " << ODFvertices.cols() << std::endl;

    //std::cout << "ODF Vertices: " << std::endl << ODFvertices << std::endl;

    TMatrix A(3,3);
    A(0,0) = 1.0;A(0,1) = 2.0;A(0,2) = 3.0;
    A(1,0) = 4.0;A(1,1) = 5.0;A(1,2) = 6.0;
    A(2,0) = 7.0;A(2,1) = 8.0;A(2,2) = 9.0;

    std::cout << A;
    TMatrix M = repmat(A, 3, 5);
    std::cout << "M Matrix" << std::endl << M << std::endl;
    TMatrix MM = TMatrix();
    TVector VV = TVector(6,1.0);
    TVector WW = TVector(3,2.0);
    std::cout << "VV Vector" << std::endl << VectorToMatrix(WW) << std::endl;
    TMatrix ZZ = VectorToMatrix(WW)*4.5;
    std::cout << ZZ;
    ZZ.apply(sqrtf);
    std::cout << ZZ.apply(sqrtf);
    TMatrix UU = repmat(WW, 2, 3);
    std::cout << "Repmat Vector" << std::endl << UU << std::endl;

    TMatrix ReconMatrix = TMatrix(BTable.rows(),ODFvertices.rows());
    TMatrix dummy = BTable.extract(BTable.rows(),1)*0.01506;
    TMatrix LValues = dummy.apply(sqrtf);
    TMatrix BVectors = BTable.extract(BTable.rows(), 3, 0, 1);
    std::cout << "BVectors: " << std::endl;
    std::cout << BVectors;
    std::cout << "LValue: " << std::endl;
    std::cout << repmat(LValues,1,3);
    std::cout << element_product(BVectors,repmat(LValues,1,3));
    std::cout << "Reconstruction Matrix Rows: " << ReconMatrix.rows() << std::endl;
    std::cout << "Reconstruction Matrix Cols: " << ReconMatrix.cols() << std::endl;
  LValues.apply(sinc);
  std::cout << A.apply(sinc);
    //std::cout << "B Value Vector" << std::endl << BTable.extract(BTable.rows(),1) << std::endl;
    //readMatFile(mat_filename, btable,true);
/*    it.GoToBegin();
    while( !it.IsAtEnd() )
    {
        SumType sum = itk::NumericTraits< SumType >::Zero;
        it.GoToBeginOfLine();
        index4D = it.GetIndex();
        while( !it.IsAtEndOfLine() )
        {
            sum += it.Get();
            ++it;
        }
        MeanType mean = static_cast< MeanType >( sum ) static_cast< MeanType >( timeLength );

        index3D[0] = index4D[0];
        index3D[1] = index4D[1];
        index3D[2] = index4D[2];

        image3D->SetPixel( index3D, static_cast< PixelType >( mean ) );
        it.NextLine();
    }
*/
  return EXIT_SUCCESS;
}
