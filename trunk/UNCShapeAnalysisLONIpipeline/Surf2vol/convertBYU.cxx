/*
December 2003
Isabelle Corouge - Code adapted from Joshua Stough
October 2004
adapted by Martin styner to read .iv and allow parcellation

Description: Transform a byu surface into a voxel volume.
*/

#include "convertBYU.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkExceptionObject.h"
#include "itkCastImageFilter.h"
#include <qstring.h>
#include "raw3io.h"
#include <iostream>
#include <string>
#include <limits.h>
#include <cstdio>


convertBYU::convertBYU()
{
  pix = 50;
  deltaxi = 0.0; deltayi = 0.0; deltazi = 0.0;
  deltaxf = 0.0; deltayf = 0.0; deltazf = 0.0;
  imDimX=0; imDimY=0; imDimZ=0;
  minx=DBL_MAX; miny=DBL_MAX; minz=DBL_MAX;
  maxx = 0.0; maxy = 0.0; maxz = 0.0;
  
  hits = (hitstruct***)new hitstruct**[500];
  for (int i=0;i<500;i++)
    {
      hits[i] = (hitstruct**) new hitstruct*[500];
      for (int j=0;j<500;j++)
  hits[i][j] = (hitstruct*)new hitstruct[20];
    }
}

convertBYU::~convertBYU()
{
  // hits
  for (int i=0;i<500;i++)
    {
      for (int j=0;j<500;j++)
  delete [] hits[i][j];
      delete [] hits[i];
    }
  delete [] hits;

  // L1 et T1
  if (L1 != NULL) delete [] L1;
  if (T1 != NULL) delete [] T1;
  
  // inImage
  if (inImage != NULL) delete inImage;
}

void convertBYU::SetDimensions(int x, int y,int z,
                                                             double pxi,double pyi,double pzi,
                                                             double pxf,double pyf,double pzf,
                                                             double add_Frame_Size)
{
  imDimX = x;
  imDimY = y;
  imDimZ = z;
  deltaxi = pxi;
  deltayi = pyi;
  deltazi = pzi;
  deltaxf = pxf;
  deltayf = pyf;
  deltazf = pzf;
  addFrameSize = add_Frame_Size;
}

