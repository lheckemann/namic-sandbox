
//  ===============================================
//  the Icosahedron subdivision -> phi,theta sampling + triangles
//  ===============================================

#include <math.h>

#include <iostream>
#include <fstream>
using namespace std;

#include "Icosahedron_subdiv.h"

#define X .525731112119133606
#define Z .850650808352039932
#define MAX_N_VERTEX 100000
#define MAX_N_TRIANGS 200000


/* Daten des Ikosaeders */

static double vert[12 * 3] = {
   -X, 0.0, Z,   X, 0.0, Z,   -X, 0.0, -Z,   X, 0.0, -Z,
   0.0, Z, X,   0.0, Z, -X,   0.0, -Z, X,   0.0, -Z, -X,
   Z, X, 0.0,   -Z, X, 0.0,   Z, -X, 0.0,   -Z, -X, 0.0
};
static int triang[20 * 3] = {
   0,4,1,   0,9,4,   9,5,4,   4,5,8,   4,8,1,
   8,10,1,   8,3,10,   5,3,8,   5,2,3,   2,7,3,
   7,10,3,   7,6,10,   7,11,6,   11,0,6,   0,1,6,
   6,1,10,   9,0,11,   9,11,2,   9,2,5,   7,2,11
};
static int edge[30 * 2] = {
   0,1,   0,4,   0,6,   0,9,   0,11,   1,4,   1,6,   1,8,   1,10,   2,3,
   2,5,   2,7,   2,9,   2,11,   3,5,   3,7,   3,8,   3,10,   4,5,   4,8,
   4,9,   5,8,   5,9,   6,7,   6,10,   6,11,   7,10,   7,11,   8,10,   9,11
};

