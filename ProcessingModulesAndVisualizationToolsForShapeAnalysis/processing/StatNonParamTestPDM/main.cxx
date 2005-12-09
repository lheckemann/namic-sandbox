#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cmath>

#include <itkMeshSpatialObject.h>
#include <itkMesh.h>
#include <itkSpatialObjectWriter.h>
#include <itkSpatialObjectReader.h>
#include <itkTriangleCell.h>
#include <itkDefaultDynamicMeshTraits.h>
#include <itkProcessObject.h>

#include "computeStatPDM.h"
#include "argio.h"

using namespace std;


void
load_MeshList_file(char * filename, int &numSubjects, int &numFeatures,
           int * &groupLabel, double * &featureValue, char * outbase) 
{
  const int MAXLINE  = 5000; 
  static char line [ MAXLINE ];
  
  #define dimension 3

  typedef itk::DefaultDynamicMeshTraits< float , dimension, dimension, double > MeshTrait;
  typedef itk::Mesh<float,dimension,MeshTrait> MeshType;

  /** Hold on to the type information specified by the template parameters. */
  typedef  MeshType::Pointer              MeshPointer;
  typedef  MeshTrait::PointType           MeshPointType;
  typedef  MeshTrait::PixelType           MeshPixelType; 
  typedef  MeshType::Pointer              MeshPointer;
  typedef  MeshType::CellTraits           CellTraits;
  typedef  MeshType::PointsContainerPointer PointsContainerPointer;
  typedef  MeshType::PointsContainer      PointsContainer;
  typedef  MeshType::CellsContainerPointer CellsContainerPointer;
  typedef  MeshType::CellsContainer       CellsContainer;
  typedef  MeshType::PointType            PointType;
  typedef  MeshType::CellType             CellType;
  typedef  itk::TriangleCell<CellType>   TriangleType;
   
  typedef itk::MeshSpatialObject<MeshType> MeshSpatialObjectType;
  typedef itk::SpatialObjectWriter<3,float,MeshTrait> MeshWriterType;
  typedef itk::SpatialObjectReader<3,float,MeshTrait> MeshReaderType;

  ifstream datafile(filename,ios::in); 
  if (!datafile.is_open()) {
    cerr << "file does not exist" << filename << endl;
    exit(0);
  }

  numSubjects = 0;

  datafile.clear(); 
  datafile.getline(line,MAXLINE);
  while (!datafile.eof()){
    char filename[MAXLINE];
    int id;
    double scale;
    int retval = sscanf(line," %d %lf %s ", &id , &scale, filename);
    if (retval == 3) {
      numSubjects++;
    }
    datafile.getline(line,MAXLINE);
  }

  std::cout << "Num Subjects: " << numSubjects << std::endl;

  groupLabel = new int [ numSubjects ];
  double * scaleFactor = new double [numSubjects];
  std::string * meshFileName = new std::string [numSubjects];
  
  // read the list
  int curLine = 0;
  datafile.clear();
  datafile.seekg(0, std::ios::beg);
  datafile.getline(line,MAXLINE);
  while (!datafile.eof()){
    char filename[MAXLINE];
    int retval = sscanf(line," %d  %lf %s ", &(groupLabel[curLine]) , &(scaleFactor[curLine]), filename);
    if (retval == 3) {
      meshFileName[curLine] = std::string(filename);
      std::cout << groupLabel[curLine] << "," << scaleFactor[curLine] << "," << filename << std::endl;
      curLine++;
    }
    datafile.getline(line,MAXLINE);
  }
  datafile.close();

  // Read the meshes
  MeshType::Pointer surfaceMesh = MeshType::New();
  MeshSpatialObjectType::Pointer  SOMesh;
  MeshReaderType::Pointer reader = MeshReaderType::New();
  PointsContainerPointer points;
  for (int index = 0; index < numSubjects; index++) {
    try
      {
       reader->SetFileName(meshFileName[index].c_str());
       reader->Update();
       MeshReaderType::SceneType::Pointer scene = reader->GetScene();  
       MeshReaderType::SceneType::ObjectListType * objList =  scene->GetObjects(1,NULL);
    
       MeshReaderType::SceneType::ObjectListType::iterator it = objList->begin();
       itk::SpatialObject<3> * curObj = *it;
       SOMesh = dynamic_cast<MeshSpatialObjectType*> (curObj);
       surfaceMesh = SOMesh->GetMesh();
       points = surfaceMesh->GetPoints();

       if (index == 0) {
         numFeatures = points->Size() * 3;
         featureValue = new double [ numSubjects * numFeatures ];
       }
    
       for (unsigned int pointID = 0; pointID < points->Size();pointID++) {
         PointType curPoint =  points->GetElement(pointID);
         for (unsigned int dim = 0; dim < 3; dim++) {
           featureValue[index*numFeatures + (pointID * 3) + dim] = curPoint[dim] / scaleFactor[index];
         }
       }
      }
    catch(itk::ExceptionObject ex)
      {
       std::cout<<ex.GetDescription()<<std::endl;
           exit(-1);
    }
  }

  // change labels to the predefined ones, this will fail if there are more than 2 labels in the file
  int preLabelA = groupLabel[0];
  int preLabelB = groupLabel[0];
  int i;
  for (i = 0; i < numSubjects; i++) {
    if (preLabelA != groupLabel[i]) {
      if (preLabelB != preLabelA && preLabelB  != groupLabel[i]) {
          cout << "Error more than 2 labels in file" << endl;
      } else {
          preLabelB = groupLabel[i];
      }
    }
  }
  for (i = 0; i < numSubjects; i++) {
    if (preLabelA == groupLabel[i]) {
      groupLabel[i] = GROUP_A_LABEL ;
    } else if (preLabelB == groupLabel[i]) {
      groupLabel[i] = GROUP_B_LABEL ;
    }
  }
  cout << "data has been relabeled: " <<  preLabelA << " --> " << GROUP_A_LABEL
       << " ; " << preLabelB << " --> " << GROUP_B_LABEL << endl;

  // compute and save averages
  static double * meanA = new double [numFeatures];
  static double * meanB = new double [numFeatures];
  int numSubjA = 0;
  int numSubjB = 0;
  for (int feat = 0; feat < numFeatures; feat++) {
    meanA[feat] = 0;
    meanB[feat] = 0;
  }
  for (int subj = 0; subj < numSubjects; subj++) {
    int subjIndex = subj * numFeatures;
    if (groupLabel[subj] == GROUP_A_LABEL) {
      numSubjA++;
      for (int feat = 0; feat < numFeatures; feat++) {
    meanA[feat] = meanA[feat] + featureValue[subjIndex + feat];
      }
    } else if (groupLabel[subj] == GROUP_B_LABEL) {
      numSubjB++;
      for (int feat = 0; feat < numFeatures; feat++) {
    meanB[feat] = meanB[feat] + featureValue[subjIndex + feat];
      }
    } else {
      std::cerr << " group label " << groupLabel[subj] << " does not exist" << std::endl;
    }
  }
  for (int feat = 0; feat < numFeatures; feat++) {
    meanA[feat] = meanA[feat] / numSubjA;
    meanB[feat] = meanB[feat] / numSubjB;
  }
  // adapt the last loaded mesh
  surfaceMesh = SOMesh->GetMesh();
  PointsContainerPointer pointsA = PointsContainer::New();
  PointsContainerPointer pointsB = PointsContainer::New();
    
  PointType curPoint;
  for (unsigned int i = 0; i < points->Size(); i++) {
    double vert[3];
    vert[0] = meanA[i*3 + 0]; vert[1] = meanA[i*3 + 1]; vert[2] = meanA[i*3 + 2];
    pointsA->InsertElement(i, PointType(vert));
    vert[0] = meanB[i*3 + 0]; vert[1] = meanB[i*3 + 1]; vert[2] = meanB[i*3 + 2];
    pointsB->InsertElement(i, PointType(vert));
  }
  surfaceMesh->SetPoints(pointsA); 
  SOMesh->SetMesh(surfaceMesh);
  MeshWriterType::Pointer writer = MeshWriterType::New();
  writer->SetInput(SOMesh);
  string FilenameA(outbase);
  FilenameA = FilenameA + string("_meanA.meta");
  writer->SetFileName(FilenameA.c_str());
  writer->Update();
  surfaceMesh->SetPoints(pointsB); 
  SOMesh->SetMesh(surfaceMesh);
  writer->SetInput(SOMesh);
  string FilenameB(outbase);
  FilenameB = FilenameB + string("_meanB.meta");
  writer->SetFileName(FilenameB.c_str());
  writer->Update();
}

