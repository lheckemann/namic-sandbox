// hypercube.cpp : Defines the entry point for the console application.
// program estimates hyper cube rotation group of vertex and edge permutations
// edges defined by vertex pairs
//
// Marching Hypercubes
// the rotations are applied to different vertex configurations to find patterns
// the patterns are tesselated to find a list of edge pairs or line segments
// in 2D the lines form contours
// in 3D the lines are connected to form polygons tesselated into triangle forming surfaces
// in 4D the triangles form tetrahedral 3D hypersurfaces
// the tesselated patterns consisting of a list of edges are rotated to generate a lookup table
// for each configuration
// author Harvey Cline, copyright Kitware Inc 2009
// April 6, 2009 complete vertexrotation groups
// April 13, 2009 complete edge permutations

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "math.h"
void permute(int *a, int *b, int *c, int length);
void permuteEdges(int vertices, int A[16][16],int group[192][16]);
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
  int Faces3D[6][8],Faces4D[6][16];
  int Rotation3D[4][8],Rotation4D[4][16];
  int group2D[4][4],group3D[24][8],group4D[192][16];
  int Cubes4D[8][16];
  int Rotation2D[4][4];
  int A[16][16];      //hypercube graph edge matrix
  //vertex and edge permutation generators
  int z4D[] ={3,1,4,2,7,5,8,6,11,9,12,10,15,13,16,14};  
  int t4D[] ={9,1,10,2,11,3,12,4,13,5,14,6,15,7,16,8};
  int y4D[] ={5,1,7,3,6,2,8,4,13,9,15,11,14,10,16,12};
  int x4D[] ={5,6,1,2,7,8,3,4,13,14,9,10,15,16,11,12};
  int y4Dinv[]={3,7,1,5,4,8,2,6,11,15,9,13,12,16,10,14};
  int x3D[] ={5,6,1,2,7,8,3,4};
  int y3D[] ={5,1,7,3,6,2,8,4};
  int z3D[] ={3,1,4,2,7,5,8,6};
  int y3Dinv[]={3,7,1,5,4,8,2,6};
  int z2D[]={3,1,4,2};
  int z1D[] ={1,2};
  //edge definition in terms of vertex pairs
  int edges4D[32][2]={{1,2},{1,3},{2,4},{3,4},{1,5},{2,6},{3,7},{4,8},{5,6},{5,7},{6,8},{7,8},
  {1,9},{2,10},{3,11},{4,12},{5,13},{6,14},{7,15},{8,16},{9,10},{9,11},{10,12},{11,2},
  {9,13},{10,14},{11,15},{12,16},{13,14},{13,15},{14,16},{15,16}};
  int xEdges[32][2],yEdges[32][2],zEdges[32][2],tEdges[32][2];
  int vertexOne[32],vertexTwo[32];

  int dimension=0;
  int vertices;
  int edges;
  int rotations;

  int i,j,k;

  //
  //input cube dimension
  //

  cout<<"input cube dimension = ";
  cin>>dimension;

  vertices = (int)pow(2.0,dimension);
  if(dimension==1)
  {
    edges = 1;
    rotations = 1;
    cout<<"vertices = "<<vertices<<"  edges = "<<edges<<"  rotations = "<<rotations<<endl;
  }
  
  if(dimension==2)
  {
    edges=4;
    rotations=4;
    cout<<"vertices = "<<vertices<<"  edges = "<<edges<<"  rotations = "<<rotations<<endl;
    //square
    for(i=0;i<vertices;i++)
    {
      Rotation2D[1][i]=z2D[i];
    }
    permute(z2D,Rotation2D[1],Rotation2D[2],vertices);
    permute(z2D,Rotation2D[2],Rotation2D[3],vertices);
    permute(z3D,Rotation2D[3],Rotation2D[0],vertices);
    //print permutations
    for(i=0;i<vertices;i++)
    {
      cout<<i+1<<"\t";
      for(j=0;j<vertices;j++)
      {
        group2D[i][j]=Rotation2D[i][j];
        cout<<group2D[i][j]<<",";
      }
      cout<<endl;
    }
  }//end 2D
  if(dimension==3)
  {
    edges=12;
    rotations=24;// cube
    cout<<"vertices = "<<vertices<<"  edges = "<<edges<<"  rotations = "<<rotations<<endl;
    //6 faces, the first 4 are a cycle
    for(i=0;i<8;i++)
    {
      Faces3D[1][i]=x3D[i];
      Faces3D[4][i]=y3D[i];
      Faces3D[5][i]=y3Dinv[i];
    }
    permute(x3D,Faces3D[1],Faces3D[2],8);
    permute(x3D,Faces3D[2],Faces3D[3],8);
    permute(x3D,Faces3D[3],Faces3D[0],8);  
      
    //4 rotations form a cycle
    for(i=0;i<8;i++)
    {
      Rotation3D[1][i]=z3D[i];
    }
    permute(z3D,Rotation3D[1],Rotation3D[2],8);
    permute(z3D,Rotation3D[2],Rotation3D[3],8);
    permute(z3D,Rotation3D[3],Rotation3D[0],8);

    cout<<"faces"<<endl;
    for(i=0;i<6;i++)
    {
      cout<<i+1<<"\t";
      for(j=0;j<8;j++)
      {
        cout<<Faces3D[i][j]<<",";
      }
      cout<<endl;
    }
    cout<<"rotations"<<endl;
    for(i=0;i<4;i++)
    {
      cout<<i+1<<"\t";
      for(j=0;j<8;j++)
      {
        cout<<Rotation3D[i][j]<<",";
      }
      cout<<endl;
    }

    //group = rotation X faces
    cout<<"24 element 3D rotation group expressed as vertex permutations"<<endl;
    for(i=0;i<4;i++)
    {
      for(j=0;j<6;j++)
      {
        k=j+6*i;
        permute(Rotation3D[i],Faces3D[j],group3D[k],8);
      }
    }
    for(i=0;i<24;i++)
    {
      cout<<i+1<<"\t";
      for(j=0;j<8;j++)
      {
        cout<<group3D[i][j]<<",";
      }
      cout<<endl;
    }
  }//end 3D

  if(dimension==4)
  {
    edges=32;
    rotations=192;
    cout<<"vertices = "<<vertices<<"  edges = "<<edges<<"  rotations = "<<rotations<<endl;
    //hypercube
    //find the 24 element 4D cube rotation group
    //6 faces
    for(i=0;i<16;i++)
    {
      Faces4D[1][i]=x4D[i];
      Faces4D[4][i]=y4D[i];
      Faces4D[5][i]=y4Dinv[i];
    }
    permute(x4D,Faces4D[1],Faces4D[2],16);
    permute(x4D,Faces4D[2],Faces4D[3],16);
    permute(x4D,Faces4D[3],Faces4D[0],16);  

    //4 rotations
    for(i=0;i<16;i++)
    {
      Rotation4D[1][i]=z4D[i];
    }
    permute(z4D,Rotation4D[1],Rotation4D[2],16);
    permute(z4D,Rotation4D[2],Rotation4D[3],16);
    permute(z4D,Rotation4D[3],Rotation4D[0],16);

    cout<<"faces"<<endl;
    for(i=0;i<6;i++)
    {
      cout<<i+1<<"\t";
      for(j=0;j<16;j++)
      {
        cout<<Faces4D[i][j]<<",";
      }
      cout<<endl;
    }
    cout<<"rotations"<<endl;
    for(i=0;i<4;i++)
    {
      cout<<i+1<<"\t";
      for(j=0;j<16;j++)
      {
        cout<<Rotation4D[i][j]<<",";
      }
      cout<<endl;
    }
    //
    //group = rotation X faces
    //
    cout<<"24 group"<<endl;
    for(i=0;i<4;i++)
    {
      for(j=0;j<6;j++)
      {
        k=j+6*i;
        permute(Rotation4D[i],Faces4D[j],group4D[k],16);
      }
    }
    for(i=0;i<24;i++)
    {
      cout<<i+1<<"\t";
      for(j=0;j<16;j++)
      {
        cout<<group4D[i][j]<<",";
      }
      cout<<endl;
    }
    //
    //8 cube rotations along an axis of eight fold symmetry
    //
    for(i=0;i<16;i++)
    {
      Cubes4D[1][i]=t4D[i];
    }
    permute(t4D,Cubes4D[1],Cubes4D[2],16);
    permute(t4D,Cubes4D[2],Cubes4D[3],16);
    permute(t4D,Cubes4D[3],Cubes4D[4],16);
    permute(t4D,Cubes4D[4],Cubes4D[5],16);
    permute(t4D,Cubes4D[5],Cubes4D[6],16);
    permute(t4D,Cubes4D[6],Cubes4D[7],16);
    permute(t4D,Cubes4D[7],Cubes4D[0],16);

    cout<<"cube rotation"<<endl;
    for(i=0;i<8;i++)
    {
      cout<<i+1<<"\t";
      for(j=0;j<16;j++)
      {
        cout<<Cubes4D[i][j]<<",";
      }
      cout<<endl;
    }
    //group = rotation X faces
    cout<<"192 group"<<endl;
    for(i=0;i<8;i++)
    {
      for(j=0;j<24;j++)
      {
        k=j+24*i;
        permute(Cubes4D[i],group4D[j],group4D[k],16);
      }
    }
    //print permutations
    for(i=0;i<192;i++)
    {
      cout<<i+1<<"\t";
      for(j=0;j<16;j++)
      {
        cout<<group4D[i][j]<<",";
      }
      cout<<endl;
    }
    //
    //the 32 edges defined by a pair of vertices are permuted using the 4D rotation group
    //
    cout<<"permuted edges defined by vertex pairs at end points= "<<edges<<endl;
    cout<<"edge    1       x       y      z     t"<<endl;
    
    for(i=0;i<edges;i++)
    {
      xEdges[i][0]=x4D[edges4D[i][0]-1];
      xEdges[i][1]=x4D[edges4D[i][1]-1];
      yEdges[i][0]=y4D[edges4D[i][0]-1];
      yEdges[i][1]=y4D[edges4D[i][1]-1];
      zEdges[i][0]=z4D[edges4D[i][0]-1];
      zEdges[i][1]=z4D[edges4D[i][1]-1];
      tEdges[i][0]=t4D[edges4D[i][0]-1];
      tEdges[i][1]=t4D[edges4D[i][1]-1];
    }
    for(i=0;i<edges;i++)
    {
      cout<<i+1<<"\t"<<edges4D[i][0]<<","<<edges4D[i][1]<<"\t"<<xEdges[i][0]<<","<<xEdges[i][1]<<"\t"
        <<yEdges[i][0]<<","<<yEdges[i][1]<<"\t"<<zEdges[i][0]<<","<<zEdges[i][1]<<"\t"<<tEdges[i][0]<<","<<tEdges[i][1]<<endl;
    }  
    cout<<endl;
    cout<<"edge permutaion generators x,y,z,t"<<endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(xEdges[i][0]==edges4D[j][0]||xEdges[i][0]==edges4D[j][1])
        {
          if(xEdges[i][1]==edges4D[j][0]||xEdges[i][1]==edges4D[j][1])
          {
            cout<<j+1<<",";
          }
        }
      }
    }
    cout<<endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(yEdges[i][0]==edges4D[j][0]||yEdges[i][0]==edges4D[j][1])
        {
          if(yEdges[i][1]==edges4D[j][0]||yEdges[i][1]==edges4D[j][1])
          {
            cout<<j+1<<",";
          }
        }
      }
    }
    cout<<endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(zEdges[i][0]==edges4D[j][0]||zEdges[i][0]==edges4D[j][1])
        {
          if(zEdges[i][1]==edges4D[j][0]||zEdges[i][1]==edges4D[j][1])
          {
            cout<<j+1<<",";
          }
        }
      }
    }
    cout<<endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(tEdges[i][0]==edges4D[j][0]||tEdges[i][0]==edges4D[j][1])
        {
          if(tEdges[i][1]==edges4D[j][0]||tEdges[i][1]==edges4D[j][1])
          {
            cout<<j+1<<",";
          }
        }
      }
    }
    cout<<endl;
  }//end 4D
  if(dimension == 0) cout<<"error: the dimension range is from one to four"<<endl;

  //permuteEdges(vertices,A,group4D);

  cin>>i;
  return 0;
}
void permuteEdges(int vertices, int A[16][16],int group4D[192][16])
{
  int i,j,k;
  int edge[32];
  //int A[16][16];
  cout<<"edge graph A"<<endl;
  for(i=0;i<vertices;i++)
  {
    for(j=0;j<vertices;j++)
    {
      A[i][j]=0;
    }
  }
  A[0][1]=1; A[1][0]=1; A[0][2]=2; A[2][0]=2; A[1][3]=3; A[3][1]=3; A[2][3]=4; A[3][2]=4;
  A[0][4]=5; A[4][0]=5; A[1][5]=6; A[5][1]=6; A[2][6]=7; A[6][2]=7; A[3][7]=8; A[7][3]=8;
  A[4][5]=9; A[5][4]=9; A[4][6]=10; A[6][4]=10; A[5][7]=11; A[7][5]=11; A[6][7]=12; A[7][6]=12;
  A[0][8]=13; A[8][0]=13; A[1][9]=14; A[9][1]=14; A[2][10]=15; A[10][2]=15; A[3][11]=16; A[11][3]=16;
  A[4][12]=17; A[12][4]=17; A[5][13]=18; A[13][5]=18; A[6][14]=19; A[14][6]=19; A[7][15] = 20; A[15][7] = 20;
  A[8][9]=21; A[9][8]=21; A[8][10]=22; A[10][8]=22; A[9][11]=23; A[11][9]=23; A[10][11]=24; A[11][10] = 24;
  A[8][12]=25; A[12][8]=25; A[9][13]=26; A[13][9]=26; A[10][14]=27; A[14][10]=27; A[11][15] = 28; A[15][11] = 28;
  A[12][13]=29; A[13][12]=29; A[12][14]=30; A[14][12]=30; A[13][15]=31; A[15][13]=31; A[14][15]=32; A[15][14]=32;

  for(i=0;i<vertices;i++)
  {
    for(j=0;j<vertices;j++)
    {
      cout<<A[i][j]<<",";
    }
    cout<<endl;
  }
  //
  //permute edges
  //
  cout<<"permute edges by permuting vertices in edge graph"<<endl;
  for(k=0;k<192;k++)
  {
    for(i=0;i<vertices;i++)
    {
      for(j=0;j<vertices;j++)
      {
        A[i][j]=A[group4D[k][i]-1][group4D[k][j]-1];
      }
    }
    edge[0]=A[0][1];    edge[1]=A[0][2];   edge[2]=A[1][3];   edge[3]=A[2][3];    edge[4]=A[0][4];    edge[5]=A[1][5];
    edge[6]=A[2][6];    edge[7]=A[3][7];   edge[8]=A[4][5];   edge[9]=A[4][6];    edge[10]=A[5][7];   edge[11]=A[6][7];
    edge[12]=A[0][8];   edge[13]=A[1][9];  edge[14]=A[2][10]; edge[15]=A[3][11];  edge[16]=A[4][12];  edge[17]=A[5][13];
    edge[18]=A[6][14];  edge[19]=A[7][15]; edge[20]=A[8][9];  edge[21]=A[8][10];  edge[22]=A[9][11];  edge[23]=A[10][11];
    edge[24]=A[8][12]; edge[25]=A[9][13]; edge[26]=A[10][14]; edge[27]=A[11][15]; edge[28]=A[12][13]; edge[29]=A[12][14];
    edge[30]=A[13][15]; edge[31]=A[14][15]; 

    for(i=0;i<32;i++)
    {
      cout<<edge[i]<<",";
    }
    cout<<endl;
  }
}
void permute(int *a, int *b, int *c, int elements)
{
  int i;
  for(i=0;i<elements;i++)
    {
    c[i]=a[b[i]-1];
    }
}


