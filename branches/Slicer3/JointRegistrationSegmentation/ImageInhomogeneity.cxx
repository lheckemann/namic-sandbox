// Data Definition

 int InvertMatrix(double **mat, double **inv_mat,int dim) {
  double det;
  if (dim < 2) {
    if (mat[0][0] == 0) return 0;
    inv_mat[0][0] = 1.0 / mat[0][0];
  } else {
    if (dim < 3) {
      det = vtkImageEMGeneral::determinant(mat,2);
      if (fabs(det) <  1e-15 ) return 0;
      det = 1.0 / det;
      inv_mat[0][0] = det * mat[1][1];
      inv_mat[1][1] = det * mat[0][0];
      inv_mat[0][1] = -det * mat[0][1];
      inv_mat[1][0] = -det * mat[1][0];
    } else {
      return vtkMath::InvertMatrix(mat,inv_mat,dim);
    }
  }
  return 1;
}

// Kilian turn around dimension so it is y,x,z like in matlab ! 
class VTK_EMATLASBRAINCLASSIFIER_EXPORT EMVolume {
public:
  //static EMVolume *New() {return (new vtkDataDef);}
  EMVolume(){this->Data  = NULL;this->MaxX = this->MaxY = this->MaxZ = this->MaxXY = this->MaxXYZ = 0;}
  EMVolume(int initZ,int initY, int initX) {this->allocate(initZ,initY, initX);}
  ~EMVolume() {this->deallocate();}

  // Convolution in all three direction 
  // Can be made using less memory but then it will be probably be slower
  void Conv(float *v,int vLen) {
    this->ConvY(v,vLen);
    this->ConvX(v,vLen);
    this->ConvZ(v,vLen);
  }
  void Resize(int DimZ,int DimY,int DimX) {
    if ((this->MaxX == DimX) && (this->MaxY == DimY) && (this->MaxZ == DimZ)) return;
    this->deallocate();this->allocate(DimZ,DimY,DimX);
  }

  float& operator () (int z,int y, int x) {return this->Data[x+this->MaxX*y + this->MaxXY*z];}
  const float& operator () (int z,int y, int x) const {return this->Data[x+this->MaxX*y + this->MaxXY*z];}

  EMVolume & operator = (const EMVolume &trg) {
    if ( this->Data == trg.Data ) return *this;
    if ((this->MaxX != trg.MaxX) || (this->MaxY != trg.MaxY) ||(this->MaxZ != trg.MaxZ)) {
    }
    for (int i = 0; i < this->MaxXYZ; i++ ) this->Data[i] = trg.Data[i];
    return *this;
  }

  void ConvY(float *v, int vLen);
  void ConvX(float *v, int vLen);
  void ConvZ(float *v, int vLen);

  void Print(char name[]);
  void Test(int Vdim);
  void SetValue(float val) {
    if (val) {for (int i = 0; i < this->MaxXYZ; i++ ) this->Data[i] = val;}
    else { memset(this->Data, 0,sizeof(float)*this->MaxXYZ);}
  }
  float* GetData() {return this->Data;}
 
protected :
  float *Data;
  int MaxX, MaxY, MaxZ, MaxXY, MaxXYZ;

  void allocate (int initZ,int initY, int initX) {
    this->MaxX  = initX;this->MaxY  = initY;this->MaxZ  = initZ;
    this->MaxXY = initX*initY; this->MaxXYZ = this->MaxXY*this->MaxZ;
    this->Data = new float[this->MaxXYZ];
  } 

  void deallocate () {
    if (this->Data) delete[] this->Data;
    this->Data  = NULL;this->MaxX = this->MaxY = this->MaxZ  =  this->MaxXY = this->MaxXYZ = 0;
  }


}; 

// ----------------------------------------------------------------------------------------------
// Definitions for 5D float array EMTriVolume (lower triangle)
// ----------------------------------------------------------------------------------------------/ 
// It is a 5 dimensional Volume where m[t1][t2][z][y][x] t1>= t2 is only defined 
// Lower Traingular matrix - or a symmetric matrix where you only save the lower triangle
class VTK_EMATLASBRAINCLASSIFIER_EXPORT EMTriVolume {
protected :
  EMVolume **TriVolume;
  int Dim;

