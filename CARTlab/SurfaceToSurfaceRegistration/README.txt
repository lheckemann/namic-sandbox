The code in this directory could be used for performing 
surface to surface registration.

As an intermediate application, we compute the distance
from points from one surfaces to faces on the other 
surface.

Thing to do:

1) Write VTK code for reading surfaces from OBJ files
   and write as VTK legacy polydata.  This is done with
   the vtkPolyDataWriter class

1) OR: use the itk::RegularSphereMeshSource<> class to generate
   a surface from an icosahedron, and subsequent subdivision.
   See helper class itkMeshGeneratorHelper in the Testing directory.

2) Modify the code in Testing/TriangleMeshToSimplexMeshFilterTest3.cxx
   to read a VTK surface and write out the Simplex mesh in a VTK surface
   using the itkVTKPolyDataReader and itkVTKPolyDataWriter.

3) Modify the code in Testing/SurfaceToSurfaceRegistrationTest1.cxx
   to run only one evaluation of the Metric, (which will be the 
   sum of squared distance values between points of two meshes

4) Feed example SurfaceToSurfaceRegistrationTest1 with 
   SurfaceA  and SurfaceB(converted to simplex mesh).

5) Probe points from surfaceA on the distance map of points 
   form the SimplexMeshB.


