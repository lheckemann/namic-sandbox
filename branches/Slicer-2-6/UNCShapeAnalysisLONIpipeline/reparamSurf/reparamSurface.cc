// reparametrize a surface
// Martin styner December 2004 
//

#include <cstdio>
#include <iostream>
#include <vector>

bool inBetween(double * vec1, double * vec2, double *Pvec)
{
  // if vec1 x Pvec * vec2 x Pvec < 0 -> true
  return (((Pvec[1] * vec1[0] - Pvec[0] * vec1[1]) * (Pvec[1] * vec2[0] - Pvec[0] * vec2[1]) + 
     (Pvec[0] * vec1[2] - Pvec[2] * vec1[0]) * (Pvec[0] * vec2[2] - Pvec[2] * vec2[0]) + 
     (Pvec[2] * vec1[1] - Pvec[1] * vec1[2]) * (Pvec[2] * vec2[1] - Pvec[1] * vec2[2])) 
    < 0 );
}

inline double Eucldist (double * from, double * to)
{
  return sqrt((from[0] - to[0])*(from[0] - to[0]) + (from[1] - to[1])*(from[1] - to[1]) +
        (from[2] - to[2])*(from[2] - to[2]));
}

int reparamSurface(int numReparamVertices, double * &surfReparamVertList, double * reparamVertList, 
       int numVertices, double * vertList, double * parVertList,
       int numTriangles, int * triangIndexList)
{
  int firstPoint, secondPoint, thirdPoint; 
  double epsilon = 1e-8;
  if (surfReparamVertList) {
    delete surfReparamVertList; // re-allocate
    surfReparamVertList = NULL;
  }
  surfReparamVertList = new double [numReparamVertices * 3];

  double* normalList  = new double [numTriangles * 3];
  //std::cout << numReparamVertices << "," << numTriangles << std::endl;
  // compute normals for all triangles
  double max_area = 0;
  for (int Tindex = 0 ; Tindex < numTriangles; Tindex ++ ) {
    firstPoint =  triangIndexList[Tindex * 3 + 0];
    secondPoint = triangIndexList[Tindex * 3 + 1];
    thirdPoint =  triangIndexList[Tindex * 3 + 2];
    double vec1[3], vec2[3];
    for (int i = 0 ; i < 3; i ++) {
      vec1[i] = parVertList[firstPoint  * 3 + i] - parVertList[secondPoint * 3 + i];
      vec2[i] = parVertList[thirdPoint  * 3 + i] - parVertList[secondPoint * 3 + i];
    }
    //cross product
    normalList[Tindex * 3 + 0] =  vec1[1] * vec2[2] - vec1[2] * vec2[1];
    normalList[Tindex * 3 + 1] =  vec1[2] * vec2[0] - vec1[0] * vec2[2];
    normalList[Tindex * 3 + 2] =  vec1[0] * vec2[1] - vec1[1] * vec2[0];
    // normalize
    double length = sqrt(normalList[Tindex * 3 + 0] * normalList[Tindex * 3 + 0] + 
      normalList[Tindex * 3 + 1] * normalList[Tindex * 3 + 1] +
      normalList[Tindex * 3 + 2] * normalList[Tindex * 3 + 2] );
    if (length > max_area) max_area = length;
    for (int i = 0 ; i < 3; i ++) {
      normalList[Tindex * 3 + i] = normalList[Tindex * 3 + i] / length;
    }
  }
  // max_area is the area of the largest triangle * 2
  double min_dist = sqrt(max_area) * 3; // minimum distance to consider a point
  //std::cout << min_dist << std::endl;

  // for all vertices in the reparametrization
  double incr =  numReparamVertices / 50;
  double progress = 0;

  for (int repIndex = 0 ; repIndex < numReparamVertices; repIndex ++ ) {
    if (progress > incr) {
      std::cout <<"."; std::cout.flush();
      progress = 0;
    }
    progress++;

    double closestPoint[3];
    int closestTriangleIndex = 0;
    double dist = 100;
    int closestmode = 0;

    //find closest triangle on spherical parametrization to reparam VertList[repIndex]
    // dist is bigger or equal then the minima distance to the closest triangle
    for (int Tindex = 0 ; Tindex < numTriangles; Tindex ++ ) {
      firstPoint =  triangIndexList[Tindex * 3 + 0];
      secondPoint = triangIndexList[Tindex * 3 + 1];
      thirdPoint =  triangIndexList[Tindex * 3 + 2];
      double pointDist = Eucldist(&(parVertList[secondPoint * 3]),&(reparamVertList[repIndex * 3]));
      if (pointDist < min_dist) { // That's a huge distance on the unit sphere
  double triangleDist = 10;
  double planeDist = 
    normalList[Tindex * 3 + 0] * (reparamVertList[repIndex * 3 + 0] - 
          parVertList[firstPoint * 3 + 0]) +
    normalList[Tindex * 3 + 1] * (reparamVertList[repIndex * 3 + 1] - 
          parVertList[firstPoint * 3 + 1]) +
    normalList[Tindex * 3 + 2] * (reparamVertList[repIndex * 3 + 2] - 
          parVertList[firstPoint * 3 + 2]);
  
  double pointInPlane[3];
  for (int i = 0 ; i < 3; i ++) {
    pointInPlane[i] = reparamVertList[repIndex * 3 + i] - normalList[Tindex * 3 + i] * planeDist;
  }
  //compute projections onto side-unit vectors -> in triangle or not and where etc
  double side1[3], side2[3], side3[3];
  for (int i = 0 ; i < 3; i ++) {
    side1[i] = parVertList[firstPoint  * 3 + i] - parVertList[secondPoint * 3 + i];
    side2[i] = parVertList[thirdPoint  * 3 + i] - parVertList[secondPoint * 3 + i];
    side3[i] = parVertList[thirdPoint  * 3 + i] - parVertList[firstPoint  * 3 + i];
  }
  double side1length = sqrt(side1[0] * side1[0] + side1[1] * side1[1] + side1[2] * side1[2]);
  double side2length = sqrt(side2[0] * side2[0] + side2[1] * side2[1] + side2[2] * side2[2]);
  double side3length = sqrt(side3[0] * side3[0] + side3[1] * side3[1] + side3[2] * side3[2]);
  for (int i = 0 ; i < 3; i ++) {
    side1[i] = side1[i] / side1length;
    side2[i] = side2[i] / side2length;
    side3[i] = side3[i] / side3length;
  }
  
  double diff1[3],diff2[3],diff3[3];
  for (int i = 0 ; i < 3; i ++) {
    diff1[i] = pointInPlane[i]  - parVertList[secondPoint * 3 + i];
    diff2[i] = pointInPlane[i]  - parVertList[secondPoint * 3 + i];
    diff3[i] = pointInPlane[i]  - parVertList[firstPoint  * 3 + i];
  }
  
  //3 sideness tests
  double norm[3];
  double length;
  norm[0] =  side1[1] * diff1[2] - side1[2] * diff1[1];
  norm[1] =  side1[2] * diff1[0] - side1[0] * diff1[2];
  norm[2] =  side1[0] * diff1[1] - side1[1] * diff1[0];
  double sideness1 = norm[0] * normalList[Tindex * 3 + 0] + norm[1] * normalList[Tindex * 3 + 1]
    + norm[2] * normalList[Tindex * 3 + 2]; 
  norm[0] =  side2[1] * diff2[2] - side2[2] * diff2[1];
  norm[1] =  side2[2] * diff2[0] - side2[0] * diff2[2];
  norm[2] =  side2[0] * diff2[1] - side2[1] * diff2[0];
  double sideness2 = norm[0] * normalList[Tindex * 3 + 0] + norm[1] * normalList[Tindex * 3 + 1]
    + norm[2] * normalList[Tindex * 3 + 2]; 
  norm[0] =  side3[1] * diff3[2] - side3[2] * diff3[1];
  norm[1] =  side3[2] * diff3[0] - side3[0] * diff3[2];
  norm[2] =  side3[0] * diff3[1] - side3[1] * diff3[0];
  double sideness3 = norm[0] * normalList[Tindex * 3 + 0] + norm[1] * normalList[Tindex * 3 + 1]
    + norm[2] * normalList[Tindex * 3 + 2]; 
  
  double proj1 = diff1[0] * side1[0] + diff1[1] * side1[1] + diff1[2] * side1[2];
  double proj2 = diff1[0] * side1[0] + diff1[1] * side1[1] + diff1[2] * side1[2];
  double proj3 = diff1[0] * side1[0] + diff1[1] * side1[1] + diff1[2] * side1[2];
  
  if ((sideness1+epsilon) >= 0.0 && (sideness2-epsilon) <= 0.0 && (sideness3+epsilon) >= 0.0) { // inside
    triangleDist = planeDist;
  } else { // outside
    if ((sideness1-epsilon) <= 0.0){ 
      if ( proj1 <= 0 ) { 
        for (int i = 0 ; i < 3; i ++) { pointInPlane[i] = parVertList[secondPoint* 3 + i];}
      } else if ( proj1 >= side1length) {
        for (int i = 0 ; i < 3; i ++) { pointInPlane[i] = parVertList[firstPoint* 3 + i];}
      } else {
        for (int i = 0 ; i < 3; i ++) { 
    pointInPlane[i] = parVertList[secondPoint * 3 + i] + proj1 * side1[i];
        }
      }
    } else if ((sideness2+epsilon) >= 0.0){  
      if ( proj2 <= 0 ) { 
        for (int i = 0 ; i < 3; i ++) { pointInPlane[i] = parVertList[secondPoint* 3 + i];}
      } else if ( proj2 >= side2length) {
        for (int i = 0 ; i < 3; i ++) { pointInPlane[i] = parVertList[thirdPoint* 3 + i];}
      } else {
        for (int i = 0 ; i < 3; i ++) { 
    pointInPlane[i] = parVertList[secondPoint * 3 + i] + proj2 * side2[i];
        }
      }
    } else if ((sideness3-epsilon) <= 0.0){  
      if ( proj3 <= 0 ) { 
        for (int i = 0 ; i < 3; i ++) { pointInPlane[i] = parVertList[secondPoint* 3 + i];}
      } else if ( proj2 >= side2length) {
        for (int i = 0 ; i < 3; i ++) { pointInPlane[i] = parVertList[thirdPoint* 3 + i];}
      } else {
        for (int i = 0 ; i < 3; i ++) { 
    pointInPlane[i] = parVertList[firstPoint * 3 + i] + proj3 * side3[i];
        }
      }
    }
  }
  triangleDist = sqrt((pointInPlane[0] - reparamVertList[repIndex * 3 + 0]) * 
          (pointInPlane[0] - reparamVertList[repIndex * 3 + 0]) +
          (pointInPlane[1] - reparamVertList[repIndex * 3 + 1]) * 
          (pointInPlane[1] - reparamVertList[repIndex * 3 + 1]) +
          (pointInPlane[2] - reparamVertList[repIndex * 3 + 2]) * 
          (pointInPlane[2] - reparamVertList[repIndex * 3 + 2])) ;
  if (triangleDist < dist ) {
    dist = triangleDist;
    closestTriangleIndex = Tindex;
    for (int i = 0 ; i < 3; i ++) { closestPoint[i] = pointInPlane[i];}
  } 
      }
    }

    firstPoint =  triangIndexList[closestTriangleIndex * 3 + 0];
    secondPoint = triangIndexList[closestTriangleIndex * 3 + 1];
    thirdPoint =  triangIndexList[closestTriangleIndex * 3 + 2];
    double e1[3], e2[3], norm[3];
    for (int i = 0 ; i < 3; i ++) {
      e1[i] = parVertList[firstPoint  * 3 + i] - parVertList[secondPoint * 3 + i];
      e2[i] = parVertList[thirdPoint  * 3 + i] - parVertList[secondPoint * 3 + i];
    }
    norm[0] =  e1[1] * e2[2] - e1[2] * e2[1];
    norm[1] =  e1[2] * e2[0] - e1[0] * e2[2];
    norm[2] =  e1[0] * e2[1] - e1[1] * e2[0];
    double normlength = sqrt(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);
    for (int i = 0 ; i < 3; i ++) { norm [i] = norm[i] / normlength;}

    // normalize
    double e2length = sqrt(e2[0] * e2[0] + e2[1] * e2[1] + e2[2] * e2[2]);
    double e1length = sqrt(e1[0] * e1[0] + e1[1] * e1[1] + e1[2] * e1[2]);
    e2[0] =  e1[2] * norm[1] - e1[1] * norm[2];
    e2[1] =  e1[0] * norm[2] - e1[2] * norm[0];
    e2[2] =  e1[1] * norm[0] - e1[0] * norm[1];
    for (int i = 0 ; i < 3; i ++) { e1[i] = e1[i] / e1length;}
    double e21length = sqrt(e2[0] * e2[0] + e2[1] * e2[1] + e2[2] * e2[2]);
    for (int i = 0 ; i < 3; i ++) { e2[i] = e2[i] / e21length;}

    // local triangle coordinates
    double coor1 = (closestPoint[0] - parVertList[secondPoint * 3 + 0]) * e1[0] + 
      (closestPoint[1] - parVertList[secondPoint * 3 + 1]) * e1[1] +
      (closestPoint[2] - parVertList[secondPoint * 3 + 2]) * e1[2];
    double coor2 = (closestPoint[0] - parVertList[secondPoint * 3 + 0]) * e2[0] + 
      (closestPoint[1] - parVertList[secondPoint * 3 + 1]) * e2[1] +
      (closestPoint[2] - parVertList[secondPoint * 3 + 2]) * e2[2];
    
    //    std::cout << coor1/e1length << "," << coor2/e2length << std::endl;
    
    if (coor1 > e1length ) coor1 = e1length;
    if (coor1 < 0  ) coor1 = 0;
    if (coor2 > e2length ) coor2 = e2length;
    if (coor2 < 0  ) coor2 = 0;

    double es1[3], es2[3], snorm[3];
    for (int i = 0 ; i < 3; i ++) {
      es1[i] = vertList[firstPoint  * 3 + i] - vertList[secondPoint * 3 + i];
      es2[i] = vertList[thirdPoint  * 3 + i] - vertList[secondPoint * 3 + i];
    }
    snorm[0] =  es1[1] * es2[2] - es1[2] * es2[1];
    snorm[1] =  es1[2] * es2[0] - es1[0] * es2[2];
    snorm[2] =  es1[0] * es2[1] - es1[1] * es2[0];
    double snormlength = sqrt(snorm[0] * snorm[0] + snorm[1] * snorm[1] + snorm[2] * snorm[2]);
    for (int i = 0 ; i < 3; i ++) { snorm [i] = snorm[i] / snormlength;}
    // snormlength = 2 * triangle area of real object

    // normalize
    double es2length = sqrt(es2[0] * es2[0] + es2[1] * es2[1] + es2[2] * es2[2]);
    double es1length = sqrt(es1[0] * es1[0] + es1[1] * es1[1] + es1[2] * es1[2]);
    es2[0] =  es1[2] * snorm[1] - es1[1] * snorm[2];
    es2[1] =  es1[0] * snorm[2] - es1[2] * snorm[0];
    es2[2] =  es1[1] * snorm[0] - es1[0] * snorm[1];
    for (int i = 0 ; i < 3; i ++) { es1[i] = es1[i] / es1length;}
    double es21length = sqrt(es2[0] * es2[0] + es2[1] * es2[1] + es2[2] * es2[2]);
    for (int i = 0 ; i < 3; i ++) { es2[i] = es2[i] / es2length;}

    for (int i = 0 ; i < 3; i ++) {
      surfReparamVertList [repIndex * 3 + i] = coor1 * es1[i]/ e1length * es1length + 
  coor2 * es2[i] / e2length * es2length + vertList[secondPoint * 3 + i];
    }
  }
  std::cout << std::endl;
  delete normalList;

  return 0;
}