  void allocate (int initDim, int initZ,int initY, int initX) {
    this->Dim = initDim;
    int x,y;
    this->TriVolume = new EMVolume*[Dim];
    for (y=0; y < initDim; y++) {
      this->TriVolume[y] = new EMVolume[y+1];
      for (x = 0; x <= y ; x++) this->TriVolume[y][x].Resize(initZ,initY,initX);
    }
  }
  void deallocate () {
    if (this->TriVolume) {
      for (int y = 0; y < this->Dim; y++) delete[] this->TriVolume[y];
      delete[] this->TriVolume;
    }
    this->TriVolume  = NULL;this->Dim  = 0;
  }
public:
  EMTriVolume(){this->TriVolume  = NULL;this->Dim  = 0;}
  EMTriVolume(int initDim,int initZ,int initY, int initX) {this->allocate(initDim,initZ,initY, initX);}
  ~EMTriVolume() {this->deallocate();}

  void Resize(int initDim, int DimZ,int DimY,int DimX) {
    this->deallocate();this->allocate(initDim,DimZ,DimY,DimX);
  }

  float& operator () (int t1, int t2, int z,int y, int x) {return this->TriVolume[t1][t2](z,y,x);}
  const float& operator () (int t1, int t2, int z,int y, int x) const {return this->TriVolume[t1][t2](z,y,x);}

  void Conv(float *v,int vLen) {
    int x,y;
    for (y=0 ; y < this->Dim; y++) { 
      for (x = 0; x <= y; x++) this->TriVolume[y][x].Conv(v,vLen);
    }
  }
  void SetValue(float val) {
    int x,y;
    for (y=0 ; y < this->Dim; y++) { 
      for (x = 0; x <= y; x++) this->TriVolume[y][x].SetValue(val);
    }
  }
};

// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimensio
inline void convVector(float vec[], float u[], int uLen, float v[], int vLen){
  int stump = vLen /2;
  int k,j,jMin,jMax;
  int kMax = uLen + stump;
  float *uSta =u, *vSta=v;  

  for (k = stump; k <  kMax; k++) {
    *vec = 0;
    jMin = (0 > (k - vLen +1) ? 0 : (k - vLen+1) ); // max(0,k-vLen+1):
    jMax = ((k+1) < uLen ? (k+1) : uLen) ;               // min(k,uLen)+1 
    u = uSta + jMin; v = vSta + k-jMin;
    for (j=jMin; j < jMax; j++) 
      *vec += (*u++) * (*v--);
    vec ++;
  }  
}


// ---------------------------------------------------------
// EMVolume Definiton 
// ---------------------------------------------------------
void EMVolume::Print(char name[]) {
  int y,x,z;
  for (z = 0; z < this->MaxZ; z++) {
    cout << name  << "[" << z+1 << "] = [" << endl; 
    for (y = 0; y < this->MaxY; y++) {
      for (x = 0; x < this->MaxX; x++) cout << (*this)(z,y,x) << " ";
      if (y < this->MaxY-1) cout << endl;
    }
    cout << "]; " << endl;
  }
}

// Kilian : This is just so we are compatible with older version
void EMVolume::Conv(double *v,int vLen) {
  float *v_f = new float[vLen];
  for (int i = 0; i < vLen; i++) v_f[i] = float(v[i]);
  this->Conv(v_f,vLen);  
  delete[] v_f;
}



void EMVolume::ConvY(float *v, int vLen) {
  int x,y,z;

  // => Utrans[i] represents the i column of U;    
  float * col     = new float[this->MaxY],
        * result  = new float[this->MaxY];
  float * DataStart = this->Data;

  for (z = 0; z < this->MaxZ; z++) {
    for (x = 0; x < this->MaxX; x++) {
      for (y = 0; y < this->MaxY; y++) {
    col[y] = *this->Data;
    this->Data +=this->MaxX; 
      }
      this->Data -= this->MaxXY;
      convVector(result,col,this->MaxY,v,vLen); // Use the i-th Rows of Utrans; 
      for (y=0; y < this->MaxY; y++) {
    *this->Data = result[y]; // Write result to this->mat as i-th column
    this->Data +=this->MaxX; 
      }
      this->Data -= this->MaxXY -1;
    }
    this->Data += this->MaxXY-this->MaxX;
  }
  this->Data = DataStart; 
  delete[] result;
  delete[] col;
} 

