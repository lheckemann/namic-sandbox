function output = anchorTractography( output )
% function output = anchorTractography( output )
% This is function finds a tracts from a local cost matrix through
% fast sweeping methods according to Kao, et al.
% Input Parameters:
%   output is a struct containing all of the output information
% Output Parameters:
%   output is a struct containing all of the output information

fprintf('Begin tractography... (anchorTractography.m)\n');

% Run fast sweeping to find the speeds
output = solveHJB( output );

fprintf('  Starting the back tracting... (anchorTractography.m)\n');

% Execute the Back Tracting
backIdx = find( output.mask.data == 0 );
targetPts = setdiff( output.targetPts, backIdx );

%% Find the minimum target point
seedPtMatrix = zeros( length(output.seedPts), 3 );
[seedPtMatrix(:,1),seedPtMatrix(:,2),seedPtMatrix(:,3)] = ...
    ind2sub( output.sizes(1:3), output.seedPts );
targetT = output.T( targetPts );
[minVal,minIdx] = min( targetT );
optTargetPt = targetPts( minIdx );
[y,x,z] = ind2sub( output.sizes(1:3), optTargetPt );
IC = [y,x,z];
LSPAN = [0 1000];

% Compute normalized Tgrad
TgradNorm = output.Tgrad;
TgradNormDenom = sqrt( output.Tgrad(:,:,:,1).^2 + ...
                       output.Tgrad(:,:,:,2).^2 + ...
                       output.Tgrad(:,:,:,3).^2 );
foreIdx = find( TgradNormDenom ~= 0 ); % avoid dividing by zero
TgradNorm1 = output.Tgrad(:,:,:,1);
TgradNorm2 = output.Tgrad(:,:,:,2);
TgradNorm3 = output.Tgrad(:,:,:,3);
TgradNorm1(foreIdx) = TgradNorm1(foreIdx) ./ TgradNormDenom(foreIdx);
TgradNorm2(foreIdx) = TgradNorm2(foreIdx) ./ TgradNormDenom(foreIdx);
TgradNorm3(foreIdx) = TgradNorm3(foreIdx) ./ TgradNormDenom(foreIdx);
TgradNorm(:,:,:,1) = TgradNorm1;
TgradNorm(:,:,:,2) = TgradNorm2;
TgradNorm(:,:,:,3) = TgradNorm3;
output.TgradNorm = TgradNorm;
clear TgradNormDenom TgradNorm1 TgradNorm2 TgradNorm3 TgradNorm;

%% Solve the ODE
fprintf('    Starting the ODE45... (anchorTractography.m)');
output.processVars.backTractingConvergenceParam;
options = odeset( ...
    'RelTol', output.processVars.backTractRelTol, ...
    'AbsTol', output.processVars.backTractAbsTol, ...
    'MaxStep', output.processVars.maxStep, ...
    'NormControl', 'on', ...  
    'Events', @(l,p) btConvergeEvent( ...
				l, p, seedPtMatrix, ...
				output.processVars.backTractingConvergenceParam ) );
[lTract pTract] = ode45(@(l,p) computeTgrad( ...
    l, p, output.TgradNorm ), ...
                        LSPAN, IC, options ); % Solve ODE
fprintf(' DONE\n');

%% Make the anchor tract
anchorTract.Y = pTract(:,1);
anchorTract.X = pTract(:,2);
anchorTract.Z = pTract(:,3);
anchorTract.lTract = lTract;
%%%% Add in the closest seed point
distance = Inf;
seedPtIdx = 0;
for n = 1:size(seedPtMatrix,1),
  distanceTemp = sqrt( ...
      (seedPtMatrix(n,1)-anchorTract.Y(end))^2 + ...
      (seedPtMatrix(n,2)-anchorTract.X(end))^2 + ...
      (seedPtMatrix(n,3)-anchorTract.Z(end))^2 ...
      );
  if( distanceTemp < distance ),
		seedPtIdx = n;
    distance = distanceTemp;
  end
end
anchorTract.Y(end+1) =  seedPtMatrix(seedPtIdx,1);
anchorTract.X(end+1) =  seedPtMatrix(seedPtIdx,2);
anchorTract.Z(end+1) =  seedPtMatrix(seedPtIdx,3);
anchorTract.lTract(end+1) = 0;
anchorTractSmooth = smoothAnchorTract( anchorTract, output.processVars );

% Write out the anchor tract to a VTK file
output.anchorTract = anchorTract;
output.anchorTractSmooth = anchorTractSmooth;
writeAnchorTractToVTKFile( output );

fprintf(' DONE\n');
