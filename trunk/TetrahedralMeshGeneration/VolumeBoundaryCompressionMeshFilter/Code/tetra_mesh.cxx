#include "tetra_mesh.h"
#include <assert.h>
#include <map>

#define RESAMPLE 1
#define RESAMPLE_COEF 3
//#define MODEL_SPHERE 1

namespace itk{  

int PETScDeformWrapper::Transpose3x3(PetscScalar M[9]){
  int i,j;
  PetscScalar Temp[9];
  for (i=0;i<3;i++) for (j=0;j<3;j++) Temp[3*i+j] = M[3*j+i];  
  for (i=0;i<9;i++)    M[i] = Temp[i];
  return 1;
}

/* Computes the local 3x3 rigidity matrix Kel 
 * + squared differences minimization 
 * for a given tet 
 */

void PETScDeformWrapper::calc_Kel( PetscScalar *B1, PetscScalar *D, PetscScalar *B2, 
    float Vel, PetscScalar Kel[9]){
  int i,j,k; 
  float sum;
  PetscScalar B_temp[18];  

  /* compute Btransposed*D */
  for (i=0;i<3;i++){
    for (k=0;k<6;k++){
      for (j=0,sum=0;j<6;j++)
        sum += B1[i + 3*j]*D[k + 6*j];

      B_temp[k + 6*i]=sum;
    }
  }

  /* multiplies previous result with B matrix */ 
  for (i=0;i<3;i++){
    for (k=0;k<3;k++){
      for (j=0,sum=0;j<6;j++)
        sum += B_temp[j+6*i]*B2[k+3*j];

      Kel[k+3*i]=sum;
    }
  }

  for (i=0;i<9;i++){
    Kel[i] *= Vel;
  }
}

/*#define CST (E*(1-nu))/((1+nu)*(1-2*nu))  */
/* CST multiplies elasticity matrix D and can be different for every tet */

void PETScDeformWrapper::ComputeBD(float b[4],float c[4],float d[4],float Vel,
  PetscScalar B[4][18], PetscScalar D[36], float E, float nu){
  int j,k;
  extern int myid;

  double CST;

  assert(nu<0.5);
  assert(E!=0);
  CST = (double) (E*(1-nu))/((1+nu)*(1-2*nu)) ;

  /* Calculate the B matrices for each vertice of tet i*/
  for (j=0;j<4;j++){
    if (Vel > 0) {
      B[j][0]=b[j]/(6*Vel);   B[j][1]=0;              B[j][2]=0; 
      B[j][3]=0;              B[j][4]=c[j]/(6*Vel);   B[j][5]=0; 
      B[j][6]=0;              B[j][7]=0;              B[j][8]=d[j]/(6*Vel);
      B[j][9]=c[j]/(6*Vel);   B[j][10]=b[j]/(6*Vel);  B[j][11]=0; 
      B[j][12]=0;             B[j][13]=d[j]/(6*Vel);  B[j][14]=c[j]/(6*Vel); 
      B[j][15]=d[j]/(6*Vel);  B[j][16]=0;             B[j][17]=b[j]/(6*Vel); 
    }
    else {
      for (k=0;k<18;k++)
        B[j][k] = 0;
    }
  }

  /* Compute the elasticity matrix D for tetrahedron i */
  D[0]=CST;             D[1]=(CST*nu)/(1-nu);           D[2]=(CST*nu)/(1-nu);   D[3]=D[4]=D[5]=0;
  D[6]=(CST*nu)/(1-nu); D[7]=CST;D[8]=(CST*nu)/(1-nu);  D[9]=D[10]=D[11]=0;
  D[12]=(CST*nu)/(1-nu);D[13]=(CST*nu)/(1-nu);          D[14]=CST;              D[15]=D[16]=D[17]=0;
  D[18]=D[19]=D[20]=0; D[21]=(CST*(1-2*nu))/(2*(1-nu)); D[22]=D[23]=0;
  D[24]=D[25]=D[26]=D[27]=0; D[28]=(CST*(1-2*nu))/(2*(1-nu)); D[29]=0;
  D[30]=D[31]=D[32]=D[33]=D[34]=0; D[35]=(CST*(1-2*nu))/(2*(1-nu));
}

int PETScDeformWrapper::Deform(struct PETScDeformWrapper::tetra_mesh *mesh, PETScDeformWrapper::IndDispList *bc, 
  PETScDeformWrapper::IndDispList *nodeForces, double E_coef, double NU_coef){
//  extern int myid,numprocs;/*MPI stuff : no of processor, and amt of procs */

  int i,j,k,l,m,v[4],ind_i[3],ind_j[3];
  /*v contains the numbers of the tet's vertices */
  float center[3]; /*contains the coords of the center of the tet */
  float a[4],b[4],c[4],d[4],Vel; /*tet's sub det's and its Volume */
  float vec[4][3]; /* Contains the 4 vertices' x,y,z coord */
  int LabelNumber;

  /* PETSC stuff */
  Mat K_global; 
  Vec F_global,u_displ,v_scatter; /*u_displ will contain the solution */
  VecScatter scatter;
  IS from, to, ISBoundaryNodes,ISForceNodes;
  PetscScalar Kel[9], *disp_vect; /* F_local[12] might be needed if int. forces */
  PetscScalar one=1.0,zero=0.0;

  PetscScalar B[4][18]; /* B[i] 6x3 strain matrixes D = elast. mat*/ 
  PetscScalar D[36];  /*elasticity matrix ! symmetric matrix */

  /* Solver stuff */
  int n,ierr,its=5000,Istart,Iend,in_range,bandWidth,procBandWidth;
  //SLES sles;
  PC pc;
  KSP ksp;
  PetscMap map;
  PetscLogDouble StartTime,EndTime, AssemblyStart,AssemblyEnd,SolveStart,SolveEnd ;

  int **connectivity_list, nzNumber,*nzDiagTable,*nzNonDiagTable,numberOfNZDiag,numberOfNZNonDiag,max,my_NRows,xyzIndex;

  char *func_name = "deform.c";

  /**************************************************************************/

  /* get connectivity list */
  connectivity_list = get_vertex_connectivity_list_tetra_mesh(mesh);
  
  ierr = PetscGetTime(&StartTime);CHKERRQ(ierr);

  /* reshuffle vertices to improve load balancing between processors */
  if (numprocs > 1)
    mesh->cell = reshuffle_cell_numbers_tetra_mesh(mesh,numprocs);

  /* Creation of global matrices, or set to zero if they already exist */
  n = 3*mesh->nvertices;  

  /* now compute local matrix sizes on this processor 
   * given the global size of the problem n
   */
  ierr = PetscMapCreateMPI(PETSC_COMM_WORLD,PETSC_DECIDE,n,&map);CHKERRQ(ierr);
  ierr = PetscMapGetLocalRange(map,&Istart,&Iend); CHKERRQ(ierr);
  ierr = PetscMapDestroy(map); CHKERRQ(ierr);
  my_NRows = Iend - Istart;

  /* for storing number of non-zeros for every line of the matrix */
  assert( (nzDiagTable = (int *)calloc(my_NRows,sizeof(int)))!=NULL);
  assert( (nzNonDiagTable = (int *)calloc(my_NRows,sizeof(int)))!=NULL);

  max = 0;
  for(i=0;i<mesh->nvertices;i++){
    for(xyzIndex=0;xyzIndex<3;xyzIndex++) 
      
      if(IsInRange(3*i+xyzIndex, Istart, Iend)){
        j=0; 
        numberOfNZDiag=0; 
        numberOfNZNonDiag=0;
        
        while (connectivity_list[i][j] != NIL) {
          assert(i<mesh->nvertices);
          assert(j<MAX_NEIGHBORS);
          //          fprintf(stderr, "%i %i", i, j);
          for (k=0;k<3;k++)
            if (IsInRange(3*connectivity_list[i][j]+k,Istart,Iend))
              numberOfNZDiag++;
            else
              numberOfNZNonDiag++;
          j++;
        }

        /* Don't forget that on this row, there's also 3 coords 
         * for the node itself ! 
         */
        for (k=0;k<3;k++)
          if (IsInRange(3*i+k,Istart,Iend))
            numberOfNZDiag++;
          else
            numberOfNZNonDiag++;

        j++;

        nzDiagTable[3*i + xyzIndex - Istart] = numberOfNZDiag;
        nzNonDiagTable[3*i + xyzIndex - Istart] = numberOfNZNonDiag;

        if (j > max )
          max = j;

      }
  }

  ierr = MatCreateMPIAIJ(PETSC_COMM_WORLD,
    my_NRows, my_NRows,
    n,n,
    0,nzDiagTable,
    0,nzNonDiagTable,
    &K_global); CHKERRQ(ierr);
  /*ierr = MatSetOption(K_global, MAT_KEEP_ZEROED_ROWS);*/ /*CHKERRQ(ierr);*/

  ierr = VecCreateMPI(PETSC_COMM_WORLD,my_NRows,n,&F_global);CHKERRQ(ierr);
  ierr = VecDuplicate(F_global,&u_displ); CHKERRQ(ierr);
  ierr = VecCreateSeq(PETSC_COMM_SELF,n,&v_scatter); CHKERRQ(ierr);
  ierr = MatZeroEntries(K_global);   CHKERRQ(ierr);
  ierr = VecSet(F_global, zero);     CHKERRQ(ierr);
  ierr = VecSet(u_displ, zero);      CHKERRQ(ierr);
  ierr = VecSet(v_scatter, zero);    CHKERRQ(ierr);   

  ierr = PetscGetTime(&AssemblyStart);CHKERRQ(ierr);

  /* Loop over all tetrahedra */
  /*
  if (myid==0) 
  fprintf(stderr, "Assembling finite element system ... ");
   */

  /* Iterate over all tets in the mesh to assemble the elasticity matrix, 
     and the external forces vector if needed.
   */ 
  for (i=0;i<mesh->ncells;i++){

    // v[] contains the numbers of tet's vertices
    // vec[] contains the 4 vertices coords
    ComputeTetInfo(mesh,i,v,center,vec);

    /*look if processor will have to fill one of the 
     * 12 lines linked to this tet 
     */
    in_range = 0;
      for (j=0;j<4;j++)
        for (k=0;k<3;k++)
          if ( IsInRange(3*v[j]+k,Istart,Iend) ||
               IsInRange(3*v[j]+k,Istart,Iend) )
            in_range++;


      /* every processor computes if the considered indices are inrange.
       * If it belongs either to Istart-Iend or to Istart-Iend,
       * it will need the shape info
       */
      if (in_range>0){
        /* Get shape function of this element */
        ComputeShape(vec,a,b,c,d,&Vel);

        /* integration indices
           only needed if external forces need
           to be interpolated onto the mesh nodes */
        /* CalcIndices(vec,center,Points); 
         * --> only if one needs to integrate over the tet 
         */
        ComputeBD(b,c,d,Vel,B,D,
          E_coef, NU_coef);
          //labels->L[LabelNumber]->E,
          //labels->L[LabelNumber]->nu);
      }


      for (j=0;j<4;j++){
        for (l=0;l<3;l++) 
          ind_i[l]=3*v[j]+l; /* 3* because x,y,z/vertice*/

        /*stuff for rigidity matrix*/ 
        for (k=j;k<4;k++){ 
          for (m=0;m<3;m++) 
            ind_j[m]=3*v[k]+m;

          /* 
           * take all row numbers that processor is going to fill
           * compute Kel and fill orig elem and transposed elem
           */
          if ( (IsInRange(ind_i[0],Istart,Iend)==1) || 
            (IsInRange(ind_i[1],Istart,Iend)==1) ||
            (IsInRange(ind_i[2],Istart,Iend)==1) ||
            (IsInRange(ind_j[0],Istart,Iend)==1) ||
            (IsInRange(ind_j[1],Istart,Iend)==1) ||
            (IsInRange(ind_j[2],Istart,Iend)==1) ){

            calc_Kel(B[j],D,B[k],Vel,Kel);

            if (IsInRange(ind_i[0],Istart,Iend)==1) {
              ierr = MatSetValues(K_global,1,ind_i,3,ind_j,
                Kel, /* 1st line of 3x3 matrix */
                ADD_VALUES); 
              CHKERRQ(ierr);
            }

            if (IsInRange(ind_i[1],Istart,Iend)==1) {
              ierr = MatSetValues(K_global,1,ind_i+1,3,ind_j,
                Kel+3, /* 2nd line of 3x3 matrix */
                ADD_VALUES); 
              CHKERRQ(ierr);
            }

            if (IsInRange(ind_i[2],Istart,Iend)==1) {
              ierr = MatSetValues(K_global,1,ind_i+2,3,ind_j,
                Kel+6, /* 3rd line of 3x3 matrix */
                ADD_VALUES); 
              CHKERRQ(ierr);
            }

            /*
             * fill in symmetric part of local 12x12 matrix
             * for non diagonal elements -> if i!=j
             * Kel(i,j) = transposed ( Kel(j,i) )
             */
            if (k!=j){
              if ((IsInRange(ind_j[0],Istart,Iend)==1) ||
                (IsInRange(ind_j[1],Istart,Iend)==1) ||
                (IsInRange(ind_j[2],Istart,Iend)==1) ){
                  Transpose3x3(Kel);

                  if (IsInRange(ind_j[0],Istart,Iend)==1) {
                    ierr = MatSetValues(K_global,1,ind_j,3,ind_i,
                      Kel,
                      ADD_VALUES); 
                    CHKERRQ(ierr);
                  }

                  if (IsInRange(ind_j[1],Istart,Iend)==1) {
                    ierr = MatSetValues(K_global,1,ind_j+1,3,ind_i,
                      Kel+3,
                      ADD_VALUES); 
                    CHKERRQ(ierr);
                  }

                  if (IsInRange(ind_j[2],Istart,Iend)==1) {
                    ierr = MatSetValues(K_global,1,ind_j+2,3,ind_i,
                      Kel+6,
                      ADD_VALUES); 
                      CHKERRQ(ierr);
                  }
              }   

            } /* k!=j*/

            } /* isinrange || || || || || ||*/

          } /*for k*/ 

        } /* for j */

      }/*End of the loop over all tets i=variable */



    /* --apply boundary conditions only if there are any */
    if (bc->NIndices != 0) {
      /*
      if (myid==0)
        fprintf(stderr,"Adding boundary conditions ...");
      */

      /*we need to pre-assemble the vector to set some of the entries 
        to the boundary condition values */
      ierr = VecAssemblyBegin(F_global); CHKERRQ(ierr);
      ierr = VecAssemblyEnd(F_global); CHKERRQ(ierr);  
      /* also, we need to do a pre-assembly for the MatrixZeroRows */
      ierr = MatAssemblyBegin(K_global,MAT_FINAL_ASSEMBLY); CHKERRQ(ierr);
      ierr = MatAssemblyEnd(K_global,MAT_FINAL_ASSEMBLY); CHKERRQ(ierr);

      /*
      if (myid==0)
        fprintf(stderr,"got the %d bc... applying ... ",bc->NIndices);
      */


      ierr = ISCreateGeneral(PETSC_COMM_WORLD,
        bc->NIndices,
        bc->Indices,
        &ISBoundaryNodes);  
      CHKERRQ(ierr);

      /*Set the corresponding lines of the stiffness matrix to unity */
      ierr = MatZeroRowsIS(K_global,ISBoundaryNodes,one);   CHKERRQ(ierr);

      /* Now put displacements in the vector at the same row locations */
      ierr = VecSetValues(F_global,
        bc->NIndices,
        bc->Indices,
        bc->Displacements,
        INSERT_VALUES);   
      CHKERRQ(ierr);

    }

    /* final assembly */
    ierr = MatAssemblyBegin(K_global,MAT_FINAL_ASSEMBLY); CHKERRQ(ierr);
    ierr = MatAssemblyEnd(K_global,MAT_FINAL_ASSEMBLY); CHKERRQ(ierr);
    ierr = VecAssemblyBegin(F_global); CHKERRQ(ierr);
    ierr = VecAssemblyEnd(F_global); CHKERRQ(ierr);  

    ierr = PetscGetTime(&AssemblyEnd);CHKERRQ(ierr);

    /***********************************************************************/
    /* Global rigidity matrix K and forces vector F have been computed *****/
    /***********************************************************************/

    ierr = PetscGetTime(&SolveStart);CHKERRQ(ierr);

    /*
     *  Create linear solver and solve the system 
     */
    
    KSPCreate(PETSC_COMM_WORLD, &ksp);CHKERRQ(ierr);
    ierr = KSPSetOperators(ksp, K_global, K_global, DIFFERENT_NONZERO_PATTERN);CHKERRQ(ierr); 
    ierr = KSPSetTolerances(ksp,1.e-7,PETSC_DEFAULT,PETSC_DEFAULT, PETSC_DEFAULT);CHKERRQ(ierr);
    ierr = KSPSolve(ksp, F_global, u_displ);CHKERRQ(ierr);
    ierr = KSPGetIterationNumber(ksp, &its);

    ierr = PetscGetTime(&SolveEnd);CHKERRQ(ierr);

    /* get elements from all parts managed by processors */
    /* This is to make sure every processor has the entire deformation
       field in memory. After parallel solving, the processors only 
       have the information about the part of the vector they manage
     */
    ierr = ISCreateStride(PETSC_COMM_SELF,n,0,1,&from);CHKERRQ(ierr);
    ierr = ISCreateStride(PETSC_COMM_SELF,n,0,1,&to);CHKERRQ(ierr);
    ierr = VecScatterCreate(u_displ,from,v_scatter,to,&scatter); 
    CHKERRQ(ierr);
    ierr = VecScatterBegin(u_displ,v_scatter,
      INSERT_VALUES,SCATTER_FORWARD,scatter); 
    CHKERRQ(ierr);
    ierr = VecScatterEnd(u_displ,v_scatter,
      INSERT_VALUES,SCATTER_FORWARD,scatter); 
    CHKERRQ(ierr);  
    VecGetArray(v_scatter,&disp_vect);
    ISDestroy(from);
    ISDestroy(to);
    VecScatterDestroy(scatter);


    /* To visualize the output vector
       ierr=VecView(v_scatter,VIEWER_STDOUT_WORLD);  CHKERRQ(ierr); */

    for (i=0;i<mesh->nvertices;i++){        
      mesh->x[i] += disp_vect[3*i];
      mesh->y[i] += disp_vect[3*i+1];
      mesh->z[i] += disp_vect[3*i+2];
    }

    /* Do the resizing to the image size otherwise, 
     * later on the program will try to read data that's 
     * outside the image !!!
     * 
     * !!! BUT Shouldn't be necessary if we enforce a 
     * !!! zero-displacement at the pictures' boundary
     */
    /* do the clamping anyway
       clamp2bbox_tetra_mesh(mesh);
     */

    //ierr = SLESDestroy(sles);
    ierr = KSPDestroy(ksp);
    ierr = MatDestroy(K_global);
    ierr = VecDestroy(F_global);
    ierr = VecDestroy(u_displ);
    ierr = VecDestroy(v_scatter);

    ierr = PetscGetTime(&EndTime);CHKERRQ(ierr);

    /*
    if (myid==0)
      fprintf(stderr, "\n\tTotal solving time (init + ass + solve + scatter) : %g \n",
        EndTime-StartTime);
    */

    return 1;
}

void PETScDeformWrapper::ComputeShape(float vec[4][3], float a[4], float b[4], float c[4],
  float d[4], float *Vel){ 

  int j,k,l;
  float vec_tmp[4][3];
  extern int myid;

  /* 1. Compute the subdeterminants of the tetrahedron */
  for (j=0;j<4;j++) /* for each vertice of tet i */
    /* a : columns = coordinates of the vertices */
    a[j]=calc_det_3(vec[(j+1)%4],vec[(j+2)%4],vec[(j+3)%4]);
  
  /* b : first column filled with ones*/
  for (l=0;l<4;l++){
    for (k=1;k<3;k++) 
      vec_tmp[l][k]=vec[l][k]; 
    vec_tmp[l][0]=1;
  }
  for (j=0;j<4;j++)
    b[j]=
      -calc_det_3(vec_tmp[(j+1)%4],vec_tmp[(j+2)%4],vec_tmp[(j+3)%4]);
    
  /* c : second column filled with ones*/
  for(l=0;l<4;l++){
    for (k=0;k<3;k++) 
      vec_tmp[l][k]=vec[l][k]; 
    vec_tmp[l][1]=1;
  }
  for (j=0;j<4;j++)
    c[j]=
      -calc_det_3(vec_tmp[(j+1)%4],vec_tmp[(j+2)%4],vec_tmp[(j+3)%4]);

  /* d: third column filled with ones*/
  for(l=0;l<4;l++){
    for (k=0;k<2;k++) 
      vec_tmp[l][k]=vec[l][k]; 
    vec_tmp[l][2]=1;
  }
  for (j=0;j<4;j++)
    d[j]=
      -calc_det_3(vec_tmp[(j+1)%4],vec_tmp[(j+2)%4],vec_tmp[(j+3)%4]);

  /* 2. Compute tet's volume. Also = jacobian! */
  *Vel=(a[0]-a[1]+a[2]-a[3])/6;    
  if (*Vel<0) 
    *Vel=-*Vel;
  
  for (j=0;j<4;j++){
    if ((j==0)||(j==2)){
      a[j]*=-1;
      b[j]*=-1;
      c[j]*=-1;
      d[j]*=-1;
    }
  }   
}



void PETScDeformWrapper::ComputeTetInfo(struct PETScDeformWrapper::tetra_mesh *mesh_in, int CellNumber,
    int v[4], float center[3], float vec[4][3]){
  int j, k;
  
  /*extract vertices' numbers of tet i*/
  for (j=0;j<4;j++)  
    v[j] = mesh_in->cell[CellNumber]->vert[j];
  
  /*put coordinates in a 4x3 matrix */
  center[0]=center[1]=center[2]=0;
  for(j=0;j<4;j++){
    center[0] += vec[j][0] = mesh_in->x[v[j]];  
    center[1] += vec[j][1] = mesh_in->y[v[j]];  
    center[2] += vec[j][2] = mesh_in->z[v[j]];   
    /*
    if ( (vec[j][0]<0)||(vec[j][0]>=SIZE_X ))
      fprintf(stderr,"Shit in x : %f \n",mesh_in->x[v[j]]);
 
   if ( (vec[j][1]<0)||(vec[j][1]>=SIZE_Y ))
      fprintf(stderr,"Shit in y : %f \n",mesh_in->y[v[j]]);

    if ( (vec[j][2]<0)||(vec[j][2]>=SIZE_Z ))
      fprintf(stderr,"Shit in z : %f \n",mesh_in->z[v[j]]);
      */
  };

  
  /* WHY is this commented out
  for (k=0;k<3;k++) center[k]/=4.0;

  //new reference system : w.r.t center of tet i 
  for (j=0;j<4;j++)
    for (k=0;k<3;k++)
      vec[j][k]-=center[k]; 
  */
}

float PETScDeformWrapper::calc_det_3(float a[3],float b[3],float c[3]){
  float result;
  result=a[0]*(b[1]*c[2]-c[1]*b[2]) - a[1]*(b[0]*c[2]-c[0]*b[2]) 
       + a[2]*(b[0]*c[1]-c[0]*b[1]);
  return result;
}

int PETScDeformWrapper::IsInRange(int ind,int Istart,int Iend){
  if ( (ind>=Istart)&&(ind<Iend))
    return 1;
  else
    return 0;
}

/***************************************************************************/
/* Get the connectivity list of every vertex in the mesh */

int** PETScDeformWrapper::get_vertex_connectivity_list_tetra_mesh(struct PETScDeformWrapper::tetra_mesh *mesh){
  int i,j,k;  
  int **connectivity_list,*connectivity_counter;
  char *func_name = "get_vertex_connectivity_list_tetra_mesh";
  struct tetra_mesh *tmpMesh;

  
  /* Allocate memory space for holding connectivitylist and counter*/
  assert( (connectivity_list = 
    (int **)calloc(mesh->nvertices,sizeof(int*)))!=NULL);
  assert( (connectivity_counter = 
    (int *)calloc(mesh->nvertices,sizeof(int)))!=NULL );
  
  for (i=0;i<mesh->nvertices;i++) {
    assert( (connectivity_list[i] = 
      (int *)calloc(MAX_NEIGHBORS,sizeof(int)))!=NULL);
    /*initialize to -1 */
    for (j=0;j<MAX_NEIGHBORS;j++)
      connectivity_list[i][j] = NIL;
  };
  

  if (!mesh->edges_present) {
    mesh = add_edge_info_tetra_mesh(mesh);
  };  
  assert(mesh->edges_present);

  for (i=0;i<mesh->nedges;i++) {
    assert(mesh->edge[2*i]<mesh->nvertices);
    assert(mesh->edge[2*i+1]<mesh->nvertices);
    assert(connectivity_counter[mesh->edge[2*i]]<MAX_NEIGHBORS);
    assert(connectivity_counter[mesh->edge[2*i+1]]<MAX_NEIGHBORS);

    /* first vertice gets the second as neighbor */
    connectivity_list[ mesh->edge[2*i] ][ connectivity_counter[mesh->edge[2*i]] ] = 
      mesh->edge[2*i+1]; /*this is the neighbor */
    
    assert(mesh->edge[2*i]<mesh->nvertices);
    connectivity_counter[mesh->edge[2*i]]++;
//    assert(connectivity_counter[mesh->edge[2*i]]<MAX_NEIGHBORS);
    
    /* second vertice gets the first as neighbor */
    connectivity_list[ mesh->edge[2*i+1] ][ connectivity_counter[mesh->edge[2*i+1]] ] = 
      mesh->edge[2*i]; /*this is the neighbor */
    
    assert(mesh->edge[2*i+1]<mesh->nvertices);
    connectivity_counter[mesh->edge[2*i+1]]++;
//    assert(connectivity_counter[mesh->edge[2*i+1]]<MAX_NEIGHBORS);
  }

  int max_conn = 0;
  for(i=0;i<mesh->nvertices;i++){
    assert(connectivity_counter[i]<MAX_NEIGHBORS);
    if(connectivity_counter[i]>MAX_NEIGHBORS || 
       connectivity_counter[i]<=0){
      fprintf(stderr, "Vertex connectivity is incorrect...\n");
      abort();
    }
    if(connectivity_counter[i]>max_conn)
      max_conn = connectivity_counter[i];
  }
  
  return connectivity_list;
}

/* renumber all tetrahedra by taking one tet every interval tets */
struct PETScDeformWrapper::cell ** PETScDeformWrapper::reshuffle_cell_numbers_tetra_mesh(struct PETScDeformWrapper::tetra_mesh *mesh, int interval){
  int i,j,cnt;