// Same just v is a row vector instead of column one
// We use the following equation :
// conv(U,v) = conv(U',v')' => conv(U,v') = conv(U',v)';
void EMVolume::ConvX(float *v, int vLen) {
  int x,i,MaxYZ;

  // Use the i-th Rows of U = ith Column of U';
  // write it to the i-th Row of 'this' => Transpose again
  float  * row     = new float[this->MaxX],
         * result  = new float[this->MaxX];
  float  * DataStart = this->Data;

  MaxYZ = this->MaxY*this->MaxZ;
  for (i = 0; i < MaxYZ; i++) {
    for (x = 0; x < this->MaxX; x++) row[x] = *this->Data++;
    this->Data -= this->MaxX;
    convVector(result,row,this->MaxX,v,vLen); 
    for (x=0; x < this->MaxX; x++) *this->Data++ = result[x];
  }
  this->Data = DataStart;
  delete[] result;
  delete[] row;
} 

// Same just v is a row vector instead of column one
// We use the following equation :
// conv(U,v) = conv(U',v')' => conv(U,v') = conv(U',v)';
inline void EMVolume::ConvX(EMVolume &src,float *v, int vLen) {
  int x,i,MaxYZ;

  // Use the i-th Rows of U = ith Column of U';
  // write it to the i-th Row of 'this' => Transpose again
  float  * row       = new float[this->MaxX],
         * result    = new float[this->MaxX],
         * DataStart = this->Data,
         * SrcStart  = src.Data;

  MaxYZ = this->MaxY*this->MaxZ;
  for (i = 0; i < MaxYZ; i++) {
    for (x = 0; x < this->MaxX; x++) row[x] = *src.Data ++;
    convVector(result,row,this->MaxX,v,vLen); 
    for (x=0; x < this->MaxX; x++) *this->Data ++ = result[x];
  }
  delete[] result;
  delete[] row;
  this->Data = DataStart;
  src.Data = SrcStart;
} 

// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimension
inline void EMVolume::ConvZ(EMVolume &src,float *v,int vLen) {
  int stump = vLen /2;
  int i,k,j,jMin,jMax;

  int kMax = this->MaxZ + stump;

  float *SrcDataStart = src.Data;
  float *DataStart    = this->Data;
  float *vSta = v;

  for (k = stump; k <  kMax; k++) {
    for (i = 0; i < this->MaxXY; i++) {
      *this->Data++ = 0;
    }
    jMin = (0 > (k+1 - vLen) ? 0 : (k+1  - vLen));     //  max(0,k+1-vLen):
    jMax = ((k+1) < this->MaxZ ? (k+1) : this->MaxZ);     //  min(k+1,mat3DZlen) 

    // this->mat3D[k-stump] += U[j]*v[k-j];
    src.Data = SrcDataStart + this->MaxXY*jMin;  
    v = vSta + k-jMin; 
    for (j=jMin; j < jMax; j++) {
      this->Data -= this->MaxXY;
      for (i = 0; i < this->MaxXY; i++) {
    *this->Data += (*src.Data) * (*v);
    this->Data++;src.Data ++;
      }
      v--;
    }
  } 
  this->Data = DataStart;
  src.Data = SrcDataStart;  
}

// No unecessary memrory -> faster
void EMVolume::ConvZ(float *v, int vLen) {
  int z,i;

  // Use the i-th Rows of U = ith Column of U';
  // write it to the i-th Row of 'this' => Transpose again
  float  * vec     = new float[this->MaxZ],
         * result  = new float[this->MaxZ];
  float  * DataStart = this->Data;

  for (i = 0; i < this->MaxXY; i++) {
    for (z = 0; z < this->MaxZ; z++) {
      vec[z] = *this->Data;
      this->Data += this->MaxXY;
    }
    this->Data -= this->MaxXYZ;
    convVector(result,vec,this->MaxZ,v,vLen); 

    for (z=0; z < this->MaxZ; z++) {
      *this->Data = result[z];
      this->Data += this->MaxXY;
    }
    this->Data -= (this->MaxXYZ-1);
  }
  this->Data = DataStart;

  delete[] result;
  delete[] vec;
} 