void convertBYU::ReadInput(const char * byuFilename)
{
  FILE *fp;
  int temp, i;
  int edges_per_poly;
  
  
  std::string filename(byuFilename);
  std::string ivsuffix(".iv");
  fp = fopen(byuFilename, "rb");
  if(fp == NULL) {
    std::cout << "Could not open input file " << filename << std::endl;
    exit (1);
  }
  
  if (filename.substr(filename.length()-ivsuffix.length(), ivsuffix.length()) == ivsuffix) {
    //std::cout << "doing iv reading" << std::endl;

    // iv file reading
    // read file linewise
    char line[500], bracket1[100], bracket2[100];
    
    // find coordinate identifier
    bool endread, entry1Found, entry2Found = false;
    endread = entry1Found = entry2Found = false;
    long locFile = 0;
    
    // how many points
    numVertices = 0;
    while (!endread && fgets(line,500,fp)) {
      if (entry1Found && entry2Found) {
  if (strstr(line, "]")) { // closing bracket
    endread = true;
  } else {
    numVertices++;
  }
      } else {
  if (sscanf(line, " Coordinate3 %s ", bracket1)) {
    entry1Found = true;
  } else if (entry1Found && sscanf(line, " point %s ", bracket2)) {
    entry2Found = true;
    locFile = ftell(fp);
  }
      }
    }
    if (numVertices == 0) {
      std::cout << " No vertices found (Coordinate 3 { point [..]}) in file " << filename << std::endl;
      exit(-1);
    }
    // get vertices first
    L1 = new double[numVertices * 3];
    
    fseek(fp,locFile,SEEK_SET);
    for (int index = 0; index < numVertices; index++){
      fgets(line,500,fp);
      sscanf(line, " %lf %lf %lf ", &(L1[3*index]),&(L1[3*index+1]),&(L1[3*index+2]));
      //std::cout << numVertices << ":" << L1[3*index + 0] << "," << L1[3*index + 1] << "," << L1[3*index + 2] <<"; " << line;
    }

    // how many triangles
    rewind(fp);
    numTriangles = 0;
    endread = entry1Found = entry2Found = false;
    while (!endread && fgets(line,500,fp)) {
      if (entry1Found && entry2Found) {
  if (strstr(line, "]")) { // closing bracket
    endread = true;
  } else {
    numTriangles++;
  }
      } else {
  if (sscanf(line, " IndexedTriangleStripSet %s ", bracket1) ||
      sscanf(line, " IndexedTriangleSet %s ", bracket1)) {
    entry1Found = true;
  } else if (entry1Found && sscanf(line, " coordIndex %s ", bracket2)) {
    entry2Found = true;
    locFile = ftell(fp);
  }
      }
    }
    if (numTriangles == 0) {
      std::cout << " No triangles found (IndexedTriangleSet/IndexedTriangleStripSet 3 { coordIndex [..]}) in file " << filename << std::endl;
      exit(-1);
    }
    T1 = new int [numTriangles * 3];
    temp = numVertices * 3;

    fseek(fp,locFile,SEEK_SET);
    for (int index = 0; index < numTriangles; index++){
      fgets(line,500,fp);
      sscanf(line, " %i , %i , %i ",&(T1[3*index]),&(T1[3*index+1]),&(T1[3*index+2]));
      //std::cout<< numTriangles << T1[3*index +0] << "," << T1[3*index +1] << "," << T1[3*index + 2] << ":" << line ;
    }
    // assuming 1 staring index for triangles, iv has  0 starting index
    for (int index = 0; index < numTriangles * 3; index++){
      T1[index] = T1[index] + 1;
    }
    

  } else {
    // byu file reading
    
    //Pick up byu1's header information. numEdges is not needed.
    fscanf(fp, "1 %d %d %d\n", &numVertices, &numTriangles, &numEdges);
    fscanf(fp, "1 %d\n", &edges_per_poly);
    
    L1 = new double[numVertices * 3];
    T1 = new int [numTriangles * 3];
    temp = numVertices * 3;
    for (i = 0; i < temp; i += 3) {
      fscanf(fp, "%lf %lf %lf\n",&L1[i],&L1[i+1],&L1[i+2]);
    }
    
    // on flippe dans les 3 directions si on reintegre dans image originale. 
    // WHY - mstyner
    if (imDimX !=0 )
      for (i = 0; i < temp; i += 3) {
  L1[i] = imDimX - L1[i];
  L1[i+1] = imDimY - L1[i+1];
  L1[i+2] = imDimZ - L1[i+2];
      }
    
    for (i = 0; i < numTriangles; i ++) {
      fscanf(fp, "%i %i -%i\n",&T1[3*i],&T1[3*i+1],&T1[3*i+2]);

    }
  }
  fclose(fp);


  //CHECK FOR NEGATIVE COORDINATES -> ADD IF NECESSARTY
  //ALL COORDINATES SHOULD BE BIGGERT THAN 0!
  for (i = 0; i < numVertices * 3; i += 3) {  
    if (L1[i] < minx) minx = L1[i];
    if (L1[i] > maxx) maxx = L1[i];
    if (L1[i+1] < miny) miny = L1[i+1];
    if (L1[i+1] > maxy) maxy = L1[i+1];
    if (L1[i+2] < minz) minz = L1[i+2];
    if (L1[i+2] > maxz) maxz = L1[i+2];
  }
  addX = addY = addZ = 0;
  if (minx < 0) addX = fabs(maxx - minx); 
  if (miny < 0) addY = fabs(maxy - miny); 
  if (minz < 0) addZ = fabs(maxz - minz); 
  if (addX || addY || addZ) {
    std::cout << "adjusting coordinates to positive range: " << addX << "," << addY << ","
        << addZ << std::endl;
    for (i = 0; i < numVertices * 3; i += 3) { 
      L1[i] = L1[i] + addX;
      L1[i + 1] = L1[i + 1] + addY;
      L1[i + 2] = L1[i + 2] + addZ;
    }
  }
  minx=DBL_MAX; miny=DBL_MAX; minz=DBL_MAX;
  maxx = 0.0; maxy = 0.0; maxz = 0.0;
}

