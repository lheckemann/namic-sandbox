function output = preprocessNrrdData( output );
% function output = preprocessNrrdData( output );
% This function preprocesses the NRRD data.
% Input Parameters:
%   output is a struct containing all of the output information
% Output Parameters:
%   output is a struct containing all of the output information

fprintf('Preprocessing the NRRD data... (preprocessNrrdData.m)\n');

% Parse the DWI data
%% Note:
%%  -all processing is done on isotropic voxels in the IJK space
%%  -remember to remove baseline gradient directions ([0 0 0])
%%  -assume baseline gradient directions come before other directions
%%  -no need for origin information when transforming the gradients
%%  -GTSegment requires the gradient directions to be in the 4th axis
%%  -Slicer3 stores gradient directions in the 1st axis
dwi = nrrdLoadWithMetadata( output.ioVars.dwiNhdrFilePath );
dwi.data = permute( dwi.data, [2 3 4 1] ); % permute grad dirs to 4th axis
dwi.data = double( dwi.data );
if( ~length(dwi.measurementframe) ),
  dwi.measurementframe = eye(3);
end
sizesFull = size(dwi.data); % sizes before cropping
spacedirectionsFull = dwi.spacedirections; % space directions before cropping
spaceoriginFull = dwi.spaceorigin; % space origin before cropping

%% Extract the gradient directions and the baseline image
baselineNhdrFilePath = strcat( output.ioVars.outputDirPath, ...
                               output.ioVars.commonOutputFilenamePrefix, ...
                               'baseline.nhdr' );
baseline.data = zeros( sizesFull(1:3) );
nDir = sizesFull(4);
nDirNonzero = 0;
nBaselines = 0;
for n = 1:nDir
  if( sum(abs(dwi.gradientdirections(n,:))) ~= 0 ),
    nDirNonzero = nDirNonzero + 1;
    grads(nDirNonzero,:) = inv(spacedirectionsFull) * ...
        dwi.measurementframe * dwi.gradientdirections(n,:)';
    grads(nDirNonzero,:) = grads(nDirNonzero,:)/norm(grads(nDirNonzero,:));
  else
    nBaselines = nBaselines + 1;
    baseline.data = baseline.data + dwi.data(:,:,:,n);
  end
end
baseline.data = baseline.data / nBaselines;

% Bring in the tensors
if( output.processVars.estimateTensors ),
  fprintf('  -estimating tensors from the DWI data... \n');
  tensorsNhdrFilePath = output.ioVars.tensorsNhdrFilePath;
  tendEstimCommand = sprintf( 'tend estim -B kvp -knownB0 true -i %s -o %s', ...
                              output.ioVars.dwiNhdrFilePath, tensorsNhdrFilePath );
  fprintf( '  --command:  %s\n', tendEstimCommand );
  system( tendEstimCommand );
  fprintf(' DONE\n');
else,
  fprintf('  -preparing slicer tensors for MATLAB usage... \n');
  tensorsNhdrFilePath = output.ioVars.tensorsNhdrFilePath;
  tendShrinkCommand = sprintf( 'tend shrink -i %s -o %s', ...
                               output.ioVars.initialTensorsNhdrFilePath, tensorsNhdrFilePath );
  fprintf( '  --command:  %s\n', tendShrinkCommand );
  system( tendShrinkCommand );
  tendUNMFCommand = sprintf( 'tend unmf -i %s -o %s', ...
                             tensorsNhdrFilePath, tensorsNhdrFilePath );
  fprintf( '  --command:  %s\n', tendUNMFCommand );
  system( tendUNMFCommand );
  fprintf(' DONE\n');
end

% Calculate FA from the tensors
fprintf('  -calculating FA from the tensors... \n');
faNhdrFilePath = output.ioVars.faNhdrFilePath;
calcFACommand = sprintf( 'tend anvol -t %f -a fa -i %s -o %s', ...
                         output.ioVars.confidenceThreshold, ...
                         tensorsNhdrFilePath, faNhdrFilePath );
fprintf( '  --command:  %s\n', calcFACommand );
system( calcFACommand );
fprintf(' DONE\n');

% Compute the voxel spacing
%% Assumes that there is only one element in each direction (DANGEROUS)
sdIdx = find(spacedirectionsFull);
voxelSpacing = mean(abs(spacedirectionsFull(sdIdx))); % mm/voxel
fprintf('  -voxel spacing (CHECK THIS) = %f\n', voxelSpacing);

% Generate the cropping function from the mask
%% cropfun is 0-indexed
fprintf('  -creating the cropping function... ');
mask = nrrdLoadWithMetadata( output.ioVars.initialMaskNhdrFilePath );
mask.data = double( mask.data );
maskIdx = find( mask.data );
if( length(maskIdx) == 0 ),
  fprintf('ERROR: The input mask did not detect white matter.\n');
  keyboard;