//------------------------------------------------------------
// Compute Image Inhomogeneity based on Wells 96 publication
// InputVector             = Log intensities of input images
// w_mPtr                  = weights / posterior probabilities resulting from the E-Step of the EM algorithm  
// InverseLogCov           = Inverse of the log covariance of each structure
// LogMu                   = Mean Log Intensities of structures of interest
// NumberOfStructures      = Number of Structures of interest
// skern                   = filter defining gaussian smoothing
// Dimension*              = dimension of image
// 
// Result of the Function are captured in:
// iv_m                    = weighted inverse variance = w_m * ivar';
// r_m                     = weighted residual  


void EstimateImageInhomegeneity(float** InputVector, float **w_mPtr, double*** InverseLogCov, LogMu, int NumberOfStructures,  float *skern,  
                int SmoothingWidth, int DimensionZ, int DimensionY, int DimensionX, EMTriVolume& iv_m, EMVolume *r_m) {

  float temp;
  float **w_m        = new float*[NumberOfStructures];
  for (int i=0; i< NumberOfStructures; i++) w_m[i] = w_mPtr[i];

  for (int i = 0; i< DimensionZ;i++){
    for (int k = 0; k<DimensionY;k++){
      for (int j = 0; j<DimensionX;j++){
      // If coordinate is not in Region of interest just do not update bias 
        // Just use the outcome of the last hierarchical level 
    for (int m=0; m< NumInputImages; m++) {
          r_m[m](i,k,j) = 0.0;
          for (int n=0; n<=m; n++) iv_m(m,n,i,k,j) = 0.0;
    } 
        
    for (int l=0; l< NumberOfStructures; l++) {
      for (int m=0; m< NumInputImages; m++) {  
        for (int n=0; n< NumInputImages; n++) {
          temp =  *w_m[l] * float(InverseLogCov[l][m][n]);
          r_m[m](i,k,j)     += temp * ((*InputVector)[n] - float(LogMu[l][n]));
          if (n <= m) iv_m(m,n,i,k,j) += temp;
        }
      }
      
      w_m[l]++;
    }
      InputVector ++;
      }
    } 
  } 
  
  //------------------------------------------------------------
  // Finalize Image Inhomogeneity Parameters  
  // smooth residuals and inv covariances - 3D
  //------------------------------------------------------------
  iv_m.Conv(skern,SmoothingWidth);
  for (int i=0; i< this->NumInputImages; i++) r_m[i].Conv(skern,SmoothingWidth); 

  delete[] w_m;
}

// -----------------------------------------------------------
// Intensity Correction 
// -----------------------------------------------------------
// estimate the smoothed image inhomoginities (bias + resiuduum)
// transform r (smoothed weighted residuals) by iv (smoother inv covariances)
// b_m = r_m./iv_m ;

// Results will be written to cY_M

void IntensityCorrection(float** InputVector, int NumInputImages, int DimensionZ, int DimensionY, int DimensionX, EMTriVolume &iv_m, EMVolume *r_m, 
             float *cY_M) {
  double **iv_mat     = new double*[NumInputImages];
  double **inv_iv_mat = new double*[NumInputImages];
  for (int i=0; i < NumInputImages; i++) {
    iv_mat[i]     = new double[NumInputImages];
    inv_iv_mat[i] = new double[NumInputImages];
  }

  float Bias;

  for (int i = 0; i< DimensionZ;i++){
    // Define Bias Value
    for (int j = 0; j< DimensionY;j++){
      for (int k = 0; k< DimensionX;k++){
        for (int l=0; l< NumInputImages ; l++) {
           iv_mat[l][l] = iv_m(l,l,i,j,k);
           for (int m = 0; m<= l; m++) {
              iv_mat[m][l] = iv_mat[l][m] = iv_m(l,m,i,j,k);
          }
        }
        if (InvertMatrix(iv_mat, inv_iv_mat,NumInputImages)) {
          for (int l=0; l< NumInputImages; l++) {
            // Only update those values that are in the region of interest 
            Bias = 0.0;
          for (int m = 0; m< NumInputImages; m++) {
        Bias += inv_iv_mat[l][m]*r_m[m](i,j,k);
          }
          (*cY_M ++) = fabs((*InputVector)[l] - double(Bias));
          }
        } else { 
          for (int l=0; l< NumInputImages; l++) {
          (*cY_M ++) = fabs((*InputVector)[l]);
          }
        }
        InputVector++;
      }
    }
  }

  for (int i=0; i <  NumInputImages; i++) {
    delete[] iv_mat[i];
    delete[] inv_iv_mat[i];
  }
  delete[] iv_mat;
  delete[] inv_iv_mat;
}