int compareHitByDistance(const void *arg1, const void *arg2)
{
  hitstruct *h1 = (hitstruct*)arg1;
  hitstruct *h2 = (hitstruct*)arg2;
  if (h1->d < h2->d)
    return -1;
  if (h1->d > h2->d)
    return  1;
  return 0;
}

// add a hit (d,s,b) to first empty slot in ray (i,j)
void convertBYU::addHit(int i, int j, double distance, int sign, int boundary, double e[])
{
  // boundary==1 -> use Ahits
  // boundary==2 -> use Bhits
  hitstruct *h = hits[i][j];
  
  // find first unused entry
  int nHits = 0;
  while (h->boundary)
    {
      if (nHits++ >= 20) {printf("ERROR: exceeded 20 hits per ray - exiting\n"); exit(1);}
      h++;
    }
  
  h->boundary = boundary;
  h->d        = distance;
  h->sign     = sign;
  h->pt.x = e[0]; h->pt.y = e[1]; h->pt.z = e[2] + distance;
}



double convertBYU::FillHitsForDist()
  //This fills the hits array for symmetric distance function computation.
  //The difference between this FillHits and the above one for volume overlap
  //is that here was want cubic voxels for the diffusion process described by
  //Yezzi.
{
  int i, j, k, t, temp;  // CAREFUL! change size of Ahits if you change this! 
  double dA;
  double e[3], n[3];  //Point and normal of the plane.
    
  // reset intersection hits
  for (i=0;i<500;i++)
    for (j=0;j<500;j++)
      for (k=0;k<20;k++){
  hits[i][j][k].d = 0;
  hits[i][j][k].sign = 0;  
  hits[i][j][k].boundary = 0;
  hits[i][j][k].pt = Vector3D(0,0,0);
      }
  
  //memset((void*)hits, 0, sizeof(hits));
  
  temp = 3*numVertices;
  for (i = 0; i < temp; i = i + 3) {
    if (L1[i] < minx) minx = L1[i];
    if (L1[i] > maxx) maxx = L1[i];
    if (L1[i+1] < miny) miny = L1[i+1];
    if (L1[i+1] > maxy) maxy = L1[i+1];
    if (L1[i+2] < minz) minz = L1[i+2];
    if (L1[i+2] > maxz) maxz = L1[i+2];
  }
  
  printf("\nBYU surface extents (x y z): \n[%f %f]\n[%f %f]\n[%f %f]\n", 
   minx,maxx,miny,maxy,minz,maxz);
  
  //Recompute to a bounding cube.  Let's make it symmetric about the 
  //objects too, rather than have the objects in a corner.
  
  Vector3D sides(maxx - minx, maxy - miny, maxz - minz);
  Vector3D mids(minx + sides.x/2.0, miny + sides.y/2.0, minz + sides.z/2.0); 
  double enlargementFactor = 1.0 + addFrameSize;
  sides *= enlargementFactor;
  printf("EnlargementFactor = %lf\n" , enlargementFactor);
  
  //I don't want to waste voxels, so the bounding box shouldn't be a cube.  
  pixes[0] = (int) ceil(sides.x*deltaxi/deltaxf + 1);
  pixes[1] = (int) ceil(sides.y*deltayi/deltayf + 1);
  pixes[2] = (int) ceil(sides.z*deltazi/deltazf + 1);  
  sides.x = pixes[0]*(deltaxf/deltaxi);
  sides.y = pixes[1]*(deltayf/deltayi);
  sides.z = pixes[2]*(deltazf/deltazi);
      
  Vector3D halves(0.0,0.0,0.0);
  halves = sides/2.0;
  
  minx = mids.x - halves.x; maxx = mids.x + halves.x;
  miny = mids.y - halves.y; maxy = mids.y + halves.y;
  minz = mids.z - halves.z; maxz = mids.z + halves.z;
  
  printf("Enlarged bounding box in original resolution (x y z): \n[%f %f]\n[%f %f]\n[%f %f]\n",    minx,maxx,miny,maxy,minz,maxz);
  
  //Now we have nearly the smallest bounding cube.  I want to leave voxels on
  //the outside in case the Yezzi code needs it.
  
  e[0] = minx; e[1] = miny; e[2] = minz;
  n[0] =  0.0; n[1] = 0.0; n[2] = 1.0;
  dA = deltaxf*deltayf;
  
  //  printf("pix = %d %d %d, thus column base area = %lf ( %f * %f )\n", 
  //   pixes[0], pixes[1], pixes[2], dA, deltaxf, deltayf);
  printf("Bounding box dimensions in final resolution: pix = %d %d %d \n", 
   pixes[0], pixes[1], pixes[2]);

  int vert;
  // for each tile, intersect rays that cover it's bounding box with the tile
  for (t = 0; t < numTriangles; t++)
    {
      // find tile's bb
      vert = T1[3*t + 0]-1;
      double tileMinx =        L1[vert*3 + 0];
      double tileMaxx =        L1[vert*3 + 0];
      double tileMiny =        L1[vert*3 + 1];
      double tileMaxy =        L1[vert*3 + 1];
      
      vert = T1[3*t + 1]-1;
      tileMinx = MIN(tileMinx, L1[vert*3 + 0]);
      tileMaxx = MAX(tileMaxx, L1[vert*3 + 0]);    
      tileMiny = MIN(tileMiny, L1[vert*3 + 1]);
      tileMaxy = MAX(tileMaxy, L1[vert*3 + 1]);
      
      vert = T1[3*t + 2]-1;
      tileMinx = MIN(tileMinx, L1[vert*3 + 0]);
      tileMaxx = MAX(tileMaxx, L1[vert*3 + 0]);    
      tileMiny = MIN(tileMiny, L1[vert*3 + 1]);
      tileMaxy = MAX(tileMaxy, L1[vert*3 + 1]);
      
      // now intersect all rays in bb with the tile and record in Ahits
      double tuv[3], dst;
      int startx = int((tileMinx - minx)*deltaxi / deltaxf);
      int   endx = int((tileMaxx - minx)*deltaxi / deltaxf);
      int starty = int((tileMiny - miny)*deltayi / deltayf);
      int   endy = int((tileMaxy - miny)*deltayi / deltayf);
      for (i = startx; i <= endx; i++){
   for (j = starty; j <= endy; j++){
     e[0] = minx + deltaxf/(2.0*deltaxi) + i*(deltaxf/deltaxi);
     e[1] = miny + deltayf/(2.0*deltayi) + j*(deltayf/deltayi);
     e[2] = minz + deltazf/(2.0*deltazi);
     dst = Intersect_tri(e,n,tuv,3*t);
     if (dst > -1.0e3) {  //A real hit, so append to the hitlist.
       int sgn = (int) (tuv[1]/fabs(tuv[1]));
       addHit(i, j, fabs(dst), sgn, 1, e);
     }
   }              
      }
    }
  
  return dA;
}

