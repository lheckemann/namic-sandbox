#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <set>
#include <iterator>

extern "C"{
float exactinit();
double orient3d(double*, double*, double*, double*);
}

using namespace std;

int             num_faces = 0, Num_nodes = 0, num_tets = 0;
int             num_tris, num_quads, num_pen5, num_pen6, num_hexa;
vector<unsigned>     connect;
vector<float>   xcoord, ycoord, zcoord;


//////////////////////////////////////////////////////////////////
  class TetFace{
  public:
    TetFace(){};
    TetFace(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3){
      fourth = v3;
      if(v0<v1)
        if(v1<v2){
          nodes[0] = v0; nodes[1] = v1; nodes[2] = v2;
        } else {
          if(v2<v0){
            nodes[0] = v2; nodes[1] = v0; nodes[2] = v1;
          } else {
            nodes[0] = v0; nodes[1] = v2; nodes[2] = v1;
          }
        }
      else
        if(v0<v2){
          nodes[0] = v1; nodes[1] = v0; nodes[2] = v2;
        } else {
          if(v1<v2){
            nodes[0] = v1; nodes[1] = v2; nodes[2] = v0;
          } else {
            nodes[0] = v2; nodes[1] = v1; nodes[2] = v0;
          }
        }
    }
    
    ~TetFace(){};
    
    bool operator==(const TetFace &f) const{
      return ((f.nodes[0]==this->nodes[0]) && (f.nodes[1]==this->nodes[1]) &&
        (f.nodes[2]==this->nodes[2]));
    }
    
    bool operator<(const TetFace &face1) const{
      if(this->nodes[0]<face1.nodes[0]) 
        return true;
      if(face1.nodes[0]<this->nodes[0])
        return false;
      if(this->nodes[1]<face1.nodes[1])
        return true;
      if(face1.nodes[1]<this->nodes[1])
        return false;
      return this->nodes[2]<face1.nodes[2];
    }

    unsigned int nodes[3];
    unsigned int fourth;
  };

map<TetFace,unsigned int> face2cnt;

const unsigned int tet_face_LUT[16] = 
    { 0, 1, 2, 3,
      0, 1, 3, 2,
      0, 2, 3, 1,
      1, 2, 3, 0 };