  struct cell **newCells;
  
  char *func_name = "reshuffle_cell_numbers_tetra_mesh";

  /* allocate table of pointers to new cells */
  assert( (newCells = (struct cell **)calloc(mesh->ncells, sizeof(struct cell *)))!=NULL);
  if (interval > 1) {
    for (i=0, cnt=0 ; i<mesh->ncells ; i += interval)     
      for (j=0 ; j<interval,cnt<mesh->ncells ; j++,cnt++)
  newCells[cnt] = mesh->cell[ interval*i + j ];
  /* free old table of pointers to cells */  
    free(mesh->cell);
    return(newCells);
  } else {
    fprintf(stderr, "Doing nothing : reshuffle interval should be >1");
    return(mesh->cell);
  }
}

struct PETScDeformWrapper::tetra_mesh *PETScDeformWrapper::add_edge_info_tetra_mesh(struct PETScDeformWrapper::tetra_mesh *mesh){
  struct tetra_mesh *mesh_out;
  
  int i,j,k;
  int v[4],edge_count,vert_count,c1,c2,e;
  int stringlen = sizeof(int);
  char *pointer = (char *)malloc(stringlen); 
  
  int big,is_new;
  int key[2];

  int edgeKey,intKey[2];
  
  /*
  Tcl_HashTable EdgeTable; // the keys will be a combination of vert numbers 
  Tcl_HashEntry *entryPtr;
  */
  std::map<std::pair<int,int>,int> EdgeTable;
  std::map<std::pair<int,int>,int>::iterator entryPtr;
  