void convertBYU::TransformWorldToImage(double * point)
{
  point[0] = (point[0] + addX - minx) * deltaxi / deltaxf;
  point[1] = (point[1] + addY - miny) * deltayi / deltayf;
  point[2] = (point[2] + addZ - minz) * deltazi / deltazf;
}

void convertBYU::FillP(double & volume)
{
  int i, j, k;

  FillHitsForDist();

  inImage = new Image3D(pixes[0], pixes[1], pixes[2]);
  inImage->setSpacing(deltaxf, deltayf, deltazf);
  //  inImage->setOriginPixelPos(Vector3D(minx,miny,minz));
  GreyValue val=MIN_GREY_VALUE;
  inImage->clear(val);
  
  double lastDist = -1;  // beginning of last segment
  
  double stepper, maxd = maxz - (deltazf/deltazi)/3.0 - minz;
  //I use the /3.0 instead of the obvious 2.0 for numerical reasons.
  //zinc/2.0 + 99*zinc is sometimes different from the obvious maxd.
  

  int count = 0, badcount = 0;
  for (i = 0; i < pixes[1]; i ++){
    for (j = 0; j < pixes[0]; j ++){
      stepper = maxz + 1;
      
      // no hits on this ray? -> fill the column with 'outside' number
      if (! hits[j][i][0].boundary){
  for (k = 0; k < pixes[2]; k ++){
    inImage->setVoxel(j,i,k,0);
    badcount ++;
  }
      }
      
      //Otherwise, this column has hits.  A column's intersections can go
      //like this:
      //-before first hit, all 100's (as in outside both).
      //-between first hit and hitting second object, 0's.
      //-after getting inside both objects, > 200's.
      
      hitstruct *h;  // local hit ptr
      int nHits = 0;
      
      // if A or B have an odd hit count, fail
      h = hits[j][i];
      while (h->boundary){
  nHits++;
  h++;
      }
   
      // sort hits by distance
      h = hits[j][i];
      qsort( (void *)h, (size_t)nHits, sizeof(hitstruct), compareHitByDistance);
    
      //Now the hits are sorted by distance.  We can loop through, filling
      //out voxels along the way.
    
      // calc d[]
      // - add this segment's distance, if positive, to proper counters
      // - update to look at next intersection
      //CHANGE this to loop along the current segment, filling the 
      //relevant voxels.
      
      //object B will be where u = 1.  object A where u = 0;
    
      lastDist = -1;  // beginning of last segment
        
      h = hits[j][i];
      while (h->boundary){
  // add the distance from the last intersection into the appropriate d[] counters
  if (lastDist != -1)  // there's no segment (to add) before the first one
    {      
      while (stepper <= h->d){
        inImage->setVoxel(j,i,k,MAX_GREY_VALUE);
        k++; count ++;
        stepper += (deltazf/deltazi);
      }            
    }//there is stuff to do before the first segment.
  else { //compute dist from floor to boundary and fill all those voxels.
    k = 0;
    //stepper = minz + deltazf/2.0;
    //stepper = 0.0;
    stepper = (deltazf/deltazi)/2.0;
    while(stepper <= h->d){
      inImage->setVoxel(j,i,k,0);
      k++; badcount ++;
      stepper += deltazf/deltazi;
    }      
  }
        
  lastDist = h->d;
        
  // recalc which surface has just been entered or exited
  h++;
      }
    
      //Through all of both surfaces, the rest is 100's.
    
      while(stepper <= maxd){
  inImage->setVoxel(j,i,k,0);
  k++; badcount ++;
  stepper += deltazf/deltazi;
      }    
    }
  }
  //  printf("segments now computed, count %i badcount %i.\n", count, badcount);
  // calcul du volume
  volume = 0.0;
  int comptVoxels = 0;
  for (int z=0 ; z<inImage->getZDim() ; z++)
    for (int y=0 ; y<inImage->getYDim() ; y++)
      for (int x=0 ; x<inImage->getXDim() ; x++)
  if(inImage->getVoxelValue(x,y,z)!=0) comptVoxels ++;

  volume = comptVoxels * inImage->getXSpacing() * inImage->getYSpacing() * inImage->getZSpacing();

} //FIN