int main( int argc, char **argv)
{
     
  exactinit();

     string ifile = "o.", ofile = "f";
     string cmd;
     char  str[100];

     char str1[200], str2[200];
     int  i, j;

     ifstream in_file(argv[1]);
     ofstream out_file((string(argv[1])+".raw").c_str());

     for(i=0;i<2;i++){
       //in_file >> str1;
       in_file.getline(str1, 200);
       cout << str1 << endl;
     }
     
     in_file >> str1 >> Num_nodes; 
//     in_file.getline(str1, 200);

     xcoord.resize(Num_nodes);
     ycoord.resize(Num_nodes);
     zcoord.resize(Num_nodes);

     cerr << "Number of nodes: " << Num_nodes << endl;

     for(i=0;i<Num_nodes;i++){
       in_file >> xcoord[i] >> ycoord[i] >> zcoord[i];
     }
     // Skip edges
     int num_edges;
     int junk;
     in_file.getline(str1, 200);
     in_file >> str1 >> num_faces;
     connect.resize(4*num_faces);
     for(i=0;i<num_faces;i++){
       in_file >> connect[i*4] >> connect[i*4+1] >> connect[i*4+2] >> connect[i*4+3];
     }

     ofstream vtk_mesh((string(argv[1])+".vtk").c_str());
     vtk_mesh << "# vtk DataFile Version 3.0" << endl;
     vtk_mesh << "vtk output" << endl << "ASCII" << endl;
     vtk_mesh << "DATASET UNSTRUCTURED_GRID " << endl;
     vtk_mesh << "POINTS " << Num_nodes << " float" << endl;
     for(i=0;i<Num_nodes;i++){
       vtk_mesh << xcoord[i] << " " << ycoord[i] << " " 
         << zcoord[i] << endl;
     }
     vtk_mesh << "CELLS " << num_faces << " " << num_faces*5 << endl;
     for(i=0;i<num_faces;i++){
       vtk_mesh << "4 " << connect[i*4] << " " <<
         connect[i*4+1] << " " <<
         connect[i*4+2] << " " <<
         connect[i*4+3] << endl;
     }
     vtk_mesh << "CELL_TYPES " << num_faces << endl;
     for(i=0;i<num_faces;i++)
       vtk_mesh << "10" << endl;
     vtk_mesh.close();
     cout << "vtk_data saved" << endl;

//     out_file << Num_nodes << " " << num_faces << endl;
     
      cout << "Num elements: " << num_faces << ", num nodes: " << Num_nodes << endl;

      cout << "Extracting faces..." << endl;
      map<TetFace,unsigned int>::iterator face2cntI;
      for(i=0;i<num_faces;i++){
        int j;
        for(j=0;j<4;j++){
          TetFace thisFace(connect[i*4+tet_face_LUT[j*4]],
            connect[i*4+tet_face_LUT[j*4+1]],
            connect[i*4+tet_face_LUT[j*4+2]],
            connect[i*4+tet_face_LUT[j*4+3]]);
          face2cntI = face2cnt.find(thisFace);
          if(face2cntI != face2cnt.end())
            face2cnt[thisFace]++;
          else
            face2cnt[thisFace] = 1;
        }
      }
      std::cout << "Total faces: " << face2cnt.size() << endl;
      std::set<unsigned int> surfVerticesSet;
      std::vector<TetFace> SurfaceFaces;
      for(face2cntI=face2cnt.begin();
        face2cntI!=face2cnt.end();
        face2cntI++){
        unsigned int j;
        switch((*face2cntI).second){
        case 1:{
          TetFace thisFace = (*face2cntI).first;
          for(j=0;j<3;j++)
            surfVerticesSet.insert(thisFace.nodes[j]);
          SurfaceFaces.push_back(thisFace);
          break;}
        case 2: break;
        case 0:
        default:assert(0);
        }
      }

     unsigned surf_vert_cnt = surfVerticesSet.size();
     map<unsigned int, unsigned int> id2id;
     vector<unsigned int> surfVerticesVector;
     insert_iterator<vector<unsigned int> > viI(surfVerticesVector, surfVerticesVector.begin());
     copy(surfVerticesSet.begin(), surfVerticesSet.end(), viI);
     out_file << surf_vert_cnt << " " << SurfaceFaces.size() << endl;
     
     for(i=0;i<surf_vert_cnt;i++){
       id2id[surfVerticesVector[i]] = i;
       out_file << xcoord[surfVerticesVector[i]] << " " << ycoord[surfVerticesVector[i]] << " " << zcoord[surfVerticesVector[i]] << endl;
     }
     for(i=0;i<SurfaceFaces.size();i++){
       TetFace thisFace = SurfaceFaces[i];
       double vertices[4][3];
       int k;
       for(j=0;j<3;j++)
         for(k=0;k<3;k++){
           vertices[j][0] = xcoord[thisFace.nodes[j]];  
           vertices[j][1] = ycoord[thisFace.nodes[j]];  
           vertices[j][2] = zcoord[thisFace.nodes[j]];  
         }
       for(k=0;k<3;k++){
         vertices[3][0] = xcoord[thisFace.fourth];
         vertices[3][1] = ycoord[thisFace.fourth];
         vertices[3][2] = zcoord[thisFace.fourth];
       }
       
       if(orient3d(&vertices[0][0], &vertices[1][0], &vertices[2][0], &vertices[3][0])>0){
         unsigned tmp_vertex = thisFace.nodes[0];
         thisFace.nodes[0] = thisFace.nodes[1];
         thisFace.nodes[1] = tmp_vertex;
         SurfaceFaces[i] = thisFace;
       }

       out_file << id2id[SurfaceFaces[i].nodes[0]] << " " << 
         id2id[SurfaceFaces[i].nodes[1]] << " " << 
         id2id[SurfaceFaces[i].nodes[2]] << endl;
     }
     
       
     /*
     ofstream vtk_surf("vtksurf.vtk");
     vtk_surf << "# vtk DataFile Version 3.0" << endl;
     vtk_surf << "vtk output" << endl << "ASCII" << endl;
     vtk_surf << "DATASET POLYDATA" << endl;
     vtk_surf << "POINTS " << Num_nodes << " float" << endl;
     for(i=0;i<Num_nodes;i++){
       vtk_mesh << xcoord[i] << " " << ycoord[i] << " " 
         << zcoord[i] << endl;
     }
      */
     
     /*
     for(i=0;i<Num_nodes;i++){
       out_file << setprecision(10) << xcoord[i] << " ";
       out_file << setprecision(10) << ycoord[i] << " ";
       out_file << setprecision(10) << zcoord[i] << endl;
     }

     for(i=0;i<num_faces;i++){
       for(j=0;j<4;j++)
         out_file << connect[i*4+j] << " ";
       out_file << endl;
     }
     */
     out_file << endl;
     out_file.close();
     
     return 0;
    }