void
load_simpleStat_file(char * filename, int featSelStart, 
           int featSelLen, int featgroupID,
           int &numSubjects, int &numFeatures,
           int * &groupLabel, double * &featureValue) 
{
  const int MAXLINE  = 10 * 50000;  // maximal 50'000 entries each 10 digits long
  static char line [ MAXLINE ];
  //int subjIDindex = 0;
  //int groupNameindex = 1;
  int groupIDindex = 2;
  int firstDataindex = 3;
  int endDataindex = -1;
  int featureLength = -1;

  if (featgroupID >= 0 ) groupIDindex = featgroupID;
  if (featSelStart >= 0 ) firstDataindex = featSelStart;
  if (featSelLen >= 0 ) featureLength = featSelLen;

  ifstream datafile(filename,ios::in); 
  if (!datafile.is_open()) {
    cerr << "file does not exist" << filename << endl;
    exit(0);
  }

  datafile.getline(line,MAXLINE);
  if (datafile.eof()){
    cerr << "not a single line in file " << filename << endl;
    exit(0);
  }
  
  int cnt = 0;
  numFeatures = 0;
  while (line[cnt] != '\0') {
    // skip delimiters
    while  (line[cnt] != '\0' && 
         ( line[cnt] == ' ' || line[cnt] == ',' || line[cnt] == ';' || 
           line[cnt] == '\t')) {
      cnt++;
    }
    if (line[cnt] != '\0') numFeatures++;
    // skip non-delimiters
    while  (line[cnt] != '\0' && 
         line[cnt] != ' ' && line[cnt] != ',' && line[cnt] != ';' && 
         line[cnt] != '\t') {
      cnt++;
    }
  }
  
  if (featureLength > 0 ) numFeatures = featureLength;
  else numFeatures = numFeatures - firstDataindex;

  endDataindex = firstDataindex + numFeatures - 1;
  cout << "Num Features: " << numFeatures << " from  " << firstDataindex << " to " << endDataindex << endl;

  numSubjects = 0;

  datafile.clear(); 
  datafile.seekg(0, std::ios::beg);
  datafile.getline(line,MAXLINE);
  while (!datafile.eof()){
    numSubjects++;
    datafile.getline(line,MAXLINE);
  }

  cout << "Num Subjects: " << numSubjects << endl;

  groupLabel = new int [ numSubjects ];
  featureValue = new double [ numSubjects * numFeatures ];

  int curLine = 0;
  datafile.clear();
  datafile.seekg(0, std::ios::beg);
  datafile.getline(line,MAXLINE);
  while (!datafile.eof()){
    // scan features per line
    int cnt = 0;
    int curFeature = 0;
    while (line[cnt] != '\0' && curFeature <= endDataindex) {
      // skip delimiters
      while  (line[cnt] != '\0' && 
           ( line[cnt] == ' ' || line[cnt] == ',' || line[cnt] == ';' || 
          line[cnt] == '\t')) {
        cnt++;
      }
      // read in feature
      bool numberRead = false;
      int numChar = 0;
      if (line[cnt] != '\0') {
        if (curFeature == groupIDindex) {
          //scan in groupID
          int label = -1;
          int numRead = sscanf(&(line[cnt]),"%d%n",&label, &numChar);
          if (!numRead || !numChar) { cout << "Error while reading " << endl;}
          groupLabel[curLine] = label;
                        numberRead = true;
        } else if (curFeature >= firstDataindex && curFeature <= endDataindex) {
          //scan in feature
          double feature = -1;
          int numRead = sscanf(&(line[cnt]),"%lf%n",&feature, &numChar);
          if (!numRead || !numChar) { cout << "Error while reading " << endl;}
          int featIndex = curFeature - firstDataindex;
          featureValue[curLine*numFeatures + featIndex] = feature;
                        numberRead = true;
        }
                      cnt = cnt + numChar;
        curFeature++;
      } 
      // skip non-delimiters
      while  (line[cnt] != '\0' && 
           line[cnt] != ' ' && line[cnt] != ',' && line[cnt] != ';' && 
           line[cnt] != '\t') {
                      if (numberRead) { 
                        // we were reading a number and now there are just adjacent to this number other characters -> error
                        // when reading number, then the next character needs to be a delimiter
          cout << "Error while reading number, encountering non-number on line " << curLine 
                             << " feature: " << curFeature << " numChar : " << numChar << endl;
                      }
        cnt++;
      }
    }
    if (curFeature <= endDataindex) { cout << "Not enough features" << endl;}
    // next line
    curLine++;
    datafile.getline(line,MAXLINE);
  }

  datafile.close();

  // change labels to the predefined ones, this will fail if there are more than 2 labels in the file
  int preLabelA = groupLabel[0];
  int preLabelB = groupLabel[0];
  int i;
  for (i = 0; i < numSubjects; i++) {
    if (preLabelA != groupLabel[i]) {
      if (preLabelB != preLabelA && preLabelB  != groupLabel[i]) {
          cout << "Error more than 2 labels in file" << endl;
      } else {
                      preLabelB = groupLabel[i];
      }
    }
  }
  for (i = 0; i < numSubjects; i++) {
    if (preLabelA == groupLabel[i]) {
      groupLabel[i] = GROUP_A_LABEL ;
    } else if (preLabelB == groupLabel[i]) {
      groupLabel[i] = GROUP_B_LABEL ;
    }
  }
  cout << "data has been relabeled: " <<  preLabelA << " --> " << GROUP_A_LABEL
       << " ; " << preLabelB << " --> " << GROUP_B_LABEL << endl;

}


