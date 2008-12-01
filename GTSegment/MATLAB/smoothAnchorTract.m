function anchorTractSmooth = smoothAnchorTract( anchorTract, processVars )
% function anchorTractSmooth = smoothAnchorTract( anchorTract, processVars )
% This function smoothes the anchor tract.
% Input Parameters:
%   anchorTract is the anchor tract
%   processVars is a struct containing all of the process variables
% Output Parameters:
%   anchorTractSmooth is the smoothed anchor tract

fprintf('Smoothing the anchor tract... (smoothAnchorTract.m)\n');

anchorTractSmooth = anchorTract;
tsks = processVars.tractSmoothingKernelSize;
nPoints = length( anchorTract.Y );
for iPoint=1:nPoints,
  % Check for beginning bounds
  minIdx = iPoint-tsks;
  maxIdx = iPoint+tsks;
  if( minIdx <= 0 ),
    minIdx = 1;
    maxIdx = iPoint+iPoint-minIdx;
  end
  if( maxIdx > nPoints ),
    maxIdx = nPoints;
    minIdx = iPoint-(nPoints-iPoint);
  end
  anchorTractSmooth.Y(iPoint) = mean( anchorTract.Y(minIdx:maxIdx) );
  anchorTractSmooth.X(iPoint) = mean( anchorTract.X(minIdx:maxIdx) );
  anchorTractSmooth.Z(iPoint) = mean( anchorTract.Z(minIdx:maxIdx) );
end