else,
  [y,x,z] = ind2sub( size(mask.data), maskIdx );
  cropfun = [min(y)-1, max(y)-1, ...
             min(x)-1, max(x)-1, ...
             min(z)-1, max(z)-1];
end
fprintf(' DONE\n');

% Crop the following: dwi, tensors, fa, mask
%%  -Assumes the DWI has gradient directions in 1st dimension
fprintf('  -crop the following: dwi, tensors, mask... \n');
cropDWINhdrFilePath = strcat( output.ioVars.outputDirPath, ...
                              output.ioVars.commonOutputFilenamePrefix, ...
                              'dwi-crop.nhdr' );
cropDWICommand = sprintf( 'unu crop -min %d %d %d %d -max M %d %d %d -i %s -o %s', ...
                          nBaselines, cropfun(1), cropfun(3), cropfun(5), ...
                          cropfun(2), cropfun(4), cropfun(6), ...
                          output.ioVars.dwiNhdrFilePath, cropDWINhdrFilePath );
fprintf( '  --command:  %s\n', cropDWICommand );
system( cropDWICommand );

cropTensorsNhdrFilePath = strcat( output.ioVars.outputDirPath, ...
                                  output.ioVars.commonOutputFilenamePrefix, ...
                                  'tensors-crop.nhdr' );
cropTensorsCommand = sprintf( 'unu crop -min 1 %d %d %d -max 6 %d %d %d -i %s -o %s', ...
                              cropfun(1), cropfun(3), cropfun(5), ...
                              cropfun(2), cropfun(4), cropfun(6), ...
                              tensorsNhdrFilePath, cropTensorsNhdrFilePath );
fprintf( '  --command:  %s\n', cropTensorsCommand );
system( cropTensorsCommand );
fprintf(' DONE\n');

cropFANhdrFilePath = strcat( output.ioVars.outputDirPath, ...
                             output.ioVars.commonOutputFilenamePrefix, ...
                             'fa-crop.nhdr' );
cropFACommand = sprintf( 'unu crop -min %d %d %d -max %d %d %d -i %s -o %s', ...
                         cropfun(1), cropfun(3), cropfun(5), ...
                         cropfun(2), cropfun(4), cropfun(6), ...
                         output.ioVars.faNhdrFilePath, cropFANhdrFilePath );
fprintf( '  --command:  %s\n', cropFACommand );
system( cropFACommand );

cropMaskNhdrFilePath = strcat( output.ioVars.outputDirPath, ...
                               output.ioVars.commonOutputFilenamePrefix, ...
                               'mask-crop.nhdr' );
cropMaskCommand = sprintf( 'unu crop -min %d %d %d -max %d %d %d -i %s -o %s', ...
                           cropfun(1), cropfun(3), cropfun(5), ...
                           cropfun(2), cropfun(4), cropfun(6), ...
                           output.ioVars.initialMaskNhdrFilePath, cropMaskNhdrFilePath );
fprintf( '  --command:  %s\n', cropMaskCommand );
system( cropMaskCommand );

cropfun1 = cropfun + 1;
baseline.data = baseline.data( ...
    cropfun1(1):cropfun1(2), ...
    cropfun1(3):cropfun1(4), ...
    cropfun1(5):cropfun1(6) );

% Loading the cropped data
fprintf('  -loading the dwi-crop data... ');
clear dwi;
dwi = nrrdLoadWithMetadata( cropDWINhdrFilePath );
dwi.data = permute( dwi.data, [2 3 4 1] ); % permute grad dirs to 4th axis
dwi.data = double( dwi.data );
sizes = size( dwi.data );
fprintf(' DONE\n');

fprintf('  -loading the tensors-crop data... ');
clear tensors;
tensors = nrrdLoadWithMetadata( cropTensorsNhdrFilePath );
tensors.data = double( tensors.data );
fprintf(' DONE\n');

fprintf('  -loading the fa-crop data... ');
clear fa;
fa = nrrdLoadWithMetadata( cropFANhdrFilePath );
fa.data = double( fa.data );
fprintf(' DONE\n');

fprintf('  -loading the mask-crop data... ');
clear mask;
mask = nrrdLoadWithMetadata( cropMaskNhdrFilePath );
mask.data = double( mask.data );
fprintf(' DONE\n');

% Save data
output.dwi = dwi;
output.tensors = tensors;
output.fa = fa;
output.mask = mask;
output.baseline = baseline;
output.sizes = sizes;
output.sizesFull = sizesFull;
output.spacedirectionsFull = spacedirectionsFull;
output.spaceoriginFull = spaceoriginFull;
output.voxelSpacing = voxelSpacing;
output.nBaselines = nBaselines;
output.grads = grads;
output.cropfun = cropfun;
output.cropDWINhdrFilePath = cropDWINhdrFilePath;
output.cropTensorsNhdrFilePath = cropTensorsNhdrFilePath;
output.cropMaskNhdrFilePath = cropMaskNhdrFilePath;
fprintf(' DONE\n');
