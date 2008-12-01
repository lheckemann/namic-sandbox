function GeodesicTractographySegmentation( ...
    dwiNhdrFilePath, initialTensorsNhdrFilePath, initialMaskNhdrFilePath, ...
    roiNhdrFilePath, roiSeedVal, roiTargetVal, ...
    vtkOutputFilePath, fbOutputFilePath )
% function GeodesicTractographySegmentation( ...
%     dwiNhdrFilePath, initialTensorsNhdrFilePath, initialMaskNhdrFilePath, ...
%     roiNhdrFilePath, roiSeedVal, roiTargetVal, ...
%     vtkOutputFilePath, fbOutputFilePath )
% This is the main function for executing geodesic tractography
% segmentation.
% Input Parameters:
%   dwiNhdrFilePath is the NRRD file path of the input DW-MRI data
%   initialTensorsNhdrFilePath (optional) is the NRRD file path of the tensors
%   initialMaskNhdrFilePath is the NRRD file path of the optional mask
%   roiNhdrFilePath is the NRRD file path of the the roi data
%   roiSeedVal is the label of the seed region
%   roiTargetVal is the label of the target region
%   vtkOutputFilePath is the .vtk file of the anchor tract
%   fbOutputFilePath is the fiber bundle output file path
% Output Parameters:
%   n/a

fprintf('########################################\n');
fprintf('## GEODESIC TRACTOGRAPHY SEGMENTATION ##\n');
fprintf('########################################\n');
fprintf('* Input data requires isotropic voxels *\n\n');
tic;

% Debugging
if( 0 ),
  system( sprintf('cp %s /tmp/GTSegmentTemp/', dwiNhdrFilePath) );
  system( sprintf('cp %s /tmp/GTSegmentTemp/', initialTensorsNhdrFilePath) );
  system( sprintf('cp %s /tmp/GTSegmentTemp/', initialMaskNhdrFilePath) );
  system( sprintf('cp %s /tmp/GTSegmentTemp/', roiNhdrFilePath) );
  exit;
end

% Load in the common parameters
ioVars.dwiNhdrFilePath = dwiNhdrFilePath;
ioVars.initialTensorsNhdrFilePath = initialTensorsNhdrFilePath;
ioVars.initialMaskNhdrFilePath = initialMaskNhdrFilePath;
ioVars.outputDirPath = '/tmp/GTSegmentTemp/';
if( ~exist( ioVars.outputDirPath, 'dir' ) ),
  [success,message,messageid] = mkdir( ioVars.outputDirPath );
  if( ~success ),
    fprintf('ERROR: could not create the output directory.\n');
    keyboard;
  end
end
ioVars.outputFilenamePrefix = 'temp-cbX-';
ioVars.commonOutputFilenamePrefix = 'temp-';
ioVars.roiNhdrFilePath = roiNhdrFilePath;
ioVars.roiSeedVal = str2double( roiSeedVal );
ioVars.roiTargetVal = str2double( roiTargetVal );
ioVars.vtkOutputFilePath = vtkOutputFilePath;
ioVars.fbOutputFilePath = fbOutputFilePath;
[ioVars,processVars] = loadVars( ioVars );

% Load in the fiber specific parameters
processVars.meanFiberRadius = 7/2;
processVars.neighborhoodRadius = ceil(processVars.meanFiberRadius); % radius for the neighborhood operator
processVars.fiberType = 'cb';

% Store into the output struct
output.ioVars = ioVars;
output.processVars = processVars;

% Run the Geodesic Tractography Segmentation
t1 = toc;
output = preprocessNrrdData( output ); t2 = toc;
output = preprocessNrrdROIs( output ); t3 = toc;
output = generateLocalCostsODF( output ); t4 = toc;
output = anchorTractography( output ); t5 = toc;
output = fiberBundleSegmentation( output ); t6 = toc;
writeFiberBundleNrrdFile( output ); t7 = toc;

% Finish up
fprintf('Time (seconds) = %f, for initial setup\n',t1);
fprintf('Time (seconds) = %f, for preprocessing the data\n',t2-t1);
fprintf('Time (seconds) = %f, for preprocessing the rois\n',t3-t2);
fprintf('Time (seconds) = %f, for generating the odf\n',t4-t3);
fprintf('Time (seconds) = %f, for anchor tractography\n',t5-t4);
fprintf('Time (seconds) = %f, for fiber bundle segmentation\n',t6-t5);
fprintf('Time (seconds) = %f, for writing fiber bundle to file\n',t7-t6);
fprintf('Time (seconds) = %f, for TOTAL algorithm duration.\n',t7);

fprintf('THE END!\n');
exit;
