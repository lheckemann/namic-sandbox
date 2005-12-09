/* this file and all the referenced functions used in rgm-deform.cxx were
 * taken from the deformation code written by Matthieu Ferrant */


#ifndef _TETRA_MESH_H
#define _TETRA_MESH_H

#ifdef __cplusplus
extern "C"{
#endif
#include <petsc.h>
#include <petscmat.h>
#include <petscvec.h>
#include <petscksp.h>

float exactinit();
double orient3d(double*, double*, double*, double*);

#ifdef __cplusplus
}
#endif

#include <iostream>

//#include "mesh.h"
//#include "myUtils.h"
//#include "samfunc.h"
//#include <math.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <time.h>
//#include <assert.h>
//#include "tcl.h"
//#include <limits.h>
//#include <sys/stat.h> /* for struct stat */
//#include "zlib.h" /* for reading and writing gzipped data */
//#include "ReadLabels.h"
//#include "ptrmaille.h"
//#include "ComputeShape.h"


#define MAX_NEIGHBORS 86
#define NIL -1000
#define B_TET -2000
#define B_COND -3000
#define B_IMAGE -4000

/* default value for outer image faces boundary condition */


#define ALL_LABELS -1000
#define ALL_LABELS_BUT_ZERO -2000

#define ASCII 0
#define BIN 1
#define ASCII_GZIP 2
#define BIN_GZIP 3
//#define VTK_ASCII 4
#define VTK_BIN 5

#define MAX_TETS_EDGE 30

#define BIG 65536

#define SQR(x) ((x)*(x))
//#define MAX(x,y) ( (x) >= (y) ? (x) : (y))
//#define MIN(x,y) ( (x) <= (y) ? (x) : (y))
#define ODD(x) ( (x) % 2 )
#define LARGE_FLOAT 1.0e+38F
#define SMALL_NUMBER 1.0e-12
#define MAX_LABEL 15

