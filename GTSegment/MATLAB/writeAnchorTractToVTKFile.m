function writeAnchorTractToVTKFile( output )
% function writeAnchorTractToVTKFile( output )
% Write the anchor tract to a .vtk file.
% Input Parameters:
%   output is a struct containing all of the output information
% Output Parameters:
%   n/a
%% NOTE: The anchor tract should be written out in RAS coordinates

fprintf('Writing the anchor tract to a .vtk file... (writeAnchorTractToVTKFile.m)\n');

% construct the spaceMatrix
spaceMatrix = zeros(3,4);
spaceMatrix(:,1:3) = output.spacedirectionsFull;
spaceMatrix(1,4) = output.spaceoriginFull(1);
spaceMatrix(2,4) = output.spaceoriginFull(2);
spaceMatrix(3,4) = output.spaceoriginFull(3);

% write the header
fid = fopen( output.ioVars.vtkOutputFilePath, 'w' );
fprintf( fid, '# vtk DataFile Version 2.0\n');
fprintf( fid, 'A Smooth Anchor Tract\n');
fprintf( fid, 'ASCII\n');
fprintf( fid, 'DATASET POLYDATA\n');

% convert points to Space and write out the points
%% must convert tract point to 0-indexing
nPoints = size( output.anchorTractSmooth.Y, 1 );
fprintf( fid, 'POINTS %i float\n', nPoints );
for iPoint=1:nPoints,
  Space = spaceMatrix * [ output.anchorTractSmooth.Y(iPoint)-1 + output.cropfun(1); ...
                    output.anchorTractSmooth.X(iPoint)-1 + output.cropfun(3); ...
                    output.anchorTractSmooth.Z(iPoint)-1 + output.cropfun(5); ...
                    1 ];
  fprintf( fid, '%d ', Space(1) ); % R
  fprintf( fid, '%d ', Space(2) ); % A
  fprintf( fid, '%d ', Space(3) ); % S
  fprintf( fid, '\n' );
end
fprintf( fid, '\n' );

% write out anchor tract line
nLines = 1; % only the anchor tract
fprintf( fid, 'LINES %d %d\n', nLines, nPoints+nLines );
fprintf( fid, '%d ', nPoints );
for iPoint = 1:nPoints,
  fprintf( fid, '%d ', iPoint-1 );
  if( mod(iPoint,10) == 0 ),
    fprintf( fid, '\n' );
  end
end
fprintf( fid, '\n' );
fprintf( fid, '\n' );

% write out the interpolated tensors at each point
fprintf( fid, 'POINT_DATA %d\n', nPoints );
fprintf( fid, 'TENSORS tensors float\n' );
%% convert the gradient directions to Space
for n = 1:size(output.grads,1),
  gradsSpace(n,:) = spaceMatrix(:,1:3) * ...
      [ output.grads(n,1); ...
        output.grads(n,2); ...
        output.grads(n,3)];
  B(n,1) = gradsSpace(n,1)^2;
  B(n,2) = 2*gradsSpace(n,1)*gradsSpace(n,2);
  B(n,3) = 2*gradsSpace(n,1)*gradsSpace(n,3);
  B(n,4) = gradsSpace(n,2)^2;
  B(n,5) = 2*gradsSpace(n,2)*gradsSpace(n,3);
  B(n,6) = gradsSpace(n,3)^2;
end
Bpinv = pinv(B);
b = output.dwi.bvalue;
for iPoint = 1:nPoints,
  point = [ output.anchorTractSmooth.Y(iPoint); ...
            output.anchorTractSmooth.X(iPoint); ...
            output.anchorTractSmooth.Z(iPoint) ];
  % interpolate DWI at the point
  tensorPtSix = tensorEstimPt( output.dwi.data, output.baseline.data, ...
                               output.mask.data, Bpinv, b, point );
  tensorPt(1) = tensorPtSix(1);
  tensorPt(2) = tensorPtSix(2);
  tensorPt(3) = tensorPtSix(3);
  tensorPt(4) = tensorPtSix(2);
  tensorPt(5) = tensorPtSix(4);
  tensorPt(6) = tensorPtSix(5);
  tensorPt(7) = tensorPtSix(3);
  tensorPt(8) = tensorPtSix(5);
  tensorPt(9) = tensorPtSix(6);
  % write tensorPt to file
  for n = 1:9,
    fprintf( fid, '%f ', tensorPt(n) );
  end
  fprintf( fid, '\n' );
end

fclose( fid );
