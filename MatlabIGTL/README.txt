--------------------------------------------------------------------------------

                          Matlab OpenIGTLink Interface


                                 Junichi Tokuda
                             tokuda@bwh.harvard.edu

             http://wiki.na-mic.org/Wiki/index.php/OpenIGTLink/Matlab

--------------------------------------------------------------------------------


Please refer the web page for up-to-date information.

 +----------------------------+
 | Contents                   |
 |                            |
 | 1. Introduction            |
 | 2. Build/Install           |
 | 3. How does it work?       |
 |                            |
 +----------------------------+


================
1. Introduction
================

 The objective of this project is to provide OpenIGTLink interface for Matlab /
 Octave to support research and development in image guided therapy (IGT).
 Matlab and Octave are widely used for prototyping image and signal processing
 algorithms. They also offers many powerful function set to handle matrix and
 coordinate data, which is useful to test and analyze coordinate data exported
 from tracking and robotic devices. The OpenIGTLink interface for Matlab / Octave
 allows importing and exporting several types of data that can be handled in
 the OpenIGTLink protocol in Matlab / Octave environment. It provides a rapid
 prototyping environment, which many researchers and engineers are already
 familiar with.
 
 This project is a generalization of Slicer Matlab Pipeline project in the 2007
 NA-MIC Project Week. 



=================
2. Build/Install
=================

2.1 Install the OpenIGTLink Library

 The instruction can be found at
   http://wiki.na-mic.org/Wiki/index.php/OpenIGTLink/Library
 
 
2.2 Get the Matlab OpenIGTLink interface source code

 The OpenIGTLink/Matlab interface is in the initial stage of development.
 The source code is available from NA-MIC SandBox repository at:
 
  http://svn.na-mic.org/NAMICSandBox/trunk/BRPTools/MatlabIGTL
 

2.3 Build MEX files

 You need to have a MEX compiler or octave to the build MEX binaries. To build
 the binaries, you may need to edit the Makefile.

 First substitute the path to the OpenIGTLink Library source and binary
 directories installed in your system:

   ### OpenIGTLink Library
   IGTLSRC= /projects/igtdev/tokuda/igtl/OpenIGTLink
   IGTLBLD= /projects/igtdev/tokuda/igtl/OpenIGTLink-build


 If you use Maltab, specify the full path to the MEX compiler. You don't need to
 have any options to the MEX compiler.

   MEX    = /local/os-exact/pkg/Matlab71-64/bin/mex
   MEXOPT = 

 If you use Octave, specify the full path to the mkoctfile program and "--mex"
 option.

   MEX    = /Applications/Octave.app/Contents/Resources/bin/mkoctfile
   MEXOPT = --mex

 Now it's ready to build your MEX file. Run make. If the MEX files are
 successfully built, you could find:

   igtlclose.mex<arc-name>
   igtlopen.mex<arc-name>
   igtlsend.mex<arc-name>

 Note that <arc-name> is the architecture name of your system. 


=====================
3. How does it work?
=====================

 The OpenIGTLink Matlab interface is implemented as a set of MEX Files, which
 are C/C++ source codes called from Matlab. Those MEX files simply receives data
 from Matlab, connect to OpenIGTLink receiver, serialize the data in appropriate
 format using the OpenIGTLink Library, and send it to the receiver.
 
 The usage of the interface is quite simple. The following example Matlab code
 is sending trancking data to the receiver waiting at port #18944 in the
 localhost.

   %%% affine transform matrix
   M = [1.0, 0.0, 0.0, 0.0;
        0.0,-1.0, 0.0, 0.0;
        0.0, 0.0, 1.0, 0.0;
        0.0, 0.0, 0.0, 1.0];
   
   IMGDATA.Type = 'TRANSFORM';
   IMGDATA.Name = 'MatlabTrans';
   IMGDATA.Trans = M;
   
   sd = igtlopen('localhost', 18944);
   r = igtlsend(sd, IMGDATA);
   igtlclose(sd);
  
  
 For tracking data transfer:
  
   %%% read image data
   fid = fopen('igtlTestImage1.raw', 'r');
   I = fread(fid, [256 256], 'uint8')';
   fclose(fid);
   
   %%% affine transform matrix
   M = [1.0, 0.0, 0.0, 0.0;
        0.0,-1.0, 0.0, 0.0;
        0.0, 0.0, 1.0, 0.0;
        0.0, 0.0, 0.0, 1.0];
   
   IMGDATA.Type = 'IMAGE';
   IMGDATA.Name = 'MatlabImage';
   IMGDATA.Image = I;
   IMGDATA.Trans = M;
   
   %%% send the image data through OpenIGTLink connection
   sd = igtlopen('localhost', 18944);
   r = igtlsend(sd, IMGDATA);
   igtlclose(sd);

