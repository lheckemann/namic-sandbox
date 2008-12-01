function dy = computeTgrad( lCur, pCur, TgradNorm )
% function dy = computeTgrad( lCur, pCur, TgradNorm )
% This function finds gradient of the arrival time at a point.
% Input Parameters:
%   lCur is the current length of the tract
%   pCur is the current position of the tract
%   TgradNorm is the normalized gradient image of the arrival times
% Output Parameters:
%   dy is the gradient vector at the pCur point (column vector)

% Interpolate the components of Tgrad
dy = zeros(3,1);
dy(1) = interp3( TgradNorm(:,:,:,1), ...
                 pCur(2),pCur(1),pCur(3) );
dy(2) = interp3( TgradNorm(:,:,:,2), ...
                 pCur(2),pCur(1),pCur(3) );
dy(3) = interp3( TgradNorm(:,:,:,3), ...
                 pCur(2),pCur(1),pCur(3) );
dy = dy/norm(dy);
