function output = fiberBundleSegmentation( output )
% function output = fiberBundleSegmentation( output )
% This function executes the flood-fill function for fiber bundle
% segmentation.
% Input Parameters:
%   output is a struct containing all of the output information
% Output Parameters:
%   output is a struct containing all of the output information

fprintf('Executing the fiber bundle segmentation... (fiberBundleSegmentation.m)\n');

% Preprocessing
nSmoothingIterations = 5;

% Initialize the statistics
output.processVars.meanFiberRadiusPixels = output.meanFiberRadiusPixels;
[likelihoods,priors,posteriors,heap] = initializeStats( ...
    output.mask.data, output.anchorTract, output.processVars );
terms.eLFApii = zeros( output.sizes(1), output.sizes(2), output.sizes(3) );
terms.eLMEpii = zeros( output.sizes(1), output.sizes(2), output.sizes(3) );
terms.eLFAoi = zeros( output.sizes(1), output.sizes(2), output.sizes(3) );
terms.eLMEoi = zeros( output.sizes(1), output.sizes(2), output.sizes(3) );

% Execute the flood-fill
%% Setup the loop
seedPts = find( heap==1 );
tractIndices{1} = seedPts';
i = 1;
tractVolumePrev = 0;
%% Loop until flood is stopped
while 1
  %% Initialize the current iteration
  i = i+1;
  tractIndices{i} = [];
  [y,x,z] = ind2sub( [output.sizes(1), output.sizes(2), output.sizes(3)], tractIndices{i-1});
  
  % X-1
  [likelihoods,posteriors,heap,terms,newTractIndices] = connectedTest( ...
      output.tensors.data, output.fa.data, likelihoods, priors, posteriors, heap, terms, x-1, y, z, output.processVars );
  tractIndices{i} = [tractIndices{i} newTractIndices'];

  % X+1
  [likelihoods,posteriors,heap,terms,newTractIndices] = connectedTest( ...
      output.tensors.data, output.fa.data, likelihoods, priors, posteriors, heap, terms, x+1, y, z, output.processVars );
  tractIndices{i} = [tractIndices{i} newTractIndices'];

  % Y-1
  [likelihoods,posteriors,heap,terms,newTractIndices] = connectedTest( ...
      output.tensors.data, output.fa.data, likelihoods, priors, posteriors, heap, terms, x, y-1, z, output.processVars );
  tractIndices{i} = [tractIndices{i} newTractIndices'];

  % Y+1
  [likelihoods,posteriors,heap,terms,newTractIndices] = connectedTest( ...
      output.tensors.data, output.fa.data, likelihoods, priors, posteriors, heap, terms, x, y+1, z, output.processVars );
  tractIndices{i} = [tractIndices{i} newTractIndices'];

  % Z-1
  [likelihoods,posteriors,heap,terms,newTractIndices] = connectedTest( ...
      output.tensors.data, output.fa.data, likelihoods, priors, posteriors, heap, terms, x, y, z-1, output.processVars );
  tractIndices{i} = [tractIndices{i} newTractIndices'];

  % Z+1
  [likelihoods,posteriors,heap,terms,newTractIndices] = connectedTest( ...
      output.tensors.data, output.fa.data, likelihoods, priors, posteriors, heap, terms, x, y, z+1, output.processVars );
  tractIndices{i} = [tractIndices{i} newTractIndices'];

  % Find the mean posterior probability for this iteration
  if( length( tractIndices{i} ) ),
    meanPosteriorProb(i) = mean( posteriors(tractIndices{i}) );
    fprintf('  The mean posterior probability is %f:  iteration %d\n', ...
            meanPosteriorProb(i), i-1);
  end

  %% Check for loop termination
  if isempty(tractIndices{i});
    break;
  end
end

% Reformat the tract to build tractRegion
b = cell2mat(tractIndices);
b = sort(b,2);
tractRegion = zeros( output.sizes(1), output.sizes(2), output.sizes(3) );
tractRegion(b) = 1;

% Smooth the tract region
tractRegionSmooth = tractRegion;
for n = 1:nSmoothingIterations,
  tractRegionSmooth = meanCurvatureSmooth3D( tractRegionSmooth );
end
tractRegionSmoothIdx = find( tractRegionSmooth > 0.5 );
tractRegionSmooth = tractRegionSmooth.*0;
tractRegionSmooth(tractRegionSmoothIdx) = 1;

% Save results
output.tractRegion = tractRegion;
output.tractRegionSmooth = tractRegionSmooth;
fprintf('    DONE\n');