  char *func_name = "add_edge_info_tetra_mesh";
  
  /*************************/

  //Tcl_InitHashTable(&EdgeTable,2); /* two entries for each edge */

  mesh_out = copy_tetra_mesh(mesh);
  
  big = mesh->ncells*6*2;

  /* Allocate enough memory to store the edges
     estimate : ncells*4/6 -> ~ ncells AND ! two entries per edge*/
  assert( (mesh_out->edge = (int *)calloc(big, sizeof(int)) )!=NULL );
  
  mesh_out->nedges = 0;

  /* NUMBERING of edges from tet's 4 vertices :
     edge 0 = verts 0 - 1 ; edge 1 = verts 0 - 2 ; edge 2 = verts 0 - 3
     edge 3 = verts 1 - 2 ; edge 4 = verts 1 - 3 ; edge 5 = verts 2 - 3
  */

  /*
  std::vector<std::set<unsigned> > neighbors_sets;
  neighbors_sets.resize(mesh->nvertices);
  for(i=0;i<mesh->ncells;i++){
    for(j=0;j<4;j++)
      for(k=0;k<4;k++)
        if(tetras[i*4+j]!=tetras[i*4+k]){
          neighbors_sets[j].insert(tetras[i*4+k]);
          neighbors_sets[k].insert(tetras[i*4+j]);
        }
  }
  for(i=0;i<mesh->nvertices;i++){
    if(neighbors_sets[i].size()>max_vertex_degree)
      max_vertex_degree = neighbors_sets[i].size();
  }
  fprintf(stderr, "VERTEX DEGREE = %i\n", max_vertex_degree);

  fprintf(stderr, "Vertex 0: %f %f %f\n", 
    mesh->vertices[0], mesh->vertices[1], mesh->vertices[2]);
  */
  
