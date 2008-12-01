function [likelihoods,priors,posteriors,heap] = initializeStats( ...
		mask, anchorTract, processVars )
% function [likelihoods,priors] = initializeStats( mask, anchorTract, processVars )
% This function initializes the bayesian statistics.
% Input Parameters:
%   mask is the image mask (0=don't use me; 1=OK!)
%   anchorTract is the anchor tract
%   processVars are the process variables
% Output Parameters:
%   likelihoods are the likelihood probabilities
%   priors are the prior probabilities
%   posteriors are the posterior probabilities
%   heap is the heap: -2=mask,-1=trial,0=background,1=fiber

fprintf('  Initializing the Bayesian statistics... (initializeStats.m)');

% Preprocessing
dataSize = size(mask);
meanFiberRadiusPixels = processVars.meanFiberRadiusPixels;
neighborhoodRadius = processVars.neighborhoodRadius;
dontUseMeIdx = find( mask==0 );
dontUseMeVal = -2;

% Compute the binary anchor tract mask
binaryATMask = zeros( dataSize );
for n = 1:length(anchorTract.Y),
	binaryATMask( round(anchorTract.Y(n)), ...
								round(anchorTract.X(n)), ...
								round(anchorTract.Z(n)) ) = 1;
end
binaryATMask(dontUseMeIdx) = 0;

% Compute the distance transform
distMap = bwdist( binaryATMask );

% Initialize the likelihoods
likelihoods = zeros( dataSize );
%% Add in the anchor tract
%likelihoods(atIdx) = 1;
likelihoods(dontUseMeIdx) = 0;

% Initialize the priors
plotPriors = 0; % boolean plot priors
priors = zeros( dataSize );
res = 0.01;
dVect = [0:res:ceil(meanFiberRadiusPixels)*4];
pVect = zeros( length(dVect), 1 );
trans1 = round(meanFiberRadiusPixels/(2*res));
trans2 = meanFiberRadiusPixels/0.01 + trans1;
pVect(1:trans1) = 1;
pVect(round(trans1)+1:round(trans2)) = 0.5;
if( plotPriors ),
	figure(10)
	plot(dVect,pVect,'b');
	hold on;
end
H = fspecial('gaussian',[round(trans1) 1],round(trans1/4));
pVect = conv( H, pVect );
pVect = pVect(round(trans1/2):length(dVect)+round(trans1/2)-1)';
pVect(1:round(trans1/2)) = 1;
if( plotPriors ),
	plot(dVect,pVect,'r');
	hold off;
end
priors(dontUseMeIdx) = 0;
for y = 1:dataSize(1),
	for x = 1:dataSize(2),
		for z = 1:dataSize(3),
			dVectTemp = abs(distMap(y,x,z)-dVect);
			[minVal,minIdx] = min(dVectTemp);
			priors(y,x,z) = pVect(minIdx);
		end
	end
end

% Find the initial tract points
%% Apply a threshold on the priors
atIdx = find( distMap<=processVars.priorThreshold );

% Initialize the posteriors
posteriors = zeros( dataSize );
posteriors(atIdx) = 1;

% Initialize the heap
heap = zeros( dataSize ) - 1; % set everything to trial
heap( dontUseMeIdx ) = -2; % remove the mask indices
%% Pad the borders
heap(1:neighborhoodRadius,:,:) = dontUseMeVal;            % Pad the border of the matrix
heap(end:-1:end-neighborhoodRadius+1,:,:) = dontUseMeVal; % so the program doesn't attempt
heap(:,1:neighborhoodRadius,:) = dontUseMeVal;            % to seek voxels outside the matrix
heap(:,end:-1:end-neighborhoodRadius+1,:) = dontUseMeVal; % boundry during the while loop below.
heap(:,:,1:neighborhoodRadius) = dontUseMeVal;            %
heap(:,:,end:-1:end-neighborhoodRadius+1) = dontUseMeVal; %
heap(atIdx) = 1; % add in the fiber

fprintf('  DONE!\n');
