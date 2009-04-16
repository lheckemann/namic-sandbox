/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImage.h,v $
  Language:  C++
  Date:      $Date: 2008-09-15 17:38:11 $
  Version:   $Revision: 1.148 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// Marching Hypercubes
// hypercube.cpp : Defines the entry point for the console application.
// program estimates hyper cube rotation group of vertex and edge permutations
// edges defined by vertex pairs
// the rotations are applied to different vertex configurations to find patterns
// the patterns are tesselated to find a list of edge pairs or line segments
// in 2D the lines form contours
// in 3D the lines are connected to form polygons tesselated into triangle forming surfaces
// in 4D the triangles form tetrahedral 3D hypersurfaces
// the tesselated patterns consisting of a list of edges are rotated to generate a lookup table
// for each configuration

// author Harvey Cline, 


#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "math.h"

void permute(int *a, int *b, int *c, int length);
void intToBinary(int n, int vertices, int *binary);

int main(int argc, char * argv [])
{
  char name1D[]=  "line";
  char name2D[]=  "square";
  char name3D[]=  "cube";
  char name4D[]=  "tesseract";
 
  int Faces3D[6][8],Faces4D[6][16];
  int Faces3DEdge[6][12],Faces4DEdge[6][32];
  int Rotation3D[4][8],Rotation4D[4][16];
  int Rotation3DEdge[4][12],Rotation4DEdge[4][32];
  int group2D[4][4],group3D[24][8],group4D[192][16];
  int group2DEdge[4][4],group3DEdge[24][12],group4DEdge[192][32];
  int Cubes4D[8][16],Cubes4DEdge[8][32];
  int Rotation2D[4][4],Rotation2DEdge[4][4];
  int x4DEdge[32],y4DEdge[32],z4DEdge[32],t4DEdge[32];
  int x3DEdge[32],y3DEdge[32],z3DEdge[32];
  //vertex and edge permutation generators
  int z4D[] ={3,1,4,2,7,5,8,6,11,9,12,10,15,13,16,14},I4D[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};  
  int t4D[] ={9,1,10,2,11,3,12,4,13,5,14,6,15,7,16,8};
  int y4D[] ={5,1,7,3,6,2,8,4,13,9,15,11,14,10,16,12};
  int x4D[] ={5,6,1,2,7,8,3,4,13,14,9,10,15,16,11,12};
  int y4Dinv[]={3,7,1,5,4,8,2,6,11,15,9,13,12,16,10,14};
  int x3D[] ={5,6,1,2,7,8,3,4};
  int y3D[] ={5,1,7,3,6,2,8,4};
  int z3D[] ={3,1,4,2,7,5,8,6},I3D[]={1,2,3,4,5,6,7,8};
  int y3Dinv[]={3,7,1,5,4,8,2,6};
  int z2D[]={3,1,4,2}, I2D[]={1,2,3,4};
  int z1D[] ={1,2};
  int m;
  int b[4],d[4],b3D[8],d3D[8];
  int c[4][16],c3D[24][16];

  //edge definition in terms of vertex pairs
  int edges4D[32][2]={{1,2},{1,3},{2,4},{3,4},{1,5},{2,6},{3,7},{4,8},{5,6},{5,7},{6,8},{7,8},
  {1,9},{2,10},{3,11},{4,12},{5,13},{6,14},{7,15},{8,16},{9,10},{9,11},{10,12},{11,12},
  {9,13},{10,14},{11,15},{12,16},{13,14},{13,15},{14,16},{15,16}};
  int edges3D[12][2]={{1,2},{1,3},{2,4},{3,4},{1,5},{2,6},{3,7},{4,8},{5,6},{5,7},{6,8},{7,8}};
  int edges2D[4][2]={{1,2},{1,3},{2,4},{3,4}};
  int xEdges[32][2],yEdges[32][2],zEdges[32][2],tEdges[32][2];
  int z2DEdge[]={2,4,1,3};

  int dimension = 0;
  int vertices;
  int edges;
  int rotations;

  int i,j,k;

  //
  //input cube dimension
  //

  std::cout<<"input cube dimension = ";
  std::cin>>dimension;

  vertices = (int)pow(2.0,dimension);
  if(dimension==1)
  {
    edges = 1;
    rotations = 1;
    std::cout<<"name = "<<name1D<<"  vertices = "<<vertices<<"  edges = "<<edges<<"  rotations = "<<rotations<<std::endl;
  }
  
  //
  //marching squares
  //
  if(dimension==2)
  {
    edges=4;
    rotations=4;
    std::cout<<"name = "<<name2D<<"   vertices = "<<vertices<<"  edges = "<<edges<<"  rotations = "<<rotations<<std::endl;
    permute(I2D,I2D,Rotation2D[0],4);
    permute(z2D,Rotation2D[0],Rotation2D[1],4);
    permute(z2D,Rotation2D[1],Rotation2D[2],4);
    permute(z2D,Rotation2D[2],Rotation2D[3],4);
    std::cout<<"vertex permutaions"<<std::endl;
    for(i=0;i<4;i++)
    {
      for(j=0;j<4;j++)
      {
        group2D[i][j]=Rotation2D[i][j];
        std::cout<<group2D[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    //
    //permute edges by permuting vertex pairs
    //
    permute(I2D,I2D,Rotation2DEdge[0],4);
    permute(z2DEdge,Rotation2DEdge[0],Rotation2DEdge[1],4);
    permute(z2DEdge,Rotation2DEdge[1],Rotation2DEdge[2],4);
    permute(z2DEdge,Rotation2DEdge[2],Rotation2DEdge[3],4);
    std::cout<<"vertex permutaions"<<std::endl;
    for(i=0;i<4;i++)
    {
      for(j=0;j<4;j++)
      {
        group2DEdge[i][j]=Rotation2DEdge[i][j];
        std::cout<<group2DEdge[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    //
    //find patterns from 16 configurations
    //
    
    std::cout<<"permuted configurations 1   z    z^2    z^3"<<std::endl;
    for(i=0;i<16;i++)
    {
      intToBinary(i,vertices,b);
      for(j=0;j<4;j++)
      {
        std::cout<<b[j]<<",";
      }
      std::cout<<"   ";
      for(j=0;j<4;j++)
      {
        std::cout<<b[group2D[1][j]-1]<<",";
      }
      std::cout<<"   ";
      for(j=0;j<4;j++)
      {
        std::cout<<b[group2D[2][j]-1]<<",";
      }
      std::cout<<"   ";
      for(j=0;j<4;j++)
      {
        std::cout<<b[group2D[3][j]-1]<<",";
      }
      std::cout<<std::endl;
    }
    std::cout<<" permuted configurations in integer format" <<std::endl;
    for(i=0;i<16;i++)
    {
      intToBinary(i,vertices,b);
      c[0][i]=b[0]+2*b[1]+4*b[2]+8*b[3];
      std::cout<<c[0][i]<<"\t";
      for(j=0;j<4;j++)d[j]=b[group2D[1][j]-1];
      c[1][i]=d[0]+2*d[1]+4*d[2]+8*d[3];
      std::cout<<c[1][i]<<"\t";
      for(j=0;j<4;j++)d[j]=b[group2D[2][j]-1];
      c[2][i]=d[0]+2*d[1]+4*d[2]+8*d[3];
      std::cout<<c[2][i]<<"\t";
      for(j=0;j<4;j++)d[j]=b[group2D[3][j]-1];
      c[3][i]=d[0]+2*d[1]+4*d[2]+8*d[3];
      std::cout<<c[3][i]<<std::endl;
    }
    std::cout<<" pattern sets " <<std::endl;
    //remove dups 
    for(i=0;i<16;i++)
    {
      for(j=0;j<4;j++)
      {
        for(k=0;k<16;k++)
        {
          for(m=0;m<4;m++)
          {
            if(i!=k)
            {
              if(j!=m)
              {
                if(c[j][i] == c[m][k])c[m][k]=-1;
              }
            }
          }
        }
      }
    }
    for(i=0;i<16;i++)
    {
      if(c[0][i]!=-1)std::cout<<c[0][i]<<",";
    }
    std::cout<<std::endl;
  }//end 2D

  //
  //marching cubes
  //
  if(dimension==3)
  {
    edges=12;
    rotations=24;// cube
    std::cout<<"name = "<<name3D<<"    vertices = "<<vertices<<"  edges = "<<edges<<"  rotations = "<<rotations<<std::endl;
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
      
    //4 face vertex rotations form a cycle
    permute(I3D,I3D,Rotation3D[0],8);
    permute(z3D,Rotation3D[0],Rotation3D[1],8);
    permute(z3D,Rotation3D[1],Rotation3D[2],8);
    permute(z3D,Rotation3D[2],Rotation3D[3],8);
    std::cout<<"vertex permutaions"<<std::endl;
    for(i=0;i<4;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<8;j++)
      {
        std::cout<<Rotation3D[i][j]<<",";
      }
      std::cout<<std::endl;
    }

    std::cout<<"faces"<<std::endl;
    for(i=0;i<6;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<8;j++)
      {
        std::cout<<Faces3D[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    std::cout<<"rotations"<<std::endl;
   
    //group = rotation X faces
    std::cout<<"24 element 3D rotation group expressed as vertex permutations"<<std::endl;
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
      std::cout<<i+1<<"\t";
      for(j=0;j<8;j++)
      {
        std::cout<<group3D[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    //
    //the 12 edges defined by a pair of vertices are permuted using the 3D rotation group
    //
    std::cout<<"permuted edges defined by vertex pairs at end points= "<<edges<<std::endl;
    std::cout<<"edge    1       x       y      z "<<std::endl;
    
    //
    //permute the edges by permuting the vertex pairs
    //

    for(i=0;i<edges;i++)
    {
      xEdges[i][0]=x3D[edges3D[i][0]-1];
      xEdges[i][1]=x3D[edges3D[i][1]-1];
      yEdges[i][0]=y3D[edges3D[i][0]-1];
      yEdges[i][1]=y3D[edges3D[i][1]-1];
      zEdges[i][0]=z3D[edges3D[i][0]-1];
      zEdges[i][1]=z3D[edges3D[i][1]-1];
    }
    for(i=0;i<edges;i++)
    {
      std::cout<<i+1<<"\t"<<edges3D[i][0]<<","<<edges3D[i][1]<<"\t"<<xEdges[i][0]<<","<<xEdges[i][1]<<"\t"
        <<yEdges[i][0]<<","<<yEdges[i][1]<<"\t"<<zEdges[i][0]<<","<<zEdges[i][1]<<"\t"<<std::endl;
    }  
    std::cout<<std::endl;

    //
    //find the edge permutations using the 12 edge labels rather than vertex pairs
    //

    std::cout<<" edge permutaion generators 1, x, y, z "<<std::endl;
    for(i=0;i<edges;i++)
    {
      std::cout<<i+1<<",";
    }
    std::cout<<std::endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(xEdges[i][0]==edges3D[j][0]||xEdges[i][0]==edges3D[j][1])
        {
          if(xEdges[i][1]==edges3D[j][0]||xEdges[i][1]==edges3D[j][1])
          {
            std::cout<<j+1<<",";
            x3DEdge[i]=j+1;
          }
        }
      }
    }
    std::cout<<std::endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(yEdges[i][0]==edges3D[j][0]||yEdges[i][0]==edges3D[j][1])
        {
          if(yEdges[i][1]==edges3D[j][0]||yEdges[i][1]==edges3D[j][1])
          {
            std::cout<<j+1<<",";
            y3DEdge[i]=j+1;
          }
        }
      }
    }
    std::cout<<std::endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(zEdges[i][0]==edges3D[j][0]||zEdges[i][0]==edges3D[j][1])
        {
          if(zEdges[i][1]==edges3D[j][0]||zEdges[i][1]==edges3D[j][1])
          {
            std::cout<<j+1<<",";
            z3DEdge[i]=j+1;
          }
        }
      }
    }
    //
    // build 24 edge permutations = (1,z,z^2,z^3)X(1,x,x^2,x^3,y,y^3)
    //
    
    //6 faces edge permutaions
    for(i=0;i<edges;i++)
    {
      Faces3DEdge[1][i]=x3DEdge[i];
      Faces3DEdge[4][i]=y3DEdge[i];
    }
    permute(x3DEdge,Faces3DEdge[1],Faces3DEdge[2],12);
    permute(x3DEdge,Faces3DEdge[2],Faces3DEdge[3],12);
    permute(x3DEdge,Faces3DEdge[3],Faces3DEdge[0],12); 
    permute(y3DEdge,Faces3DEdge[4],Faces3DEdge[5],12);
    permute(y3DEdge,Faces3DEdge[5],Faces3DEdge[5],12); //create y^3
    //print
    std::cout<<std::endl;
    std::cout<<"edge faces"<<std::endl;
    for(i=0;i<6;i++)
    {
      for(j=0;j<12;j++)
      {
        std::cout<<Faces3DEdge[i][j]<<",";
      }
      std::cout<<std::endl;
    }

     //vertex rotations about face
    for(i=0;i<edges;i++)
    {
      Rotation3DEdge[1][i]=z3DEdge[i];
    }
    permute(z3DEdge,Rotation3DEdge[1],Rotation3DEdge[2],12);
    permute(z3DEdge,Rotation3DEdge[2],Rotation3DEdge[3],12);
    permute(z3DEdge,Rotation3DEdge[3],Rotation3DEdge[0],12);
    
    std::cout<<"edge rotations"<<std::endl;
    for(i=0;i<4;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<12;j++)
      {
        std::cout<<Rotation3DEdge[i][j]<<",";
      }
      std::cout<<std::endl;
    }
     //
    //group3DEdge = rotationEdge X facesEdge
    //
    std::cout<<"24 group"<<std::endl;
    for(i=0;i<4;i++)
    {
      for(j=0;j<6;j++)
      {
        k=j+6*i;
        permute(Rotation3DEdge[i],Faces3DEdge[j],group3DEdge[k],edges);
      }
    }
    for(i=0;i<24;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<edges;j++)
      {
        std::cout<<group3DEdge[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    std::cout<<" permuted configurations in integer format" <<std::endl;
    for(i=0;i<256;i++)
    {
      intToBinary(i,vertices,b3D);
      c3D[0][i]=b3D[0]+2*b3D[1]+4*b3D[2]+8*b3D[3]+16*b3D[4]+32*b3D[5]+64*b3D[6]+128*b3D[7];
      std::cout<<c3D[0][i]<<",";
      //
      // permute configurations for the 24 element rotation group
      //
      for(k=1;k<24;k++)
      {
        for(j=0;j<8;j++)d3D[j]=b3D[group3D[k][j]-1];
        c3D[k][i]=d3D[0]+2*d3D[1]+4*d3D[2]+8*d3D[3]+16*d3D[4]+32*d3D[5]+64*d3D[6]+128*d3D[7];
        std::cout<<c3D[k][i]<<",";
      }
      std::cout<<c3D[2][i]<<std::endl;

    }
  }//end 3D

  //
  //Marching Hyper Cubes
  //

  if(dimension==4)
  {
    edges=32;
    rotations=192;
    std::cout<<"name = "<<name4D<<"    vertices = "<<vertices<<"  edges = "<<edges<<"  rotations = "<<rotations<<std::endl;
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
    permute(I4D,I4D,Rotation4D[0],16);
    permute(z4D,Rotation4D[0],Rotation4D[1],16);
    permute(z4D,Rotation4D[1],Rotation4D[2],16);
    permute(z4D,Rotation4D[2],Rotation4D[3],16);
    
    std::cout<<"faces"<<std::endl;
    for(i=0;i<6;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<16;j++)
      {
        std::cout<<Faces4D[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    std::cout<<"rotations"<<std::endl;
    for(i=0;i<4;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<16;j++)
      {
        std::cout<<Rotation4D[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    //
    //group = rotation X faces
    //
    std::cout<<"24 group"<<std::endl;
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
      std::cout<<i+1<<"\t";
      for(j=0;j<16;j++)
      {
        std::cout<<group4D[i][j]<<",";
      }
      std::cout<<std::endl;
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

    std::cout<<"cube rotation"<<std::endl;
    for(i=0;i<8;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<16;j++)
      {
        std::cout<<Cubes4D[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    //group = rotation X faces
    std::cout<<"192 group"<<std::endl;
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
      std::cout<<i+1<<"\t";
      for(j=0;j<16;j++)
      {
        std::cout<<group4D[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    //
    //the 32 edges defined by a pair of vertices are permuted using the 4D rotation group
    //
    std::cout<<"permuted edges defined by vertex pairs at end points= "<<edges<<std::endl;
    std::cout<<"edge    1       x       y      z     t"<<std::endl;
    
    //
    //permute the edges by permuting the vertex pairs
    //

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
      std::cout<<i+1<<"\t"<<edges4D[i][0]<<","<<edges4D[i][1]<<"\t"<<xEdges[i][0]<<","<<xEdges[i][1]<<"\t"
        <<yEdges[i][0]<<","<<yEdges[i][1]<<"\t"<<zEdges[i][0]<<","<<zEdges[i][1]<<"\t"<<tEdges[i][0]<<","<<tEdges[i][1]<<std::endl;
    }  
    std::cout<<std::endl;
    //
    //find the edge permutations using the 32 edge labels rather than vertex pairs
    //

    std::cout<<" edge permutaion generators 1, x, y, z, t "<<std::endl;
    for(i=0;i<edges;i++)
    {
      std::cout<<i+1<<",";
    }
    std::cout<<std::endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(xEdges[i][0]==edges4D[j][0]||xEdges[i][0]==edges4D[j][1])
        {
          if(xEdges[i][1]==edges4D[j][0]||xEdges[i][1]==edges4D[j][1])
          {
            std::cout<<j+1<<",";
            x4DEdge[i]=j+1;
          }
        }
      }
    }
    std::cout<<std::endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(yEdges[i][0]==edges4D[j][0]||yEdges[i][0]==edges4D[j][1])
        {
          if(yEdges[i][1]==edges4D[j][0]||yEdges[i][1]==edges4D[j][1])
          {
            std::cout<<j+1<<",";
            y4DEdge[i]=j+1;
          }
        }
      }
    }
    std::cout<<std::endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(zEdges[i][0]==edges4D[j][0]||zEdges[i][0]==edges4D[j][1])
        {
          if(zEdges[i][1]==edges4D[j][0]||zEdges[i][1]==edges4D[j][1])
          {
            std::cout<<j+1<<",";
            z4DEdge[i]=j+1;
          }
        }
      }
    }
    std::cout<<std::endl;
    for(i=0;i<edges;i++)
    {
      for(j=0;j<edges;j++)
      {
        if(tEdges[i][0]==edges4D[j][0]||tEdges[i][0]==edges4D[j][1])
        {
          if(tEdges[i][1]==edges4D[j][0]||tEdges[i][1]==edges4D[j][1])
          {
            std::cout<<j+1<<",";
            t4DEdge[i]=j+1;
          }
        }
      }
    }
    std::cout<<std::endl;

    //
    // build 192 edge permutations = (1,t,t^2,t^3,t^4,t^5,t^6,t^7)X(1,z,z^2,z^3)X(1,x,x^2,x^3,y,y^3)
    //
    
    //6 faces edge permutaions
    for(i=0;i<32;i++)
    {
      Faces4DEdge[1][i]=x4DEdge[i];
      Faces4DEdge[4][i]=y4DEdge[i];
    }
    permute(x4DEdge,Faces4DEdge[1],Faces4DEdge[2],32);
    permute(x4DEdge,Faces4DEdge[2],Faces4DEdge[3],32);
    permute(x4DEdge,Faces4DEdge[3],Faces4DEdge[0],32); 
    permute(y4DEdge,Faces4DEdge[4],Faces4DEdge[5],32);
    permute(y4DEdge,Faces4DEdge[5],Faces4DEdge[5],32); //create y^3
    //print
    std::cout<<"edge faces"<<std::endl;
    for(i=0;i<6;i++)
    {
      for(j=0;j<32;j++)
      {
        std::cout<<Faces4DEdge[i][j]<<",";
      }
      std::cout<<std::endl;
    }

     //vertex rotations about face
    for(i=0;i<32;i++)
    {
      Rotation4DEdge[1][i]=z4DEdge[i];
    }
    permute(z4DEdge,Rotation4DEdge[1],Rotation4DEdge[2],32);
    permute(z4DEdge,Rotation4DEdge[2],Rotation4DEdge[3],32);
    permute(z4DEdge,Rotation4DEdge[3],Rotation4DEdge[0],32);
    
    std::cout<<"edge rotations"<<std::endl;
    for(i=0;i<4;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<32;j++)
      {
        std::cout<<Rotation4DEdge[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    //

    //
    //8 cube edge rotations along an axis of eight fold symmetry
    //
    for(i=0;i<32;i++)
    {
      Cubes4DEdge[1][i]=t4DEdge[i];
    }
    permute(t4DEdge,Cubes4DEdge[1],Cubes4DEdge[2],32);
    permute(t4DEdge,Cubes4DEdge[2],Cubes4DEdge[3],32);
    permute(t4DEdge,Cubes4DEdge[3],Cubes4DEdge[4],32);
    permute(t4DEdge,Cubes4DEdge[4],Cubes4DEdge[5],32);
    permute(t4DEdge,Cubes4DEdge[5],Cubes4DEdge[6],32);
    permute(t4DEdge,Cubes4DEdge[6],Cubes4DEdge[7],32);
    permute(t4DEdge,Cubes4DEdge[7],Cubes4DEdge[0],32);

    std::cout<<"cube edge rotation"<<std::endl;
    for(i=0;i<8;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<32;j++)
      {
        std::cout<<Cubes4DEdge[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    //
    //group4DEdge = rotationEdge X facesEdge
    //
    std::cout<<"24 group"<<std::endl;
    for(i=0;i<4;i++)
    {
      for(j=0;j<6;j++)
      {
        k=j+6*i;
        permute(Rotation4DEdge[i],Faces4DEdge[j],group4DEdge[k],32);
      }
    }
    for(i=0;i<24;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<32;j++)
      {
        std::cout<<group4DEdge[i][j]<<",";
      }
      std::cout<<std::endl;
    }
    std::cout<<"192 group"<<std::endl;
    for(i=0;i<8;i++)
    {
      for(j=0;j<24;j++)
      {
        k=j+24*i;
        permute(Cubes4DEdge[i],group4DEdge[j],group4DEdge[k],32);
      }
    }
    //print 4D edge permutations
    for(i=0;i<192;i++)
    {
      std::cout<<i+1<<"\t";
      for(j=0;j<32;j++)
      {
        std::cout<<group4DEdge[i][j]<<",";
      }
      std::cout<<std::endl;
    }
  }//end 4D
  if(dimension == 0) 
  {
    std::cout<<"error: the dimension range is from one to four"<<std::endl;
  }
  std::cin >> i;
  return 0;
}
void permute(int *a, int *b, int *c, int elements)
{
  int i;
  for(i=0;i<elements;i++)
    {
    c[i]=a[b[i]-1];
    }
}
void intToBinary(int n, int vertices, int *binary)
{
  int i;
  int *pB;
  for(i=0;i<vertices;i++)
  {
    pB=binary+i;
    if(n==2*(n/2))
    {
      *pB=0;
    }
    else
    {
      *pB=1;
    }
    n=n/2;
  }
}