  for (i=0;i<mesh->ncells;i++) {

    /*debug_loop_message(func_name,"cell %d ",i);
     */

    /* get the 4 vert numbers in simplified form */
    for(j=0;j<4;j++)
      v[j] = mesh->cell[i]->vert[j];

    /*for every pair of different vertices of the tet */    
    /*edge_count is the count of vertices inside this tet */
    edge_count=0;
    for(j=0;j<4;j++)
      for(k=j+1;k<4;k++){
        key[0] = mesh->cell[i]->vert[j]; /* could be v[j] ?*/
        key[1] = mesh->cell[i]->vert[k]; /* could be v[k] ?*/

        Order2((int *)key,(int *)key+1);
        
        if (key[0] >= key[1])
          fprintf(stderr,"Error ordering 2 vertices of tet : %d and %d \n",key[0],key[1]);

        //entryPtr = Tcl_CreateHashEntry(&EdgeTable,(char *)key,&is_new);
        std::pair<int,int> key_pair(key[0],key[1]);
        entryPtr = EdgeTable.find(key_pair);

        if (entryPtr!=EdgeTable.end()){/*entry already exists */
          assert(edge_count<6);
          mesh_out->cell[i]->edge[edge_count] = EdgeTable[key_pair];

          /*report_message(func_name,"Added again edge no %d = %d %d",
            mesh_out->cell[i]->edge[edge_count],v[j],v[k]); */
          edge_count++; /*one more edge has been inserted into tet struct */

        } else { /* entry does not exist yet -> ADD NEW EDGE*/
          EdgeTable[key_pair] = mesh_out->nedges;
          assert(edge_count<6);
          mesh_out->cell[i]->edge[edge_count] = mesh_out->nedges;
          /*report_message(func_name," ...nedge = %d",mesh_out->cell[i]->edge[edge_count]);
           */
          edge_count++;
          /* put edges with ordered vertices ... shouldn't change anything*/
          mesh_out->edge[2*mesh_out->nedges] = key[0];
          mesh_out->edge[2*mesh_out->nedges+1] = key[1];
          /*
          mesh_out->edge[2*mesh_out->nedges] =  mesh->cell[i]->vert[j];
          mesh_out->edge[2*mesh_out->nedges+1] =  mesh->cell[i]->vert[k];
           */
          mesh_out->nedges++;

          if (mesh_out->nedges > big) {
            fprintf(stderr,
              "ohhohhhh big trouble in estimating amount of edges \n");
          }
        }

      }/*done for j and k pairs */

    assert(edge_count == 6);

    
    /*    report_warning(func_name,"tet %d :Verts %d %d %d %d ",i,mesh_out->cell[i]->vert[0],mesh_out->cell[i]->vert[1],mesh_out->cell[i]->vert[2],mesh_out->cell[i]->vert[3]);
    report_warning(func_name,"tet %d :Edges %d %d %d %d %d %d ",i,mesh_out->cell[i]->edge[0],mesh_out->cell[i]->edge[1],mesh_out->cell[i]->edge[2],mesh_out->cell[i]->edge[3],mesh_out->cell[i]->edge[4],mesh_out->cell[i]->edge[5]);
    */
  }
    