int main( int argc, char *argv[] ) {

  // Variables defined by user
  // SmoothingWidth = Default value is 11
  // SmoothinSigma  = Default value is 5
  // LogMu          = mean log intensities of structures
  // LogCovariance  = structure specific covariance of log intensities of structures
  // NumberOfStructures      = Number of Structures of interest

  // inPtr          = Images with image inhomogeneity  
  // w_mPtr         = weights / posterior probabilities resulting from the E-Step of the EM algorithm        

  // Defined by inPtr
  // DimensionZ,DimensionY,DimensionX = Dimension of image 
  // NumInputImages = Number of input chanels (e.g. if inPtr includes T1 and T2 =>  NumInputImages = 2

  // Results are written to cY_MPtr which is the bias corected log intensity of InputVector
  // So I do not save the Bias field currently but you cuold easily change that !

  // Define Log intensities
  int ImageProd = DimensionZ * DimensionY *DimensionX;
  float **InputVector = new float*[ImageProd];
  for(int idx1 = 0; idx1 < ImageProd; idx1++) {
    InputVector[idx1] = new float[NumInputImages];
    for(int idx2 = 0; idx2 < NumInputImages; idx2++) {
      InputVector[idx1][idx2] = log(float(inPtr[idx1][idx2]) +1);
    }
  }


  // Define Smoothing 
  double lbound = (-(this->SmoothingWidth-1)/2); // upper bound = - lbound
  float *skern = new float[this->SmoothingWidth];
  for (int i=0; i < this->SmoothingWidth; i++) skern[i] = float(vtkImageEMGeneral::FastGauss(1.0 / SmoothingSigma,i + lbound));

  // Defined Intensity characteristics 
 double*** InverseLogCov        = new double**[NumberOfStructures];
 for (int i = 0; i < NumberOfStructures; i++) {
    InverseLogCov[i]  = new double*[NumInputImages];
    for (int x = 0; x < NumInputImages; x++)  this->InverseLogCov[i][x]  = new double[NumInputImages];
  }

  if (InvertMatrix(LogCovariance,InvLogCov,NumInputChannels) == 0 ) return 0;

  // Calculate determinant 
  double* InvSqrtDetLogCov     = new double[NumberOfStructures];
  InvSqrtDetInvCov = sqrt(vtkImageEMGeneral::determinant(InvLogCov,NumInputChannels));

  // Allocate memory for results of functions
  EMTriVolume iv_m(NumInputImages,DimensionZ,DimensionY,DimensionX); // weighted inverse covariances 
  EMVolume *r_m  = new EMVolume[NumInputImages]; // weighted residuals
  for (int i=0; i < NumInputImages; i++) r_m[i].Resize(DimensionZ,DimensionY,DimensionX);

  cY_MPtr = new float[NumInputImages*ImageProd]; 
  memset(cY_MPtr, 0, NumInputImages * ImageProd * sizeof(float));


  // Finally do something
  EstimateImageInhomegeneity(InputVector, w_mPtr, InvLogCov, LogMu, NumberOfStructures, skern, SmoothingWidth, DimensionZ, DimensionY, 
                 DimensionX, iv_m, r_m);

  IntensityCorrection(InputVector, NumInputImages, DimensionZ, DimensionY, DimensionX, iv_m, r_m, cY_MPtr);

  // Delete allocated memory 
  // ....
}