namespace itk{
class PETScDeformWrapper{

public:

struct Point_s {
  float Ind[3];
  int Number;
};
typedef struct Point_s Point;

struct LP_s {
Point **P;
int NPoints;
};
typedef struct LP_s LP;

/* Contains the boundary conditions numbers as in the FE matrix
 * , and the corresponding displacement (idem for forces)
 *
 */

struct IndDispList_s{
  int NIndices;
  int *Indices;
  double *Displacements; /*need doubles for PETSc */
};
typedef struct IndDispList_s IndDispList;

struct _Label_s {
  int value;  /* the label that was assigned to this element of the tet */
  float E;    /* Young's modulus */
  float nu;   /* Poisson's ratio */
};
typedef struct _Label_s _Label;


struct LabelInfo_s{
  int NLabels;
  _Label *L[MAX_LABEL];
};
typedef struct LabelInfo_s LabelInfo;

/**********************************************************************
 *
 * tetra_mesh.h : header file for tetra_mesh.c
 *
 * Routine creating, reading, writing, extracting,
 * boundary surfaces, copying,resizing tetrahedral mesh structures
 * 
 * Author: M. Ferrant (ferrant@mit.edu, ferrant@tele.ucl.ac.be)
 * 
 * Dates:  Created 10/20/98
 *         Latest revision : 9/18/2000
 *
 **********************************************************************/
/********************************************************************** 
 *
 * File formats of tetra_meshes :
 * 1. ascii : # BBOX LABELED ASCII : 3 optional keywords
 *              ncells nfaces nboundfaces nvertices
 *        x y z coords of vertices
 *        for every face : cell0 cell1 vert0 vert1 vert2
 *        for every boundary face : facenb BC vert0 vert1 vert2
 *        if labels : labelcell0 ... labelcelln
 *
 * 2. binary : # BBOX LABELED BINARY : 3 optional keywords
 *             ncells nfaces nboundfaces nvertices
 *             x coords of all verts, 
 *             y coords of all verts,
 *             z coords of all verts
 *             face info (fwrite struct face for every face)
 *             boundary face info : no vert info
 *             label info
 * 3. gzipped : reads binary or ascii gzipped in the same format as above
 *              writes out gzipped binary or ascii files as well
 *              write modes : ASCII BIN ASCII_GZIP BIN_GZIP
 * 
 *************************************************************************/


struct face { 
              int cell[2]; /* two cells surrounding the face */
              int vert[3]; /*  3 vertices : orientation -> direction from 
          *  first cell to second by right hand rule 
          *  =>normal outwards for first cell
          */
              int edge[3];  /* 3 edges forming the face */
            };

struct cell { 
              int face[4];  /* 4 faces of the tetrahedron */
              int vert[4];  /* 4 vertices of the tetrahedron */               
              int edge[6]; /* 6 edges forming the tet */
              int label;    /* tet's structure label value */
           };/*orientation : 3 vertices of 1 face counter-clockwise + 1 vert*/


struct tetra_mesh {
                    int edges_present; /*tells if the edge's structure is present in mesh */
                    int faces_present;/*tells if the face's structure is present in mesh */
                    int ncells;      /* amount of tetrahedra in the mesh */
                    int nfaces; /* amount of triangular faces in the mesh */
                    int nboundfaces; /* amount of boundary triangular faces */
                    int nvertices;   /* amount of vertices in mesh */
                    int nboundvertices; /* nb of vert. of the boundary surf */
                    int nedges; /* amount of edges present in the mesh*/
                    float **vertices;   /* vertices in array vertices[i][j] i is the coordinate (x,y,z) j is the number of the vertex */
                    float *x,*y,*z;  /* coordinates of the vertices */
                    struct face **face;  /* list of faces w/cells&vertices */
                    struct cell **cell;/*list of tet. cells w/faces&vertices*/
                    int *face_bc;  /* list of boundary faces & their corresponding boundary condition */
                    int *edge;    /*list of pairs of vertices forming an edge */
                    float *face_color; /* list of faces and their corresponding r,g,b colors ? why not vertices -> scalar field ? */
                    float bb[2][3];  /*bounding box */
                    float vox_size[3]; /* aspect ratio of voxels from which the mesh was created */
                  };

int myid, numprocs;
double *vertices;
unsigned* tetras;
unsigned* surface_vertices;
int n_vertices, n_surface_vertices, n_tetras, n_surface_faces, N_iter;


void ComputeTetInfo(struct tetra_mesh *mesh_in,
        int CellNumber,
        int v[4],
        float center[3],
        float vec[4][3]);
void calc_Kel( PetscScalar *B1, PetscScalar *D, PetscScalar *B2, 
  float Vel, PetscScalar Kel[9]);

void ComputeShape(float vec[4][3], float a[4],float b[4],float c[4], float d[4], float *Vel);
void ComputeBD(float b[4],float c[4],float d[4],float Vel,
  PetscScalar B[4][18],PetscScalar D[36], float E, float nu);

struct tetra_mesh *create_tetra_mesh(int ncells, int nfaces, int nboundfaces, int nvertices, int nedges);

struct tetra_mesh *copy_tetra_mesh(struct tetra_mesh *mesh);

struct tetra_mesh *add_edge_info_tetra_mesh(struct tetra_mesh *mesh);
int Deform(struct tetra_mesh *mesh, IndDispList *bc, 
  IndDispList *nodeForces, double e, double nu);
int Transpose3x3(PetscScalar M[9]);
int **get_vertex_connectivity_list_tetra_mesh(struct tetra_mesh *mesh);
struct cell **reshuffle_cell_numbers_tetra_mesh(struct tetra_mesh *mesh, int interval);
double get_min_edge_len(struct tetra_mesh *mesh);
void Order2(int*, int*);
int IsInRange(int ind,int Istart,int Iend);
float calc_det_3(float a[3],float b[3],float c[3]);

struct tetra_mesh *m_Mesh;

};

} // end namespace itk

#endif /* _TETRA_MESH_H */


