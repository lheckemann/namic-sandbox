function output = solveHJB( output )
% function output = solveHJB( output )
% This function solves the HJB equation to find the tracts,
% according to Kao, et al.
% Input Parameters:
%   output is a struct containing all of the output information
% Output Parameters:
%   output is a struct containing all of the output information

fprintf('Solve the Hamilton-Jacobi-Bellman equation... (solveHJB.m)\n');

% Contstruct the inputs
%% Add the seed points
%% Add the padding
[ySeed,xSeed,zSeed] = ind2sub( output.sizes(1:3), output.seedPts );
foreIdx = find( output.mask.data );
backIdx = find( output.mask.data==0 );
maxLocalCost = 0;
for n = 1:output.sizes(4),
  localCostsSlice = output.localCosts(:,:,:,n);
  maxLocalCostTemp = max( localCostsSlice(foreIdx) );
  if( maxLocalCostTemp > maxLocalCost ),
    maxLocalCost = maxLocalCostTemp;
  end
end
clear localCostsSlice;
if( isinf(maxLocalCost) ),
  fprintf('  ERROR: the max local cost is INF.\n');
  keyboard;
end
Tsmall = zeros( output.sizes(1), output.sizes(2), output.sizes(3) );
Tsmall(foreIdx) = pi*max(size(Tsmall))*maxLocalCost;
Tsmall(backIdx) = inf;
Tsmall(ySeed,xSeed,zSeed) = 0; % set the seed points back to zero
T = inf( output.sizes(1)+2, output.sizes(2)+2, output.sizes(3)+2 );
T( 2:output.sizes(1)+1, 2:output.sizes(2)+1, 2:output.sizes(3)+1 ) ...
    = Tsmall;
clear Tsmall;
L = T;
localCosts = zeros( output.sizes(1)+2, output.sizes(2)+2, ...
                    output.sizes(3)+2, output.sizes(4));
localCosts( 2:output.sizes(1)+1, 2:output.sizes(2)+1, ...
            2:output.sizes(3)+1, : ) = output.localCosts;
%% Pad the mask
mask.data = zeros( output.sizes(1)+2, output.sizes(2)+2, output.sizes(3)+2 );
mask.data( 2:output.sizes(1)+1, 2:output.sizes(2)+1, 2:output.sizes(3)+1 ) = ...
    output.mask.data;

% Call Mex Function to perform Fast Sweeping
fprintf( '  -compiling the mex fast sweeping...');
if( output.processVars.testMode ),
  cd MATLAB/;
  mex fast_sweeping.c;
  cd ../;
end
fprintf(' DONE\n');
T = fast_sweeping( ...
		T, localCosts, mask.data, ...
		output.grads, output.processVars.sweepingConvergenceParam );

% Remove Border Pixels added in the MEX Code
T = T( 2:output.sizes(1)+1, 2:output.sizes(2)+1, 2:output.sizes(3)+1 );
output.T = T;

% Remove the inf values from T* and L* to help in computing gradients
Tnoinf = T;
infIdx = find( isinf(Tnoinf) );
Tnoinf(infIdx) = 0;
[distDummy,nearNeighborIdx] = bwdist(Tnoinf);
Tnoinf = Tnoinf( nearNeighborIdx );
Tnoinf(output.seedPts) = 0; % add the seed points back in

% Compute the gradient of Tnoinf
[dx,dy,dz] = gradient( Tnoinf );
output.Tgrad(:,:,:,1) = -dy;
output.Tgrad(:,:,:,2) = -dx;
output.Tgrad(:,:,:,3) = -dz;

fprintf(' DONE\n');