  /* find the edges of the faces based on the edge list 
     created above
   */
  for (i=0;i<mesh_out->nfaces;i++) {

    /* for the 3 edges in the face, find the entry in the
       tabel we've just created
     */
    for (j=0;j<3;j++) {
      /* get a pair of vertices forming edge j */ 
      key[0] = mesh->face[i]->vert[j%3];
      key[1] = mesh->face[i]->vert[(j+1)%3];

      Order2((int *)key,(int *)key+1);
      assert( key[0] < key[1] );
      
     
      /* verification */
      std::pair<int,int> key_pair(key[0],key[1]);
      //entryPtr = Tcl_CreateHashEntry(&EdgeTable,(char *)key,&is_new);
      entryPtr = EdgeTable.find(key_pair);
      assert(entryPtr!=EdgeTable.end());
      
      /* get the number of this edge 
         and put it into face structure*/
      mesh_out->face[i]->edge[j] = (*entryPtr).second;
    }
  }

  
  /* free unneeded memory */
  assert( (mesh_out->edge = (int *)realloc(mesh_out->edge,2*mesh_out->nedges*sizeof(int)) )!=NULL );
  mesh_out->edges_present = 1;
  int* vertex_connectivity = new int[mesh_out->nvertices];
  int max_vertex_degree = 0;
  double min_edge_length = 1000, edge_length;
  bzero(vertex_connectivity, sizeof(int)*(mesh_out->nvertices));
  for(i=0;i<mesh_out->nedges;i++){
    double v0[3], v1[3];
    assert(mesh_out->edge[i*2]<mesh_out->nvertices);
    assert(mesh_out->edge[i*2+1]<mesh_out->nvertices);

    v0[0] = mesh_out->vertices[0][mesh_out->edge[i*2]];
    v1[0] = mesh_out->vertices[0][mesh_out->edge[i*2+1]];
    v0[1] = mesh_out->vertices[1][mesh_out->edge[i*2]];
    v1[1] = mesh_out->vertices[1][mesh_out->edge[i*2+1]];
    v0[2] = mesh_out->vertices[2][mesh_out->edge[i*2]];
    v1[2] = mesh_out->vertices[2][mesh_out->edge[i*2+1]];
    
    edge_length = sqrt((v0[0]-v1[0])*(v0[0]-v1[0]) + (v0[1]-v1[1])*(v0[1]-v1[1]) +
      (v0[2]-v1[2])*(v0[2]-v1[2]));
    if(edge_length<min_edge_length)
      min_edge_length = edge_length;
    
    vertex_connectivity[mesh_out->edge[i*2]]++;
    vertex_connectivity[mesh_out->edge[i*2+1]]++;
    if(vertex_connectivity[mesh_out->edge[i*2]]>max_vertex_degree)
      max_vertex_degree = vertex_connectivity[mesh_out->edge[i*2]];
    if(vertex_connectivity[mesh_out->edge[i*2+1]]>max_vertex_degree)
      max_vertex_degree = vertex_connectivity[mesh_out->edge[i*2+1]];
  }
  /*
  fprintf(stderr, "Minimum edge length: %f\n", min_edge_length);
  fprintf(stderr, "Maximum vertex degree: %i\n", max_vertex_degree);
  if(!myid)
    fprintf(stderr,"Amount of edges in this structure : %d; max vertex degree: %i\n",
     mesh_out->nedges, max_vertex_degree);
  */
  delete [] vertex_connectivity;
  return mesh_out;
}

struct PETScDeformWrapper::tetra_mesh* PETScDeformWrapper::create_tetra_mesh( int ncells, int nfaces,
  int nboundfaces, int nvertices, int nedges){  
//  extern int myid;
  struct tetra_mesh *mesh;
  char func_name[] = "create_tetra_mesh";
  
  int i,j;

  assert((mesh=(struct tetra_mesh *)malloc(sizeof(struct tetra_mesh)))!=NULL);
  
  mesh->ncells = ncells;
  mesh->nfaces = nfaces;
  mesh->nboundfaces = nboundfaces;
  mesh->nvertices = nvertices;
  mesh->nedges = nedges;

  /*  report_message(func_name,"glub 1");   */
  if (nfaces > 0)
    assert( (mesh->face = (struct face **)calloc(nfaces,sizeof(struct face *)))!=NULL);
  if (ncells > 0)
    assert( (mesh->cell = (struct cell **)calloc(ncells,sizeof(struct cell *)))!=NULL);
  if (nboundfaces > 0) {
    assert( (mesh->face_bc = (int *)calloc(2*nboundfaces,sizeof(int)))!=NULL);
  }
  if (nedges>0) 
    assert((mesh->edge = (int *)calloc(2*nedges,sizeof(int)))!=NULL);
  
  /*  report_message(func_name,"glub 2");   */
  
  /* vertices' coordinates are accessible through a double scheme */
  assert(( mesh->vertices = (float**)calloc(3,sizeof(float *)))!=NULL);
  if (nvertices > 0) {
    for (i=0;i<3;i++)
      assert(( mesh->vertices[i] = (float *)calloc(nvertices,sizeof(float)))!=NULL);
    mesh->x = mesh->vertices[0];
    mesh->y = mesh->vertices[1];
    mesh->z = mesh->vertices[2];
  }

  
  /*  report_message(func_name,"glub 3");   */

/* initializations */
  for (i=0;i<nvertices;i++)
    for (j=0;j<3;j++)
      mesh->vertices[j][i]=NIL;

  /*  report_message(func_name,"glub 3 bis");   */
 
  for (i=0;i<nfaces;i++)    {

    assert((mesh->face[i]=(struct face *)malloc(sizeof(struct face))) !=NULL);
    for (j=0;j<2;j++)
      mesh->face[i]->cell[j]=NIL;
    for (j=0;j<3;j++) {
      mesh->face[i]->vert[j]=NIL;
      mesh->face[i]->edge[j]=NIL;
    }
  }

  /*  report_message(func_name,"glub 3 pre-4");   */
  
  for (i=0;i<nboundfaces;i++) {
    mesh->face_bc[2*i]=NIL;
    mesh->face_bc[2*i+1]=NIL;
  }
  

  for (i=0;i<ncells;i++)    {
    if (((mesh->cell[i]=(struct cell *)malloc(sizeof(struct cell)))==NULL)&& 
      (myid==0)) {  
      fprintf(stderr,"Could not allocate space for cell %d\n",i);
      abort();
    }
    
    for (j=0;j<4;j++){
      mesh->cell[i]->face[j]=NIL;
      mesh->cell[i]->vert[j]=NIL;
    }
    
    mesh->cell[i]->label=NIL;
    
  }
  
  mesh->edges_present = 0;
  
  for (i=0;i<3;i++)
    mesh->vox_size[i] = 1;
  
  return mesh;
  
}/* end of create_tetra_mesh */

/********************************************************************/
/*copy a tetmesh struct into a new allocated memory space and return it */

struct PETScDeformWrapper::tetra_mesh* PETScDeformWrapper::copy_tetra_mesh(struct PETScDeformWrapper::tetra_mesh *mesh){
  extern int myid;
  struct tetra_mesh *out_mesh;
  int i,j;
   
