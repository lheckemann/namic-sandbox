function [odfSharpPt, prinODFDirectionPt] = ...
    computeODF( dwiNormalPt, basis, srt, gradsIn );
% function [odfSharpPt, prinODFDirectionPt] = ...
%     computeODF( dwiNormalPt, basis, srt, gradsIn );
% This function computes the ODF.
% Input Parameters:
%   dwiNormalPt is the dwi / baseline at a point
%   basis is the signal matrix
%   srt is the spherical radon transform
%   gradsIn are the corresponding directions (3 x nDir)
% Output Parameters:
%   odfSharpPt is the sharpened odf at a point
%   prinODFDirectionPt is the pricipal ODF direction at a point
 
NOF=1;

dwiNormalPt = [dwiNormalPt;dwiNormalPt];
Sn = dwiNormalPt/norm(dwiNormalPt);

[coef,indx] = matchpursuit(basis,Sn,NOF);

odfSharpPt = srt(:,indx);

prinODFDirectionPt = squeeze( gradsIn(:,indx) );

% Cut out the anti-podal pairs for use in the rest of the code
odfSharpPt = abs( odfSharpPt(1:end/2) );
