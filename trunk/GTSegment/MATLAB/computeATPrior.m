function pAT = computeATPrior( anchorTract, dataSize, processVars );
% function pAT = computeATPrior( anchorTract, processVars );
% This function computes priors based on the previously computed
% anchor tract.
% Input Parameters:
%   anchorTract is the anchor tract
%   processVars are the process variables
% Output Parameters:
%   pAT are the anchor tract priors

fprintf('  Computing the anchor tract priors... (computeATPrior.m)\n');

% Compute the distance map
binaryATMask = zeros( dataSize );
for n = 1:length(anchorTract.Y),
	binaryATMask( round(anchorTract.Y(n)), ...
								round(anchorTract.X(n)), ...
								round(anchorTract.Z(n)) ) = 1;
end
distMap = bwdist( binaryATMask );

% Compute the prior map
pAT = exp(-distMap);
