function [value,isterminal,direction] = ...
    btConvergeEvent( lCur, pCur, seedPtMatrix, convergeParam )
% function [value,isterminal,direction] = ...
%     btConvergeEvent( lCur, pCur, seedPtMatrix, convergeParam )
% This function determines if back tracting has converged.
% Input Parameters:
%   lCur is the current length of the tract
%   pCur is the current position of the tract
%   seedPtMatrix is a matrix of seed points
%   convergeParam is when to stop the back tracting
% Output Parameters:
%   value is the event value
%   isterminal whether or not the event is terminal
%   direction the dirction from when the event was approached

% Compute the minimal Euclidean distance between pCur and the seed
% points.  If less than the convergence parameter, then converged.
distance = Inf;
for n = 1:size(seedPtMatrix,1),
  distanceTemp = sqrt( ...
      (seedPtMatrix(n,1)-pCur(1))^2 + ...
      (seedPtMatrix(n,2)-pCur(2))^2 + ...
      (seedPtMatrix(n,3)-pCur(3))^2 ...
      );
  if( distanceTemp < distance ),
    distance = distanceTemp;
  end
end

value = distance - convergeParam;
if( distance < convergeParam ),
  value = 0;
end
isterminal = 1; % yes, stop at this value
direction = 0;  % from any direction