void set_up_icosahedron_triangs(int       subdiv,
        int      &n_vert,
        double * &icos,
        int      &n_triangs,
        int *    &triangs)
{
  
   int i, n, m, k, numtriags;
   double x1, x2, y1, y2, z1, z2, x3, y3, z3; 
   double dx12, dy12, dz12, dx23, dy23, dz23;
   double length;
   
   double epsilon = 0.00001;

   if (icos) {delete icos; icos = NULL; }
   if (triangs) {delete triangs; triangs = NULL;}

   n=0;
   if(subdiv > 2) {
      for(i=1; i<(subdiv-1); i++) 
   n += i;
   }
   n_vert = 12 + (subdiv - 1)*30 + n*20;

   icos = new double [n_vert * 3];
   double *all_triangs = new double [(n_vert * 4) * 9]; 
   //max 3* n_vert triangles, each 9 entries since all_triangs has the coordinates rather than index

   memcpy(icos, vert, 12 * sizeof(double) * 3);

   k=12;

   for(i=0; i<30; i++) {
     x1 = vert[edge[i * 2 + 0]  * 3 + 0];
     y1 = vert[edge[i* 2 + 0]  * 3 + 1];
      z1 = vert[edge[i* 2 + 0]  * 3 + 2];
      x2 = vert[edge[i* 2 + 1]  * 3 + 0];
      y2 = vert[edge[i* 2 + 1]  * 3 + 1];
      z2 = vert[edge[i* 2 + 1]  * 3 + 2];
      dx12 = (x2 - x1)/subdiv;
      dy12 = (y2 - y1)/subdiv;
      dz12 = (z2 - z1)/subdiv;
      for(n=1; n<subdiv; n++) {
   icos[k * 3 + 0] = x1 + n*dx12;
   icos[k * 3 + 1] = y1 + n*dy12;
   icos[k * 3 + 2] = z1 + n*dz12;
   length = sqrt(icos[k * 3 + 0]*icos[k * 3 + 0]+
           icos[k * 3 + 1]*icos[k * 3 + 1]+
           icos[k * 3 + 2]*icos[k * 3 + 2]);
   icos[k * 3 + 0] /= length;
   icos[k * 3 + 1] /= length;
   icos[k * 3 + 2] /= length;
   k++;
      }
   }

   if(subdiv > 2) {
      for(i=0; i<20; i++) {
   x1 = vert[triang[i * 3 + 0]  * 3 + 0];
   y1 = vert[triang[i * 3 + 0]  * 3 + 1];
   z1 = vert[triang[i * 3 + 0]  * 3 + 2];
   x2 = vert[triang[i * 3 + 1]  * 3 + 0];
   y2 = vert[triang[i * 3 + 1]  * 3 + 1];
   z2 = vert[triang[i * 3 + 1]  * 3 + 2];
   x3 = vert[triang[i * 3 + 2]  * 3 + 0];
   y3 = vert[triang[i * 3 + 2]  * 3 + 1];
   z3 = vert[triang[i * 3 + 2]  * 3 + 2];
   dx12 = (x2 - x1)/subdiv;
   dy12 = (y2 - y1)/subdiv;
   dz12 = (z2 - z1)/subdiv;
   dx23 = (x3 - x2)/subdiv;
   dy23 = (y3 - y2)/subdiv;
   dz23 = (z3 - z2)/subdiv;

   n = 1;
   do {
      for(m=1; m<=n; m++) {
         icos[k * 3 + 0] = x1 + (n+1)*dx12 + m*dx23;
         icos[k * 3 + 1] = y1 + (n+1)*dy12 + m*dy23;
         icos[k * 3 + 2] = z1 + (n+1)*dz12 + m*dz23;
         length = sqrt(icos[k * 3 + 0]*icos[k * 3 + 0]+
           icos[k * 3 + 1]*icos[k * 3 + 1]+
           icos[k * 3 + 2]*icos[k * 3 + 2]);
         icos[k * 3 + 0] /= length;
         icos[k * 3 + 1] /= length;
         icos[k * 3 + 2] /= length;
         k++;
      }
      n++;
   } 
   while( n<=(subdiv-2) );
      }
   }

   numtriags=0;
   // get triangulation
   if (subdiv > 1) {
      for(i=0; i<20; i++) {
   x1 = vert[triang[i * 3 + 0]  * 3 + 0];
   y1 = vert[triang[i * 3 + 0]  * 3 + 1];
   z1 = vert[triang[i * 3 + 0]  * 3 + 2];
   x2 = vert[triang[i * 3 + 1]  * 3 + 0];
   y2 = vert[triang[i * 3 + 1]  * 3 + 1];
   z2 = vert[triang[i * 3 + 1]  * 3 + 2];
   x3 = vert[triang[i * 3 + 2]  * 3 + 0];
   y3 = vert[triang[i * 3 + 2]  * 3 + 1];
   z3 = vert[triang[i * 3 + 2]  * 3 + 2];
   dx12 = (x2 - x1)/subdiv;
   dy12 = (y2 - y1)/subdiv;
   dz12 = (z2 - z1)/subdiv;
   dx23 = (x3 - x2)/subdiv;
   dy23 = (y3 - y2)/subdiv;
   dz23 = (z3 - z2)/subdiv;

   n = 1;
   do {
      for(m=1; m<=n; m++) {
        // Draw lower triangle
        all_triangs[numtriags * 3 + 0] = x1 + n*dx12 + m*dx23;
        all_triangs[numtriags * 3 + 1] = y1 + n*dy12 + m*dy23;
        all_triangs[numtriags * 3 + 2] = z1 + n*dz12 + m*dz23;
        length = sqrt(all_triangs[numtriags * 3 + 0]*all_triangs[numtriags * 3 + 0]+
          all_triangs[numtriags * 3 + 1]*all_triangs[numtriags * 3 + 1]+
          all_triangs[numtriags * 3 + 2]*all_triangs[numtriags * 3 + 2]);
        all_triangs[numtriags * 3 + 0] /= length;
        all_triangs[numtriags * 3 + 1] /= length;
        all_triangs[numtriags * 3 + 2] /= length;
        numtriags++;
        all_triangs[numtriags * 3 + 0] = x1 + (n-1)*dx12 + (m-1)*dx23;
        all_triangs[numtriags * 3 + 1] = y1 + (n-1)*dy12 + (m-1)*dy23;
        all_triangs[numtriags * 3 + 2] = z1 + (n-1)*dz12 + (m-1)*dz23;
        length = sqrt(all_triangs[numtriags * 3 + 0]*all_triangs[numtriags * 3 + 0]+
          all_triangs[numtriags * 3 + 1]*all_triangs[numtriags * 3 + 1]+
          all_triangs[numtriags * 3 + 2]*all_triangs[numtriags * 3 + 2]);
        all_triangs[numtriags * 3 + 0] /= length;
        all_triangs[numtriags * 3 + 1] /= length;
        all_triangs[numtriags * 3 + 2] /= length;
        numtriags++;
        all_triangs[numtriags * 3 + 0] = x1 + n*dx12 + (m-1)*dx23;
        all_triangs[numtriags * 3 + 1] = y1 + n*dy12 + (m-1)*dy23;
        all_triangs[numtriags * 3 + 2] = z1 + n*dz12 + (m-1)*dz23;
        length = sqrt(all_triangs[numtriags * 3 + 0]*all_triangs[numtriags * 3 + 0]+
          all_triangs[numtriags * 3 + 1]*all_triangs[numtriags * 3 + 1]+
          all_triangs[numtriags * 3 + 2]*all_triangs[numtriags * 3 + 2]);
        all_triangs[numtriags * 3 + 0] /= length;
        all_triangs[numtriags * 3 + 1] /= length;
        all_triangs[numtriags * 3 + 2] /= length;
        numtriags++;
        if ( m != n ) {
    // Draw lower left triangle
    all_triangs[numtriags * 3 + 0] = x1 + n*dx12 + m*dx23;
    all_triangs[numtriags * 3 + 1] = y1 + n*dy12 + m*dy23;
    all_triangs[numtriags * 3 + 2] = z1 + n*dz12 + m*dz23;
    length = sqrt(all_triangs[numtriags * 3 + 0]*all_triangs[numtriags * 3 + 0]+
            all_triangs[numtriags * 3 + 1]*all_triangs[numtriags * 3 + 1]+
            all_triangs[numtriags * 3 + 2]*all_triangs[numtriags * 3 + 2]);
    all_triangs[numtriags * 3 + 0] /= length;
    all_triangs[numtriags * 3 + 1] /= length;
    all_triangs[numtriags * 3 + 2] /= length;
    numtriags++;
    all_triangs[numtriags * 3 + 0] = x1 + (n-1)*dx12 + m*dx23;
    all_triangs[numtriags * 3 + 1] = y1 + (n-1)*dy12 + m*dy23;
    all_triangs[numtriags * 3 + 2] = z1 + (n-1)*dz12 + m*dz23;
    length = sqrt(all_triangs[numtriags * 3 + 0]*all_triangs[numtriags * 3 + 0]+
            all_triangs[numtriags * 3 + 1]*all_triangs[numtriags * 3 + 1]+
            all_triangs[numtriags * 3 + 2]*all_triangs[numtriags * 3 + 2]);
    all_triangs[numtriags * 3 + 0] /= length;
    all_triangs[numtriags * 3 + 1] /= length;
    all_triangs[numtriags * 3 + 2] /= length;
    numtriags++;
    all_triangs[numtriags * 3 + 0] = x1 + (n-1)*dx12 + (m-1)*dx23;
    all_triangs[numtriags * 3 + 1] = y1 + (n-1)*dy12 + (m-1)*dy23;
    all_triangs[numtriags * 3 + 2] = z1 + (n-1)*dz12 + (m-1)*dz23;
    length = sqrt(all_triangs[numtriags * 3 + 0]*all_triangs[numtriags * 3 + 0]+
            all_triangs[numtriags * 3 + 1]*all_triangs[numtriags * 3 + 1]+
            all_triangs[numtriags * 3 + 2]*all_triangs[numtriags * 3 + 2]);
    all_triangs[numtriags * 3 + 0] /= length;
    all_triangs[numtriags * 3 + 1] /= length;
    all_triangs[numtriags * 3 + 2] /= length;
    numtriags++;
        }
      }
      n++;
   } while( n<=subdiv );
      }
   }
   
   //std::cout << k << " , " << n_vert << "," << numtriags << endl;
   // indexing of triangs
   if (subdiv == 1) {
     numtriags = 20 * 3;
     triangs = new int[numtriags];
     memcpy(triangs, triang, numtriags * sizeof(int));
   } else {
     //find for every point in triangle list the corresponding index in icos
     
     triangs = new int[numtriags];
     // initialize
     for (i=0; i < numtriags; i ++) {
       triangs[i] = -1;
     }

     // find indexes
     for(i=0; i<n_vert; i++) {
       for (int j = 0; j < numtriags; j++) {
   if (triangs[j] < 0) {
    if ( (fabs(icos[i * 3 + 0] - all_triangs[j * 3 + 0]) < epsilon) && 
         (fabs(icos[i * 3 + 1] - all_triangs[j * 3 + 1]) < epsilon) && 
         (fabs(icos[i * 3 + 2] - all_triangs[j * 3 + 2]) < epsilon ) ) {
       triangs[j] = i;
     }
   }
       }
     }
     for (i=0; i < numtriags; i ++) {
       if (triangs[i] == -1)
   cerr << " - " << i << " :" << all_triangs[i * 3 + 0] 
        << "," << all_triangs[i * 3 + 1] << "," << all_triangs[i * 3 + 2] << endl;
     }
     
   }
   // numtriags is the number of vertices in triangles -> divide it by 3 
   n_triangs = numtriags / 3;

   delete all_triangs;

}