  /* Creation and allocation of a new mesh having same size as mesh */
  out_mesh = create_tetra_mesh(mesh->ncells, mesh->nfaces, mesh->nboundfaces, mesh->nvertices, mesh->nedges);

  if (mesh->edges_present)
    out_mesh->edges_present = 1; 
  
  for (i=0;i<mesh->nvertices;i++)
    for (j=0;j<3;j++)
      out_mesh->vertices[j][i] = mesh->vertices[j][i];
  
  for (i=0;i<mesh->nfaces;i++)
    {
      for (j=0;j<2;j++)
        out_mesh->face[i]->cell[j] = mesh->face[i]->cell[j];
      
      for (j=0;j<3;j++)
        out_mesh->face[i]->vert[j] = mesh->face[i]->vert[j];
      
      if (mesh->edges_present==1)
      for (j=0;j<3;j++)
        out_mesh->face[i]->edge[j] = mesh->face[i]->edge[j];
    }
 
  for (i=0;i<mesh->nboundfaces;i++) {
    out_mesh->face_bc[2*i] = mesh->face_bc[2*i];
    out_mesh->face_bc[2*i+1] = mesh->face_bc[2*i+1];
  }
  
  for (i=0;i<mesh->ncells;i++) {
    for (j=0;j<4;j++)
      {
      out_mesh->cell[i]->face[j] = mesh->cell[i]->face[j];
      out_mesh->cell[i]->vert[j] = mesh->cell[i]->vert[j];
      }
    
    if (out_mesh->edges_present==1)
      for (j=0;j<6;j++)
        out_mesh->cell[i]->edge[j] = mesh->cell[i]->edge[j];
    
    out_mesh->cell[i]->label = mesh->cell[i]->label;
  }
 
