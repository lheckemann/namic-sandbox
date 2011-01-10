#include "DefRegEval.h"

#include "itkUnaryFunctorImageFilter.h" 
#include "itkShiftScaleImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkSubtractImageFilter.h"

#include "vtkOutputWindow.h" 
#include "vtkSmartPointer.h"
#include "vtkXMLUnstructuredGridWriter.h" 
#include "vtkUnstructuredGridWriter.h"
#include "IO/vtkFEBioWriter.h" 
#include "vtkMath.h"
#include "vtkCharArray.h"
#include "vtkDoubleArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkIdList.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkAppendPolyData.h"
#include "vtkSTLReader.h"
#include "IO/vtkNetgenMeshReader.h"
#include "IO/vtkMeditMeshReader.h"

typedef itk::Image <unsigned char, 3> OutputImageType;


// squared distance between a line and a point (use sqrt to get actual distance)
static double PointDistance2FromLine(double point[3], double L[3], double v[3])
{
  double u;
  double Intersection[3];

  u = ( ( ( point[0] - L[0] ) * v[0] ) +
    ( ( point[1] - L[1] ) * v[1] ) +
    ( ( point[2] - L[2] ) * v[2] ) ) ;

  Intersection[0] = L[0] + u * v[0];
  Intersection[1] = L[1] + u * v[1];
  Intersection[2] = L[2] + u * v[2];

  return vtkMath::Distance2BetweenPoints(point, Intersection);
}

void AddUniqueTriangle(vtkIdList *trianglePointIds,int pt0, int pt1, int pt2)
{
  int numberOfTriangles=trianglePointIds->GetNumberOfIds()/3;
  int curId=0;
  for (int i=0; i<numberOfTriangles; i++)
  {
    if (trianglePointIds->GetId(curId++)!=pt0)
    {
      // skip remaining 2 ids
      curId+=2;
      continue;
    }
    if (trianglePointIds->GetId(curId++)!=pt1)
    {
      // skip remaining id
      curId++;
      continue;
    }
    if (trianglePointIds->GetId(curId++)==pt2)
    {
      // triangle already in the list
      return;
    }
  }
  trianglePointIds->InsertNextId(pt0);
  trianglePointIds->InsertNextId(pt1);
  trianglePointIds->InsertNextId(pt2);
}

// Finds all surface triangles in a sphere
void AddSurfaceTrianglePointIdsInSphere(vtkIdList *trianglePointIds, vtkUnstructuredGrid* ugrid, double* boundingSphereCenter, double boundingSphereRadius)
{
  double radius2=boundingSphereRadius*boundingSphereRadius;

  vtkDoubleArray* pointCoords=vtkDoubleArray::SafeDownCast(ugrid->GetPoints()->GetData());
  vtkCharArray* onSurfaceArray=vtkCharArray::SafeDownCast(ugrid->GetPointData()->GetArray(DefRegEvalGlobal::ArrayNameOnSurface));

  // Extract all the triangles from the polyhedra elements
  //ugrid->Update();
  vtkPoints *points = ugrid->GetPoints();
  vtkCellArray *cells = ugrid->GetCells();
  cells->InitTraversal();  
  vtkIdType npts, *pts;
  double coord[3]={0,0,0};
  bool surfPoint[4]={false,false,false};
  while (cells->GetNextCell(npts, pts))
  {
    if (npts != 4)
    {
      // only extract tetrahedra
      continue;
    }
    // Ordering: counter-clockwise (http://www.vtk.org/VTK/img/file-formats.pdf)
    
    for (int i=0; i<4; i++)
    {
      surfPoint[i]=false;
      if (onSurfaceArray->GetValue(pts[i])!=0)
      {
        pointCoords->GetTupleValue(pts[i], coord);
        if (vtkMath::Distance2BetweenPoints(coord, boundingSphereCenter)<radius2)
        {
          surfPoint[i]=true;
        }
      }
    }
      
    if (surfPoint[0])
    {
      if (surfPoint[1])
      {
        if (surfPoint[2])
        {
          AddUniqueTriangle(trianglePointIds,pts[0],pts[2],pts[1]);
        }
        if (surfPoint[3])
        {
          AddUniqueTriangle(trianglePointIds,pts[0],pts[1],pts[3]);
        }
      }
      if (surfPoint[2])
      {
        if (surfPoint[3])
        {
          AddUniqueTriangle(trianglePointIds,pts[0],pts[3],pts[2]);
        }
      }
    }
    if (surfPoint[1])
    {
      if (surfPoint[2])
      {
        if (surfPoint[3])
        {
          AddUniqueTriangle(trianglePointIds,pts[1],pts[2],pts[3]);
        }
      }
    }

  }

}