void convertBYU::WriteVolume(const char* imagename,int type)
{
  //std::cout << "Write Volume ... " << std::endl;
  int m_maxgreyvalue;
  if (type == 0)
    m_maxgreyvalue = 65535;
  else
    m_maxgreyvalue = 255;
  
  typedef ImageType::Pointer ImagePointer;
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  
  ImagePointer m_outputimage = ImageType::New();
  
  int imagesize[3];
  imagesize[0] = inImage->getXDim();
  imagesize[1] = inImage->getYDim();
  imagesize[2] = inImage->getZDim();
  ImageType::SpacingType  spacing;
  spacing[0] = inImage->getXSpacing();
  spacing[1] = inImage->getYSpacing();
  spacing[2] = inImage->getZSpacing();
  
  float values[3];
  values[0]= spacing[0]; 
  values[1]= spacing[1];
  values[2]= spacing[2];
  float origin_x= ((imagesize[0]/2)*values[0]*(-1));
  float origin_y= ((imagesize[1]/2)*values[1]*(-1));
  float origin_z= ((imagesize[0]/2)*values[2]*(-1));
  float origin[3] = {origin_x, origin_y, origin_z};
  
  ImageType::RegionType region;
  ImageType::SizeType size;
  size[0]= imagesize[0];
  size[1]= imagesize[1];
  size[2]= imagesize[2];
  region.SetSize( size );
  m_outputimage->SetRegions( region );
  m_outputimage->Allocate();
  m_outputimage->SetOrigin(origin);
  m_outputimage->SetSpacing(values);

  //Copy data to the ITK image
  IteratorType m_outputitimage(m_outputimage,m_outputimage->GetLargestPossibleRegion());
  m_outputitimage.GoToBegin();
  
  for (unsigned int z=0;z<size[2];z++)
    for (unsigned int y=0;y<size[1];y++)
      for (unsigned int x=0;x<size[0];x++)
  {
    m_outputitimage.Set(inImage->getVoxelValue(x,y,z));
//     if (inImage->getVoxelValue(x,y,z)!=0)
//       m_outputitimage.Set(m_maxgreyvalue);
//     else
//       m_outputitimage.Set(0); //inImage->getVoxelValue(x,y,z));
    
    ++m_outputitimage;
  }
  
  std::cout << "Write Volume: " << imagename << std::endl;
  if (QString(imagename).mid(QString(imagename).findRev(".")) == ".raw3")
    {
      int m_type = 65535;
      if (type !=0)
  {
    m_type = 256;
    char* charImage = new char[imagesize[0]*imagesize[1]*imagesize[2]]; 
    for (int z=0;z<imagesize[2];z++)
      for (int y=0;y<imagesize[2];y++)
        for (int x=0;x<imagesize[2];x++)
    charImage[z*imagesize[0]*imagesize[1] + y*imagesize[0] + x]  = (char)inImage->getVoxelValue(x,y,z);
    
    Raw3IO raw3img(imagesize[0],imagesize[1],imagesize[2],m_type,charImage,spacing[0],spacing[1],spacing[2]);
    raw3img.save(imagename);
  }
      else
  {
    Raw3IO raw3img(imagesize[0],imagesize[1],imagesize[2],m_type,inImage->getVoxelAddr(0,0,0),spacing[0],spacing[1],spacing[2]);
    raw3img.save(imagename);
  }
    }  
  else
    {
      if (type ==0)
  {
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(imagename);
    writer->SetInput(m_outputimage);
    writer->Update();
  }
      else
  {
    typedef itk::CastImageFilter<ImageType,ImageTypeChar> CastFilterType;
    CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(m_outputimage);
    castFilter->Update();
    WriterTypeChar::Pointer writer = WriterTypeChar::New();
    writer->SetFileName(imagename);
    writer->SetInput(castFilter->GetOutput());
    writer->Update();  
  }
    }
}


