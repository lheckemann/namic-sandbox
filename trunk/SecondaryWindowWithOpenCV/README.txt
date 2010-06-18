Module Name: SecondaryWindowWithOpenCV

This module is based on SecondaryWindow module by J. Tokuda (BWH): http://svn.na-mic.org/NAMICSandBox/trunk/IGTLoadableModules/SecondaryWindow/ 

(c) Copyright 2010 Atsushi Yamada and M. Komura (Fujimoto Lab., Nagoya
Institute of Technology, Japan)
CMakeLists.txt, FindOpenCV.cmake, and FindOpenIGTLink.cmake are
contributions of K. Chinzei(AIST) and T. Takeuchi (SFC Corp.).
 Acknowledgement: K. Chinzei (AIST), Y. Hayashi (Nagoya Univ.), T.
Takeuchi (SFC Corp.), J. Tokuda (BWH), N. Hata (BWH), and H. Fujimoto (NIT)

Date: 7/17/2009
Update: 4/25/2010

Documentation of SecondaryWindowWithOpenCV module

1. Install

The installation process was tested on the environment: Mac OS X 10.6.4,
OpenCV 2.1 and 3D Slicer 3.5 Alpha on MacBook Pro 13-inch (using built-in
iSight camera, 64-bit mode)

This module requires OpenCV and CMake. You need to install these open
source software in advance.
(a) OpenCV: http://sourceforge.net/projects/opencvlibrary/files/
OpenCV 2.1 is required.  
OpenCV can be installed using CMake.
(b) CMake: http://www.cmake.org/cmake/resources/software.html

We prepare the special CMakeLists.txt and FindOpenCV.cmake (written by K.
Chinzei (AIST) and T. Takeuchi (SFC Corp.)). These files can configure
necessary pathes automatically.

(c) build process
1. make SecondaryWindowWithOpenCV-build directory.
2. type 'cd SecondaryWindowWithOpenCV-build' and enter.
3. type 'ccmake ../secondaryWindowWithOpenCV' and enter.
4. press 'c' key.
5. press 'e' key and input Slicer3 path (the path of Slicer3-build). press
'c' key.
6. If 'Press [g] to generate and exit' message is appeared, press 'g' key.
7. type 'make' and enter.

(d) registration of the built module.
8. start up Slicer3 (for example, type '~/Slicer3-build/Slicer3' and
enter).
9. press F2 key (select Application Settings).
10. select Module Settings and add the module path (for example, the path
is ~/secondaryWindowWithOpenCV-build/Slicer3/Modules)
11. exit Slicer3 and start up Slicer3 again.
12. you can see that there is SecondaryWindowWithOpenCV on the module list.

2. Operation
This module has some push buttons on the left pain of Slicer3.
You can display the secondary window which has the captured video image by
clicking 'On' button. 

3. Module Local Test Operation:
1. Select 'SecondaryWinodowWithOpenCV' module.
2. Click 'On' button. Then, you can see the secondary window.
3. Select 'Transforms' module, and create transform node 'LinearTransform'.
4. Select 'Data' module, and check 'Display MRML ID's'. You can see that the MRML ID of 'Linear Transform' is 'vtkMRMLLinearTransformNode4'.
5. Select 'SecondaryWinodowWithOpenCV' module again, and click 'OBSERVE MRML' button.
6. Select 'Transform' module, and select transform node 'LinearTransForm'.
7. If you move Translation progress bars, you can see that status values overlaid the camera view changes smoothy.