int main (int argc, const char ** argv) 
{
  if (argc <=1 || ipExistsArgument(argv, "-usage")) {
    cout << argv[0] << endl;
    cout << " computing  local statistics corrected for multiple comparisons via non-parametric permutations tests" << endl;
    cout << "  the metric for univariate data is mean difference and for multivariate data Hotelling T^2" << endl;
    cout << "usage:  infile [options] "<< endl;
    cout << endl;
    cout << "infile        input file " << endl;
    cout << "-surfList     if present, then the input file does not contain the data, but rather is a list of" << endl;
    cout << "              lines each with an itk readable mesh file preceded by the group identifier and an inverse" << endl;
    cout << "              scaling factor (separator = space), e.g. id f bla.mesh -> points in bla.mesh will be scaled by 1/f" << endl;
    cout << "-out <base>   output base name, if omitted then infile is used " << endl << endl;
    cout << "-featgroupID  column number of group id" << endl;
    cout << "-featSelStart column number for first feature" << endl;
    cout << "-featSelLen   number of consecutive columns with features" << endl;
    cout << "              indexes for featSelStart and -featgroupID begn at 0" << endl << endl;
    cout << "-log          perform log(1+p) transform necessary for magnitude data (all features are transformed)" << endl;
    cout << "-numPerms <i> number of permutations" << endl;
    cout << "-signLevel <f> maximum level of significance,choose 1 if all levels should be computed" << endl;
    cout << "-signSteps <i> number of steps from 0..signLevel" << endl;
    cout << "-3DvectorData  3D statistics with Hotelling T^2 "<< endl;
    cout << "-2DvectorData  2D statistics with Hotelling T^2 "<< endl;
    cout << endl << endl;
    cout << " the uncorrrected p-values will be written out as well as the corrected p-vals" << endl;
    cout << endl << endl;
    exit(0);
  }

  char *infile     = strdup(argv[1]);
  char *outbase    = ipGetStringArgument(argv, "-out", NULL);
  if (!infile) {
    cout << " no input file specified " << endl;exit(1);
    }
  if (!outbase) {
    outbase = strdup(infile);
  } 

  const int numPerms = ipGetIntArgument(argv, "-numPerms", 10000);
  const double significanceLevel = ipGetDoubleArgument(argv, "-signLevel", 0.05);
  const int significanceSteps = ipGetIntArgument(argv, "-signSteps", 100);

  const int featSelStart  = ipGetIntArgument(argv, "-featSelStart", -1);
  const int featSelLen  = ipGetIntArgument(argv, "-featSelLen", -1);
  const int featgroupID  = ipGetIntArgument(argv, "-featgroupID", -1);

  const int logOn = ipExistsArgument(argv, "-log");

  int numSubjects, numFeatures;
  int * groupLabel = NULL;
  double * featureValue = NULL; 

  bool surfListInput = ipExistsArgument(argv,"-surfList");
  bool vector2DDataFlag = ipExistsArgument(argv, "-2DvectorData");
  bool vector3DDataFlag = ipExistsArgument(argv, "-3DvectorData");

  if (!surfListInput) {
    load_simpleStat_file(infile,featSelStart,featSelLen,featgroupID,numSubjects, 
             numFeatures,groupLabel,featureValue);
  } else {
    // load the surfListInput
    vector3DDataFlag = true;
    load_MeshList_file(infile,numSubjects,numFeatures,groupLabel,featureValue, outbase);
  }

  if (logOn) {
    // log transform the Feature data
    for (int subj = 0; subj < numSubjects; subj++) {
      for (int feat = 0; feat < numFeatures; feat++) {
    int subjIndex = subj * numFeatures;
    if (featureValue[subjIndex + feat] < 0) {
      cerr << "Features are negative, and thus do not represent magnitude data" << endl;
      cerr << "Cowardly refusing to log transform data... " << endl;
      exit(-1);
    }
    featureValue[subjIndex + feat] = log (1 + featureValue[subjIndex + feat]);
      }
    }
  }

  int tupel_size = 1;
  if (vector3DDataFlag) {
    tupel_size = 3;
  } else  if (vector2DDataFlag) {
    tupel_size = 2;
  } else {
    tupel_size = 1;
  }

  double * pValue = new double [numFeatures];
  
  doTesting(numSubjects, numFeatures, numPerms, tupel_size, groupLabel, featureValue,
        significanceLevel, significanceSteps,pValue, outbase);

  return 0; 
}
