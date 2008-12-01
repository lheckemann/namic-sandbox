function tensorPtSix = tensorEstimPt( dwi, baseline, mask, Bpinv, b, point )
% function tensorPtSix = tensorEstimPt( dwi, baseline, mask, Bpinv, b, point )
% Interpolate the tensor at an off-grid point.
% Input Parameters:
%   dwi is the raw DWI data
%   baseline is the baseline b0 data
%   mask is the volumetric mask
%   Bpinv is the pseudo inverse of the B matrix
%   b is the b-value of the DWI data
%   point is the coordinates of the off-grid point
% Output Parameters:
%   tensorPtSix is the 6 component tensor at the point

% Find neighboring grid points
yS = ceil( point(1) );
yN = floor( point(1) );
xE = ceil( point(2) );
xW = floor( point(2) );
zU = ceil( point(3) );
zD = floor( point(3) );
if( yS == yN ), % fix error cond on gridlines
  yS = yS + 1;
  yN = yN - 1;
end
if( xE == xW ), % fix error cond on gridlines
  xE = xE + 1;
  xW = xW - 1;
end
if( zU == zD ), % fix error cond on gridlines
  zU = zU + 1;
  zD = zD - 1;
end

% Make sure all the points lie in the volume
sizes = size(dwi);
if( yS > sizes(1) ),
	yS = sizes(1);
end
if( yN < 1 ),
	yN = 1;
end
if( xE > sizes(2) ),
	xE = sizes(2);
end
if( xW < 1 ),
	xW = 1;
end
if( zU > sizes(3) ),
	zU = sizes(3);
end
if( zD < 1 ),
	zD = 1;
end

% Calculate weights for neighboring grid points
ySdist = yS-point(1);
yNdist = point(1)-yN;
xEdist = xE-point(2) ;
xWdist = point(2)-xW;
zUdist = zU-point(3);
zDdist = point(3)-zD;

% Calculate weights
nwdMask = mask(yN,xW,zD);
if( nwdMask == 0 ),
  nwdWeight = 0;
else
  nwdWeight = ySdist*xEdist*zUdist;
end
nedMask = mask(yN,xE,zD);
if( nedMask == 0 ),
  nedWeight = 0;
else
  nedWeight = ySdist*xWdist*zUdist;
end
swdMask = mask(yS,xW,zD);
if( swdMask == 0 ),
  swdWeight = 0;
else
  swdWeight = yNdist*xEdist*zUdist;
end
sedMask = mask(yS,xE,zD);
if( sedMask == 0 ),
  sedWeight = 0;
else
  sedWeight = yNdist*xWdist*zUdist;
end
nwuMask = mask(yN,xW,zU);
if( nwuMask == 0 ),
  nwuWeight = 0;
else
  nwuWeight = ySdist*xEdist*zDdist;
end
neuMask = mask(yN,xE,zU);
if( neuMask == 0 ),
  neuWeight = 0;
else
  neuWeight = ySdist*xWdist*zDdist;
end
swuMask = mask(yS,xW,zU);
if( swuMask == 0 ),
  swuWeight = 0;
else
  swuWeight = yNdist*xEdist*zDdist;
end
seuMask = mask(yS,xE,zU);
if( seuMask == 0 ),
  seuWeight = 0;
else
  seuWeight = yNdist*xWdist*zDdist;
end

% Normalize all the weight to sum to 1.
denom = nwdWeight + nedWeight + ...
        swdWeight + sedWeight + ...
        nwuWeight + neuWeight + ...
        swuWeight + seuWeight;
if( denom == 0 ), 
  % We are surrounded by INF values, the tract will
  % end prematurely here.
	tensorPtSix = zeros(6,1);
  return;
end
nwdWeight  = nwdWeight / denom;
nedWeight  = nedWeight / denom;
swdWeight  = swdWeight / denom;
sedWeight  = sedWeight / denom;
nwuWeight  = nwuWeight / denom;
neuWeight  = neuWeight / denom;
swuWeight  = swuWeight / denom;
seuWeight  = seuWeight / denom;

% Interpolate the DWI at the point
dwiPt = squeeze( ...
    dwi(yN,xW,zD,:) * nwdWeight + ...
    dwi(yN,xE,zD,:) * nedWeight + ...
    dwi(yS,xW,zD,:) * swdWeight + ...
    dwi(yS,xE,zD,:) * sedWeight + ...
    dwi(yN,xW,zU,:) * nwuWeight + ...
    dwi(yN,xE,zU,:) * neuWeight + ...
    dwi(yS,xW,zU,:) * swuWeight + ...
    dwi(yS,xE,zU,:) * seuWeight );
zeroPts = find( dwiPt == 0 );
dwiPt(zeroPts) = eps;

% Interpolate the DWI at the point
baselinePt = ...
    baseline(yN,xW,zD) * nwdWeight + ...
    baseline(yN,xE,zD) * nedWeight + ...
    baseline(yS,xW,zD) * swdWeight + ...
    baseline(yS,xE,zD) * sedWeight + ...
    baseline(yN,xW,zU) * nwuWeight + ...
    baseline(yN,xE,zU) * neuWeight + ...
    baseline(yS,xW,zU) * swuWeight + ...
    baseline(yS,xE,zU) * seuWeight;

% Interpolate the tensor at the point
s = -log( dwiPt / baselinePt ) / b;
tensorPtSix = Bpinv * s;

if( sum(isinf(tensorPtSix)) ),
	% There are INF tensor values
	keyboard;
end
