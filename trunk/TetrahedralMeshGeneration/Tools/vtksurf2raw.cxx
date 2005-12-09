#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

int             num_faces = 0, Num_nodes = 0, num_tets = 0;
int             num_tris, num_quads, num_pen5, num_pen6, num_hexa;
vector<int>     connect;
vector<float>   xcoord, ycoord, zcoord;

//////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////

void read_file( const string &s)
{
     ifstream  infile;
     infile.open(s.c_str(), ios::in);
     if( infile.fail() ) {
         cout << " File cann't open " << s << endl;
         return;
     }
     cout << " Reading File " << s << endl;

     infile >> num_faces >> Num_nodes;

     xcoord.resize( Num_nodes );
     ycoord.resize( Num_nodes );
     zcoord.resize( Num_nodes );

     for( int i = 0; i < Num_nodes; i++) infile >> xcoord[i];
     for( int i = 0; i < Num_nodes; i++) infile >> ycoord[i];
     for( int i = 0; i < Num_nodes; i++) infile >> zcoord[i];

     connect.resize(3*num_faces);
     for( int i = 0; i < num_faces; i++){ 
          for( int j = 0; j < 3; j++) 
               infile >> connect[3*i+j];
     }
}

//////////////////////////////////////////////////////////////////

void write_file( const string &fname, int fmarker)
{
     ofstream   ofile;
     ofile.open( fname.c_str(), ios::out);
     if( ofile.fail() ) {
         cout << " File cann't open " << fname << endl;
         return;
     }

     cout << " Output File " << fname << endl;

     ofile << Num_nodes << "  " << num_faces << "  " << num_tets << endl;
    
     cout << " x " << xcoord.size() << endl;
     for( int i = 0; i < Num_nodes; i++) 
          ofile << setprecision(10) << xcoord[i] << " ";
     ofile << endl;
     cout << " x " << endl;

     for( int i = 0; i < Num_nodes; i++) 
          ofile << setprecision(10) << ycoord[i] << " ";
     ofile << endl;
     cout << " x " << endl;

     for( int i = 0; i < Num_nodes; i++) 
          ofile << setprecision(10) << zcoord[i] << " ";
     ofile << endl;
     cout << " x " << endl;


     for( int i = 0; i < num_faces; i++){ 
          for( int j = 0; j < 3; j++) 
               ofile << connect[3*i+j] << " ";
          cout << connect[3*i] << " " << connect[3*i+1] << " " << connect[3*i+2] << endl;
     }
     ofile << endl;
     cout << " x " << endl;

     for( int i = 0; i < num_faces; i++) 
          ofile << fmarker << " ";
     ofile << endl;
}


//////////////////////////////////////////////////////////////////
void read_solidmesh_ugrid( const string &s)
{
     ifstream  infile;
     infile.open(s.c_str(), ios::in);
     if( infile.fail() ) {
         cout << " File cann't open " << s << endl;
         return;
     }
     cout << " Reading File " << s << endl;

     infile >> Num_nodes >> num_tris >> num_quads >> num_tets >> num_pen5
     >> num_pen6 >> num_hexa;

     xcoord.resize( Num_nodes );
     ycoord.resize( Num_nodes );
     zcoord.resize( Num_nodes );

     for( int i = 0; i < Num_nodes; i++) 
          infile >> xcoord[i] >> ycoord[i] >> zcoord[i];

     int n1, n2, n3;
     for( int i = 0; i < num_tris; i++) 
          infile >> n1 >> n2 >> n3;
 
     for( int i = 0; i < num_tris; i++) 
          infile >> n1;

     connect.resize( 4*num_tets);
     for( int i = 0; i < num_tets; i++) 
          infile >> connect[4*i] >> connect[4*i+1] >> connect[4*i+2] >> connect[4*i+3];

} 
//////////////////////////////////////////////////////////////////
void write_andriey_file( const string &fname)
{
     ofstream   ofile;
     ofile.open( fname.c_str(), ios::out);
     if( ofile.fail() ) {
         cout << " File cann't open " << fname << endl;
         return;
     }

     cout << " Output File " << fname << endl;

     ofile << Num_nodes << endl;
    
     for( int i = 0; i < Num_nodes; i++) 
          ofile << i << "  " << setprecision(10) << xcoord[i] << " " 
                                    << ycoord[i] << " " 
                                    << zcoord[i] << endl;

     ofile << num_tets << endl;
     for( int i = 0; i < num_tets; i++){ 
          ofile << i << "  ";
          for( int j = 0; j < 4; j++) 
               ofile << connect[4*i+j]-1 << " ";
          ofile << endl;
     }


}

int main( int argc, char **argv)
{
     
     string ifile = "o.", ofile = "f";
     string cmd;
     char  str[100];

     char str1[200], str2[200];
     int  i, j;

     ifstream in_file(argv[1]);
     ofstream out_file(argv[2]);

     for(i=0;i<4;i++){
       //in_file >> str1;
       in_file.getline(str1, 200);
       cout << str1 << endl;
     }
     
     in_file >> str1 >> Num_nodes; 
     in_file.getline(str1, 200);

     xcoord.resize(Num_nodes);
     ycoord.resize(Num_nodes);
     zcoord.resize(Num_nodes);

     cerr << "Number of nodes: " << Num_nodes << endl;

     for(i=0;i<Num_nodes;i++){
       in_file >> setprecision(10) >> xcoord[i] >> ycoord[i] >> zcoord[i];
     }

     in_file.getline(str1, 200);
     in_file >> str1 >> num_faces >> str1;
     connect.resize(3*num_faces);
     for(i=0;i<num_faces;i++){
       int junk;
       in_file >> junk >> connect[i*3] >> connect[i*3+1] >> connect[i*3+2];
     }
     out_file << Num_nodes << " " << num_faces << endl;
     
     for(i=0;i<Num_nodes;i++){
       out_file << setprecision(10) << xcoord[i] << " ";
       out_file << setprecision(10) << ycoord[i] << " ";
       out_file << setprecision(10) << zcoord[i] << endl;
     }

     for(i=0;i<num_faces;i++){
       for(j=0;j<3;j++)
         out_file << connect[i*3+j] << " ";
       out_file << endl;
     }
     out_file << endl;
     out_file.close();
     
     return 0;
    }
