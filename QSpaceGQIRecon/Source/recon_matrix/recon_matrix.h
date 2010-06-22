#include <vnl/vnl_matrix.h>
#include <vnl/vnl_file_matrix.h>
#include <vnl/vnl_vector.h>
#include <fstream>
#include <vxl/vcl/vcl_iostream.h>

typedef vnl_matrix< float >     TMatrix;
typedef vnl_matrix< double >    DMatrix;
typedef vnl_vector< float >     TVector;
typedef vnl_file_matrix<float>  VNLFile;

void readMatFile(char *mat_filename, double mij[515][4],bool print_flag = false)
{
    std::ifstream fin( mat_filename , std::ifstream::in );

    for(int i = 0; i < 515; i++)
        for(int j = 0; j < 4; j++)
            fin >> mij[i][j];

    if(print_flag)
    {
        for(int i = 0; i < 515; i++)
        {
            for(int j = 0; j < 4; j++)
                vcl_cout << mij[i][j] << " ";
            vcl_cout << vcl_endl;
        }
    }
}

int WriteMatrix( char *mat_filename, TMatrix M )
{
  unsigned int r = M.rows();
  unsigned int c = M.cols();

  std::ofstream fout( mat_filename , std::ofstream::out );

  for(unsigned int i = 0; i < r; i++)
  {
      for(unsigned int j = 0; j < c; j++)
      {
          fout << M(i,j) << " ";
      }
      fout << vcl_endl;
  }
  return 1;
}

TMatrix VectorToMatrix(const TVector A)
{
    TMatrix M(A.size(), 1);
    for(unsigned int i = 0; i < A.size(); i++)
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
