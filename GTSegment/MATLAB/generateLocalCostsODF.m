function output = generateLocalCostsODF( output )
% function output = generateLocalCostsODF( output )
% This function generates the local costs from file.
% Input Parameters:
%   output is a struct containing all of the output information
% Return Value:
%   output is a struct containing all of the output information

fprintf('Generating the local costs... (generateLocalCostsODF.m)\n');

% Find foreground and background values in the ODF
foreIdx = find( output.mask.data );
[yForeIdx,xForeIdx,zForeIdx] = ind2sub( output.sizes(1:3), foreIdx );
nPoints = length( foreIdx );

% Setup the ODF Code
[basis, srt, grads] = findBasisAndRadon( output );
prinODFDirections = zeros( [output.sizes(1:3), 3] );

% Compute the local costs ODF
backgroundODFVal = -1;
odf = zeros( output.sizes ) + backgroundODFVal;
for iPoint = 1:nPoints,
  iY = yForeIdx(iPoint);
  iX = xForeIdx(iPoint);
  iZ = zForeIdx(iPoint);
  diffVals = squeeze(output.dwi.data(iY,iX,iZ,:));
  dwiNormalPt = squeeze( output.dwi.data(iY,iX,iZ,:) / output.baseline.data(iY,iX,iZ) );
  [odfVect, prinODFDirections(iY,iX,iZ,:)] = ...
      computeODF( dwiNormalPt, basis, srt, grads );
  odf(iY,iX,iZ,:) = odfVect;
end
foreIdx4D = find( odf(:) > backgroundODFVal );

% Compute the local costs:  -log(ODF)
localCosts = inf( size(odf) );
localCosts(foreIdx4D) = -log(odf(foreIdx4D));
output.localCosts = localCosts;
fprintf(' DONE\n');