 for (i=0;i<3;i++) {
   out_mesh->vox_size[i] = mesh->vox_size[i];
   for (j=0;j<2;j++)
     out_mesh->bb[j][i] = mesh->bb[j][i];
 }
 
 if (mesh->edges_present==1) {
   
   assert( (out_mesh->edge = (int *)calloc(2*mesh->nedges, sizeof(int))) != NULL);
 
   for (i=0;i<2*mesh->nedges;i++){
     out_mesh->edge[i] = mesh->edge[i];
   }
   
   out_mesh->nedges = mesh->nedges;
   
 }
 return out_mesh;
}

void PETScDeformWrapper::Order2(int *v1,int *v2){
  if (*v1>*v2){
    int tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
  }
}

double PETScDeformWrapper::get_min_edge_len(struct PETScDeformWrapper::tetra_mesh* mesh){
  double min_edge_length = 1000;
  for(int i=0;i<mesh->nedges;i++){
    double v0[3], v1[3], edge_length;
    assert(mesh->edge[i*2]<mesh->nvertices);
    assert(mesh->edge[i*2+1]<mesh->nvertices);

    v0[0] = mesh->vertices[0][mesh->edge[i*2]];
    v1[0] = mesh->vertices[0][mesh->edge[i*2+1]];
    v0[1] = mesh->vertices[1][mesh->edge[i*2]];
    v1[1] = mesh->vertices[1][mesh->edge[i*2+1]];
    v0[2] = mesh->vertices[2][mesh->edge[i*2]];
    v1[2] = mesh->vertices[2][mesh->edge[i*2+1]];
    
    edge_length = sqrt((v0[0]-v1[0])*(v0[0]-v1[0]) + (v0[1]-v1[1])*(v0[1]-v1[1]) +
      (v0[2]-v1[2])*(v0[2]-v1[2]));
    if(edge_length<min_edge_length)
      min_edge_length = edge_length;
  }
  return min_edge_length;
}
}
