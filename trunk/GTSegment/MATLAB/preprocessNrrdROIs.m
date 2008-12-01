function output = preprocessNrrdROIs( output )
% function output = preprocessNrrdROIs( output )
% This function preprocesses the NRRD data.
% Input Parameters:
%   output is a struct containing all of the output information
% Output Parameters:
%   output is a struct containing all of the output information

fprintf('Preprocessing the NRRD ROIs... (preprocessNrrdROIs.m)\n');

% Crop the roi
fprintf('  -crop the roi... \n');
cropRoiNhdrFilePath = strcat( output.ioVars.outputDirPath, ...
                              output.ioVars.outputFilenamePrefix, ...
                              'roi-crop.nhdr' );
cropRoiCommand = sprintf( 'unu crop -min %d %d %d -max %d %d %d -i %s -o %s', ...
                          output.cropfun(1), output.cropfun(3), output.cropfun(5), ...
                          output.cropfun(2), output.cropfun(4), output.cropfun(6), ...
                          output.ioVars.roiNhdrFilePath, cropRoiNhdrFilePath );
fprintf( '  --command:  %s\n', cropRoiCommand );
system( cropRoiCommand );
fprintf(' DONE\n');

% Loading the cropped roi
fprintf('  -loading the roi-crop data... ');
roi = nrrdLoadWithMetadata( cropRoiNhdrFilePath );
fprintf(' DONE\n');

% Extract seedPts and targetPts from the roi
fprintf('  -extracting seedPts and targetPts from the roi... ');
seedPts = find( roi.data == output.ioVars.roiSeedVal );
targetPts = find( roi.data == output.ioVars.roiTargetVal );
fprintf(' DONE\n');

% Compute the mean fiber radius in voxels
meanFiberRadiusPixels = output.processVars.meanFiberRadius/output.voxelSpacing;

% Check for bundle specific masking
if( strcmp( output.processVars.fiberType, 'cb' ) ),
  % Select the middle point of the ROI
  [yS,xS,zS] = ind2sub( output.sizes(1:3), seedPts ); % seed
  zSMedIdx = find( zS == median(zS) );
  yS = yS(zSMedIdx); xS = xS(zSMedIdx); zS = zS(zSMedIdx);
  seedPts = sub2ind( output.sizes(1:3), yS, xS, zS );
  [yT,xT,zT] = ind2sub( output.sizes(1:3), targetPts ); % target
  zTMedIdx = find( zT == median(zT) );
  yT = yT(zTMedIdx); xT = xT(zTMedIdx); zT = zT(zTMedIdx);
  targetPts = sub2ind( output.sizes(1:3), yT, xT, zT );

  % Mask out strong left-right diffusion
  backPts = find( output.mask.data == 0 );
  tensorDiagonal = output.tensors.data([1,4,6],:,:,:);
  tensorDiagonal(1,:,:,:) = tensorDiagonal(1,:,:,:) / ...
      output.ioVars.ccThreshold;
  [tensorDiagMax,tensorDiagMaxIdx] = max(tensorDiagonal,[],1);
  ccPts = find( tensorDiagMaxIdx == 1 );
  ccPts = setdiff( ccPts, backPts );
  % Create the corpus ROI
  corpusROI = output.mask.data.*0;
  corpusROI(ccPts) = 1;
  [islands,nRegions] = bwlabeln( corpusROI, 18 );
  s = regionprops(islands, 'Area');
  area = cat(1, s.Area);
  if (length(area)),     % check to see if salient features exist
    [maxArea,nMaxRegion] = max(area);
    iBackPixels = find(islands ~= nMaxRegion);
    corpusROI(iBackPixels) = 0;
  end
  ccPts = find( corpusROI );
  [yCC,xCC,zCC] = ind2sub( output.sizes(1:3), ccPts );
  % Build fencePts from seedPts, targetPts, and ccPts
  [yS,xS,zS] = ind2sub( output.sizes(1:3), seedPts );
  [yT,xT,zT] = ind2sub( output.sizes(1:3), targetPts );
  xFenceMin = min( max(xS), max(xT) ) + 1;
  xFenceMax = max( min(xS), min(xT) ) - 1;
  corpusROIExtended = corpusROI;
  corpusROIHalf = corpusROIExtended;
  corpusROIHalf(:,1:xFenceMin+round((xFenceMax-xFenceMin)/2),:) = 0;
  corpusHalfPts = find( corpusROIHalf );
  [yCCHalf,xCCHalf,zCCHalf] = ind2sub( output.sizes(1:3), corpusHalfPts );
  %% Build fencePts laterally to the CC
  superiorDirection = sign( sum(output.dwi.spacedirections(7:9)) );
  minXCC = min(xCC(:));
  maxXCC = max(xCC(:));
  buffer = 10;
  for n = 1:length( ccPts ),
    if( (xCC(n)>minXCC+buffer) && (xCC(n)<maxXCC-buffer) ),
      if( superiorDirection == -1 ),
        if( isempty(find(corpusROIExtended(yCC(n),xCC(n),zCC(n)+1:end))) ),
          corpusROIExtended(:,xCC(n),zCC(n):zCC(n)+2) = 1;
        end
      else
        if( isempty(find(corpusROIExtended(yCC(n),xCC(n),1:zCC(n)-1))) ),
          corpusROIExtended(:,xCC(n),zCC(n)-2:zCC(n)) = 1;
        end
      end     
    end
  end
  %% Build fencePts beneath the CC
  if( superiorDirection == -1 ),
    zRange = [output.sizes(3):-1:1];
  else
    zRange = [1:output.sizes(3)];
  end
  xRange = round( mean( [xFenceMin xFenceMax] ) );
  xRange = [xRange-2:xRange+2];
  for x = xRange,
    for y = 1:output.sizes(1),
      for z = zRange,
        if( corpusROIExtended(y,x,z) == 1 ),
          break;
        else
          corpusROIExtended(y,x,z) = 1;
        end
      end
    end
  end
  % Mask out corpus and pixels under the corpus
  ccExtendedPts = find( corpusROIExtended );
  output.mask.data(ccExtendedPts) = 0;
  output.mask.data(seedPts) = 1;   % add these back in just in case they were
  output.mask.data(targetPts) = 1; % removed
end

% Mask out noisy points
fprintf('  -mask out noisy points... ');
minDWI = min( output.dwi.data, [], 4 );
noisyPts = find( minDWI < output.ioVars.noisyThreshold );
output.mask.data(noisyPts) = 0;

% Put back the seed and target points
output.mask.data(seedPts) = 1;   % add these back in just in case they were
output.mask.data(targetPts) = 1; % removed

% Save data
output.meanFiberRadiusPixels = meanFiberRadiusPixels;
output.seedPts = seedPts;
output.targetPts = targetPts;
fprintf(' DONE\n');