// Finds all points triangles in a sphere (optionally retrieve only surface points)
bool AddPointIdsInSphere(vtkIdList* pointIds, vtkUnstructuredGrid* ugrid, double* centerPosition, double radius, bool allowNonSurface=false)
{
  if (pointIds==NULL)
  {
    return false;
  }  
  if (ugrid==NULL)
  {
    return false;
  }
  if (ugrid->GetPoints()==NULL)
  {
    return false;
  }
  if (ugrid->GetPoints()->GetData()==NULL)
  {
    return false;
  }
  vtkDoubleArray* pointCoords=vtkDoubleArray::SafeDownCast(ugrid->GetPoints()->GetData());
  if (ugrid->GetPoints()->GetData()==NULL)
  {
    return false;
  }
  if (ugrid->GetPointData()==NULL)
  {
    return false;
  }
  vtkCharArray* onSurfaceArray=vtkCharArray::SafeDownCast(ugrid->GetPointData()->GetArray(DefRegEvalGlobal::ArrayNameOnSurface));
  if (onSurfaceArray==NULL)
  {
    return false;
  } 
  int numberOfComponents=pointCoords->GetNumberOfComponents();
  if (numberOfComponents!=ImageDimension)
  {
    return false;
  }

  double radius2=radius*radius;
  int numberOfPoints=pointCoords->GetNumberOfTuples(); 
  double coord[3]={0,0,0};
  for (int p=0; p<numberOfPoints; p++)
  {
    if (onSurfaceArray->GetValue(p)!=0 || allowNonSurface)
    {
      // it's on the surface
      pointCoords->GetTupleValue(p, coord);
      if (vtkMath::Distance2BetweenPoints(coord, centerPosition)<radius2)
      {
        // it's within the specified radius
        pointIds->InsertUniqueId(p);
      }
    }
  }
  return true;
}

static const char* ARRAY_NAME_MATERIAL="material";

