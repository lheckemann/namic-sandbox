% The following mex compilation commands link off of the TEEM
% libraries included with Slicer3 releases.  All you need to have
% installed to make this work is Slicer3 and MATLAB.
%
% Contributed by John Melonakos, jmelonak@ece.gatech.edu, (2008).
%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% For Windows, do the following things:
% 1) Add the following to the PATH environment variable - C:/Program Files/Slicer3 3.2.2008-08-08/bin
% 2) Create the following environment variable - NRRD_STATE_KEYVALUEPAIRS_PROPAGATE=1
% 3) a) Uncomment the following lines, 
%    b) modify the paths to point to your Slicer3 installation, 
%    c) and run this script in MATLAB
%    d) (Be sure to comment out the Linux stuff below):
% mex nrrdLoadWithMetadata.c -I"z:/Slicer3 3.2.2008-08-08/include" -I"z:/Slicer3 3.2.2008-08-08/lib/TEEM-1.9/" 
% mex nrrdSaveWithMetadata.c -I"z:/Slicer3 3.2.2008-08-08/include" -I"z:/Slicer3 3.2.2008-08-08/lib/TEEM-1.9/" 
% mex nrrdLoad.c -I"z:/Slicer3 3.2.2008-08-08/include" -I"z:/Slicer3 3.2.2008-08-08/lib/TEEM-1.9/" 
% mex nrrdSave.c -I"z:/Slicer3 3.2.2008-08-08/include" -I"z:/Slicer3 3.2.2008-08-08/lib/TEEM-1.9/" 


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% For Linux, do the following things:
% 1) Add the following to .bashrc (changing the paths to point to your Slicer3 installation):
%      export TEEM_ARCH=linux.32
%      export LD_LIBRARY_PATH=/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/include:$LD_LIBRARY_PATH
%      export LD_LIBRARY_PATH=/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/lib/TEEM-1.9:$LD_LIBRARY_PATH
%      export LD_LIBRARY_PATH=/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/lib/vtk-5.2:$LD_LIBRARY_PATH
%      export PATH=/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/bin:$PATH
%      export NRRD_STATE_KEYVALUEPAIRS_PROPAGATE=1
% 2) a) Uncomment the following lines, 
%    b) modify the paths to point to your Slicer3 installation, 
%    c) and run this script in MATLAB 
%    d) (Be sure to comment out the Windows stuff above):
mex nrrdLoadWithMetadata.c -I/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/include -L/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/lib/TEEM-1.9/ -lair -lalan -lbane -lbiff -lcoil -ldye -lecho -lell -lgage -lhest -lhoover -llimn -lmite -lmoss -lnrrd -lpush -lten -lunrrdu -lz -lm
mex nrrdSaveWithMetadata.c -I/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/include -L/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/lib/TEEM-1.9/ -lair -lalan -lbane -lbiff -lcoil -ldye -lecho -lell -lgage -lhest -lhoover -llimn -lmite -lmoss -lnrrd -lpush -lten -lunrrdu -lz -lm
%mex nrrdLoad.c -I/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/include -L/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/lib/TEEM-1.9/ -lair -lalan -lbane -lbiff -lcoil -ldye -lecho -lell -lgage -lhest -lhoover -llimn -lmite -lmoss -lnrrd -lpush -lten -lunrrdu -lz -lm
%mex nrrdSave.c -I/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/include -L/software/Slicer3-3.3-alpha-2008-08-01-linux-x86/lib/TEEM-1.9/ -lair -lalan -lbane -lbiff -lcoil -ldye -lecho -lell -lgage -lhest -lhoover -llimn -lmite -lmoss -lnrrd -lpush -lten -lunrrdu -lz -lm
