#include "recon_matrix.h"
#include <vxl/vcl/vcl_cstdlib.h>

int main( int argc, char * argv [] )
{
  if( argc < 4 )
  {
    vcl_cerr << "Usage: " << vcl_endl;
    vcl_cerr << argv[0] << " btable_filename odf_vertices_filename mean_diffusion_distance_ratio outputfilename half [= 0] " << vcl_endl;
    return 1;
  }

    char        *btable_filename  = argv[1];
    VNLFile     BTable(btable_filename);
    std::cout << "b-table Rows: " << BTable.rows() << vcl_endl;
    std::cout << "b-table Cols: " << BTable.cols() << vcl_endl;

    char        *odf_vetices_filename  = argv[2];
    VNLFile     FullODFvertices(odf_vetices_filename);
    TMatrix     ODFvertices;

 /*   if(argc == 5)
        if( vcl_atoi(argv[5]) == 1)
            ODFvertices = FullODFvertices.extract(FullODFvertices.rows()/2, FullODFvertices.cols());
        else
            ODFvertices = FullODFvertices;
    else
            ODFvertices = ODFvertices;
*/
    vcl_cout << "ODF Vertices Rows: " << ODFvertices.rows() << vcl_endl;
    vcl_cout << "ODF Vertices Cols: " << ODFvertices.cols() << vcl_endl;

    float       mean_diffusion_distance_ratio =   vcl_atof(argv[3]);
    vcl_cout << "Mean Diffusion Distance Ratio =  " << mean_diffusion_distance_ratio << vcl_endl;

    TMatrix LValues         = ( BTable.extract(BTable.rows(),1)*0.01506 ).apply(sqrtf);
    TMatrix BVectors        = element_product(BTable.extract(BTable.rows(), 3, 0, 1),repmat(LValues,1,3));
    TMatrix ReconMatrix     = ( BVectors*ODFvertices.transpose()*mean_diffusion_distance_ratio ).apply(sinc);

    char        *output_filename  = argv[4];
    WriteMatrix(output_filename, ReconMatrix.transpose());

    //vcl_cout << "Reconstruction Matrix Rows: " << ReconMatrix.rows() << vcl_endl;
    //vcl_cout << "Reconstruction Matrix Cols: " << ReconMatrix.cols() << vcl_endl;

  return 0;
}