int main(int argc, char *argv[])
{
  VTK_LOG_TO_CONSOLE;

  std::string inputObjectFilename = "object-volumemesh.vol";
  std::string inputSupportFilename = "support-volumemesh.vol";
  std::string outputFilename = "femmodel.feb";

  vtkstd::vector<double> supportFixedPosition; // {67, 1, 36} LPS
  double supportFixedRadius=12; // 12 mm
  vtkstd::vector<double> probePosition1; // {67, 95, 10} LPS
  vtkstd::vector<double> probePosition2; // {67, 95, 45} LPS
  double probeRadius=8; // 8 mm

  vtkstd::vector<double> probeForce; // {-5..+5, -18, 8}

  int solverTimeSteps=10;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--CombinedVolMeshInputFn", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputObjectFilename, "Input volume mesh of the object in netgen vol format");
  args.AddArgument("--FemModelOutputFn", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFilename, "Output FEM model in FEBio format");

  args.AddArgument("--SupportFixedPosition", vtksys::CommandLineArguments::MULTI_ARGUMENT, &supportFixedPosition, "Center of the sphere that defines fixed points of the support material"); 
  args.AddArgument("--SupportFixedRadius", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &supportFixedRadius, "Radius of the sphere that defines fixed points of the support material"); 

  args.AddArgument("--ProbePosition1", vtksys::CommandLineArguments::MULTI_ARGUMENT, &probePosition1, "Position of the probe contact center 1"); 
  args.AddArgument("--ProbePosition2", vtksys::CommandLineArguments::MULTI_ARGUMENT, &probePosition2, "Position of the probe contact center 2"); 
  args.AddArgument("--ProbeRadius", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &probeRadius, "Radius of the probe contact"); 
  args.AddArgument("--ProbeForce", vtksys::CommandLineArguments::MULTI_ARGUMENT, &probeForce, "Probe force vector"); 

  args.AddArgument("--SolverTimeSteps", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &solverTimeSteps, "Number of timesteps for the solver"); 

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (supportFixedPosition.size()!=3)
  {
    std::cerr << "Invalid supportFixedPosition" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (probePosition1.size()!=3)
  {
    std::cerr << "Invalid probePosition1" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (probePosition2.size()!=3)
  {
    std::cerr << "Invalid probePosition2" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (probeForce.size()!=3)
  {
    std::cerr << "Invalid probeForce" << std::endl;
    exit(EXIT_FAILURE);
  } 

  /////////////

  vtkSmartPointer<vtkMeditMeshReader> combinedMeshReader=vtkSmartPointer<vtkMeditMeshReader>::New();
  combinedMeshReader->SetFileName(inputObjectFilename.c_str());   
  combinedMeshReader->Update();  

/*
vtkSmartPointer<vtkXMLUnstructuredGridWriter> xmlGridWriter=vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
xmlGridWriter->SetInput(organMeshReader->GetOutput());  
xmlGridWriter->SetFileName(outputFilename.c_str());    
xmlGridWriter->Update(); 
*/

  vtkSmartPointer<vtkFEBioWriter> febioWriter=vtkSmartPointer<vtkFEBioWriter>::New();
  febioWriter->SetInputConnection(combinedMeshReader->GetOutputPort());
  febioWriter->SetFileName(outputFilename.c_str());
  
  // Add support fixation boundary condition
  vtkSmartPointer<vtkIdList> supportFixedPointIds=vtkSmartPointer<vtkIdList>::New();
  AddPointIdsInSphere(supportFixedPointIds, combinedMeshReader->GetOutput(), &(supportFixedPosition[0]), supportFixedRadius, true);
  febioWriter->AddPointFixBoundaryCondition(supportFixedPointIds, "xyz");
  
  // Collect the points using several spheres instead of one cylinder
  // (somewhat slower but gives the same result and we don't have to implement AddSurfacePointIds for cylinder)
  vtkSmartPointer<vtkIdList> probeContactPointIds=vtkSmartPointer<vtkIdList>::New();
  for (double weight=0; weight<=1.0; weight+=0.05)
  {
    double center[3];
    center[0]=probePosition1[0]+(probePosition2[0]-probePosition1[0])*weight;
    center[1]=probePosition1[1]+(probePosition2[1]-probePosition1[1])*weight;
    center[2]=probePosition1[2]+(probePosition2[2]-probePosition1[2])*weight;
    AddPointIdsInSphere(probeContactPointIds, combinedMeshReader->GetOutput(), center, probeRadius, true);
  }

  if (probeForce[0]!=0)
  {
    febioWriter->AddPointForceBoundaryCondition(probeContactPointIds, "x", probeForce[0]);
  }
  if (probeForce[1]!=0)
  {
    febioWriter->AddPointForceBoundaryCondition(probeContactPointIds, "y", probeForce[1]);
  }
  if (probeForce[2]!=0)
  {
    febioWriter->AddPointForceBoundaryCondition(probeContactPointIds, "z", probeForce[2]);
  }

  febioWriter->SetNumberOfTimeSteps(solverTimeSteps);

  febioWriter->Update();

  return EXIT_SUCCESS;
}