void convertBYU::WriteImage(const char* imagename, int type)
{
  int m_maxgreyvalue;
  if (type == 0)
    m_maxgreyvalue = 65535;
  else
    m_maxgreyvalue = 255;

  // back to original image, at any resolution
  std::cout<<"imDim : "<<imDimX<<" "<<imDimY<<" "<<imDimZ<<std::endl;
  if((imDimX !=0 && imDimY != 0 && imDimZ !=0) 
     && (deltaxf !=0 && deltayf !=0 && deltazf !=0))
    {
      int voxminx=(int)floor(minx*deltaxi/deltaxf);
      int voxminy=(int)floor(miny*deltayi/deltayf);
      int voxminz=(int)floor(minz*deltazi/deltazf);
      int voxmaxx=voxminx+pixes[0];
      int voxmaxy=voxminy+pixes[1];
      int voxmaxz=voxminz+pixes[2];
  
      typedef itk::Image<unsigned short,3> ImageType;
      typedef ImageType::Pointer ImagePointer;
      typedef itk::ImageRegionIterator<ImageType> IteratorType;
      ImagePointer m_outputimage = ImageType::New();

      if (type == 0){
  typedef ImageType::Pointer ImagePointer;
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  ImagePointer m_outputimage = ImageType::New();
      }

      int imagesize[3];
      imagesize[0] = (int)floor(imDimX*deltaxi/deltaxf);
      imagesize[1] = (int)floor(imDimY*deltayi/deltayf); 
      imagesize[2] = (int)floor(imDimZ*deltazi/deltazf); 
      ImageType::SpacingType  spacing;
      spacing[0] = deltaxf; 
      spacing[1] = deltayf; 
      spacing[2] = deltazf;
   
      float values[3];
      values[0]= spacing[0]; 
      values[1]= spacing[1];
      values[2]= spacing[2];
      float origin_x= ((imagesize[0]/2)*values[0]*(-1));
      float origin_y= ((imagesize[1]/2)*values[1]*(-1));
      float origin_z= ((imagesize[0]/2)*values[2]*(-1));
      float origin[3] = {origin_x, origin_y, origin_z};
 
      ImageType::RegionType region;
      ImageType::SizeType size;
      size[0]= imagesize[0];
      size[1]= imagesize[1];
      size[2]= imagesize[2];
      region.SetSize( size );
      m_outputimage->SetRegions( region );
      m_outputimage->Allocate();
      m_outputimage->SetOrigin(origin);
      m_outputimage->SetSpacing(values);

      //Copy data to the ITK image
      IteratorType m_outputitimage(m_outputimage,m_outputimage->GetLargestPossibleRegion());
      m_outputitimage.GoToBegin();
      ImageType::SizeType m_imagesize = (m_outputimage->GetLargestPossibleRegion().GetSize());
    
      for (int z=0;z<m_imagesize[2];z++)
  for ( int y=0;y<m_imagesize[1];y++)
    for ( int x=0;x<m_imagesize[0];x++)
      {
  if ((x>=voxminx) && (x<voxmaxx) && (y>=voxminy) && (y<voxmaxy)  \
      && (z>=voxminz) && (z<voxmaxz))
    {
      
      m_outputitimage.Set(inImage->getVoxelValue(x,y,z));
//       if(inImage->getVoxelValue(x-voxminx,y-voxminy,z-voxminz)!=0)
//         m_outputitimage.Set(m_maxgreyvalue);
//       //inImage->getVoxelValue(x-voxminx,y-voxminy,z-voxminz));
//       else
//         m_outputitimage.Set(0);
    }
  else
    m_outputitimage.Set(0);
  
  ++m_outputitimage;
    }
      
    std::cout << "Write Image: " << imagename << std::endl;
    if (QString(imagename).mid(QString(imagename).findRev(".")) == ".raw3")
      {
  int m_type = 65535;
  if (type !=0)
    {
      m_type = 256;
      char* charImage = new char[imagesize[0]*imagesize[1]*imagesize[2]]; 
      for (int z=0;z<imagesize[2];z++)
        for (int y=0;y<imagesize[2];y++)
    for (int x=0;x<imagesize[2];x++)
      charImage[z*imagesize[0]*imagesize[1] + y*imagesize[0] + x]  = (char)inImage->getVoxelValue(x,y,z);
      
      Raw3IO raw3img(imagesize[0],imagesize[1],imagesize[2],m_type,charImage,spacing[0],spacing[1],spacing[2]);
      raw3img.save(imagename);
    }
  else
    {
      Raw3IO raw3img(imagesize[0],imagesize[1],imagesize[2],m_type,inImage->getVoxelAddr(0,0,0),spacing[0],spacing[1],spacing[2]);
      raw3img.save(imagename);
    }
      }  
    else
      {
  if (type ==0)
    {
      WriterType::Pointer writer = WriterType::New();
      writer->SetFileName(imagename);
      writer->SetInput(m_outputimage);
      writer->Update();
    }
  else
    {
      typedef itk::CastImageFilter<ImageType,ImageTypeChar> CastFilterType;
      CastFilterType::Pointer castFilter = CastFilterType::New();
      castFilter->SetInput(m_outputimage);
      castFilter->Update();
      WriterTypeChar::Pointer writer = WriterTypeChar::New();
      writer->SetFileName(imagename);
      writer->SetInput(castFilter->GetOutput());
      writer->Update();  
    }
      }  
  }
  
}


double convertBYU::Intersect_tri(double e[], double v[], double tuv[], int trind)
  //Returns the extent along v from e to hit the triangle (negative if 
  //non-intersect). In the end, tuv[0] will contain a scaler for v, and 
  //tuv[1] will be a sign, positive if the triangle's normal points to e.
  //Optimized by assuming orthographic projection.  I return false before 
  //much computation if e[0] e[1] is not within the bounding x y of the
  //triangle.  This should prevent several dozen multiplications and mallocs.
{
  double edge1[3], edge2[3];
  int i;
  
  for (i = 0; i < 3; i ++) {
    edge1[i] = L1[(T1[trind+1]-1)*3+i] - L1[(T1[trind]-1)*3+i]; //t->p1[i] - t->p0[i];
    edge2[i] = L1[(T1[trind+2]-1)*3+i] - L1[(T1[trind]-1)*3+i]; //t->p2[i] - t->p0[i];
  }
  
  double tvec[3], pvec[3], qvec[3], det,inv_det;
  
  //Cross v and edge2.
  pvec[0] = v[1]*edge2[2] - v[2]*edge2[1];
  pvec[1] = v[2]*edge2[0] - v[0]*edge2[2];
  pvec[2] = v[0]*edge2[1] - v[1]*edge2[0];
  
  //dot edge1 with the cross of v and edge2.  If the dot is close to 
  //zero, the v is in the plane of the triangle.
  det = edge1[0]*pvec[0] + edge1[1]*pvec[1] + edge1[2]*pvec[2];
  
  if (fabs(det) < 1.0e-10) return -10000.0;
  
  inv_det = 1.0/det;
  
  //Distance from vert0 to ray origin.
    tvec[0] = e[0]- L1[(T1[trind]-1)*3]; //t->p0[0];
    tvec[1] = e[1]- L1[(T1[trind]-1)*3+1]; //t->p0[1];
    tvec[2] = e[2]- L1[(T1[trind]-1)*3+2]; //t->p0[2];
  
  //Calculate u and test bounds
  tuv[1] = tvec[0]*pvec[0] + tvec[1]*pvec[1] + tvec[2]*pvec[2];
  tuv[1] = tuv[1]*inv_det;
  if ((tuv[1] < -1.0e-10)||(tuv[1] > 1.0)) return -10000.0;
  
  //Prepare for v parameter.
  qvec[0] = tvec[1]*edge1[2] - tvec[2]*edge1[1];
  qvec[1] = tvec[2]*edge1[0] - tvec[0]*edge1[2];
  qvec[2] = tvec[0]*edge1[1] - tvec[1]*edge1[0];
  
  //Calculate v and test bounds
  tuv[2] = v[0]*qvec[0] + v[1]*qvec[1] + v[2]*qvec[2];
  tuv[2] = tuv[2]*inv_det;
  
  if ((tuv[2] < -1.0e-10)||(tuv[1] + tuv[2] > 1.0)) return -10000.0;
  
  //Calculate t.
  tuv[0] = edge2[0]*qvec[0] + edge2[1]*qvec[1] + edge2[2]*qvec[2];
  tuv[0] = tuv[0]*inv_det;
  
  //We'll now put into tuv[1] the sign of the hit, v dotted with the 
  //normal to the triangle. Reusing variables.
  
  pvec[0] = edge1[1]*edge2[2] - edge1[2]*edge2[1];
  pvec[1] = edge1[2]*edge2[0] - edge1[0]*edge2[2];
  pvec[2] = edge1[0]*edge2[1] - edge1[1]*edge2[0];
  
  //pvec is the normal to the triangle. tuv[1] used to be extent along the 
  //u barrycentric vector.
  tuv[1] = v[0]*pvec[0] + v[1]*pvec[1] + v[2]*pvec[2];
  
 
  //We also scale the distance t by the length of vector v, so that
  //this t is comparable to those returned by other functions.
  
  //tuv[0] = tuv[0]/(sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]));
  
  return tuv[0];
}
