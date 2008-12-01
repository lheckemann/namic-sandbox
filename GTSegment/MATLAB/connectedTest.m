function [likelihoods,posteriors,heap,terms,newTractIndices] = ...
    connectedTest( tensors, fa, likelihoods, priors, posteriors, ...
                   heap, terms, x, y, z, processVars )
% function [likelihoods,posteriors,heap,newTractIndices] = ...
%     connectedTest( tensors, fa, likelihoods, priors, posteriors, ...
%                    heap, x, y, z, processVars )
% This function tests whether or not a point should be added to the tract.
% Input Parameters:
%   tensors is the tensor data
%   fa is the fractional anisotropy volume
%   likelihoods are the likelihood probabilities
%   priors are the prior probabilities
%   posteriors are the posterior probabilities
%   heap is the heap: -2=mask,-1=trial,0=background,1=fiber
%   terms are the terms of the energy
%   [x,y,z] are the points under evaluation
%   processVars are the process variables
% Output Parameters:
%   likelihoods are the likelihood probabilities
%   posteriors are the posterior probabilities
%   heap is the heap: -2=mask,-1=trial,0=background,1=fiber
%   terms are the terms of the energy
%   newTractIndices are the new points to be added to the tract

% Preprocessing
sizes = size(fa);
nhoodRadius = processVars.neighborhoodRadius;
vol = (nhoodRadius*2+1)^3;

% Iterate through all the points under evaluation
newTractIndices = [];
for n = 1:length(x),
  % Check points too close to the boundary
	if( ((y(n)-nhoodRadius)<1) || ...
			((x(n)-nhoodRadius)<1) || ...
			((z(n)-nhoodRadius)<1) || ...
			((y(n)+nhoodRadius)>sizes(1)) || ...
			((x(n)+nhoodRadius)>sizes(2)) || ...
			((z(n)+nhoodRadius)>sizes(3)) ),
		heap(y(n),x(n),z(n)) = 0;
		continue;
	end
  if( (heap(y(n),x(n),z(n))==-1) | (heap(y(n),x(n),z(n))==0) ),
    % Create neighborhood volumes
    nhoodTensors = tensors( ...
        :, ...
        y(n)-nhoodRadius:y(n)+nhoodRadius, ...
        x(n)-nhoodRadius:x(n)+nhoodRadius, ...
        z(n)-nhoodRadius:z(n)+nhoodRadius );
    nhoodFA = fa( ...
        y(n)-nhoodRadius:y(n)+nhoodRadius, ...
        x(n)-nhoodRadius:x(n)+nhoodRadius, ...
        z(n)-nhoodRadius:z(n)+nhoodRadius );
    nhoodLike = likelihoods( ...
        y(n)-nhoodRadius:y(n)+nhoodRadius, ...
        x(n)-nhoodRadius:x(n)+nhoodRadius, ...
        z(n)-nhoodRadius:z(n)+nhoodRadius );
    nhoodPriors = priors( ...
        y(n)-nhoodRadius:y(n)+nhoodRadius, ...
        x(n)-nhoodRadius:x(n)+nhoodRadius, ...
        z(n)-nhoodRadius:z(n)+nhoodRadius );
    nhoodPost = posteriors( ...
        y(n)-nhoodRadius:y(n)+nhoodRadius, ...
        x(n)-nhoodRadius:x(n)+nhoodRadius, ...
        z(n)-nhoodRadius:z(n)+nhoodRadius );
    nhoodHeap = heap( ...
        y(n)-nhoodRadius:y(n)+nhoodRadius, ...
        x(n)-nhoodRadius:x(n)+nhoodRadius, ...
        z(n)-nhoodRadius:z(n)+nhoodRadius );

    % Determine 'in' and 'out' points
    sizesSmall = size(nhoodFA);
    centerPt = [ nhoodRadius+1, ...
                 nhoodRadius+1, ...
                 nhoodRadius+1 ];
    cPt = sub2ind( sizesSmall, ...
                   centerPt(1), ...
                   centerPt(2), ...
                   centerPt(3) );
    iPts = find( nhoodHeap==1 );
    piPts = [iPts; cPt];
    poPts = find( (nhoodHeap~=1) & (nhoodHeap~=-2) );
    oPts = setdiff( poPts, cPt );
    aPts = [iPts; poPts];

    % Determin the prior probability
    priorProb = priors(y(n),x(n),z(n));

    % Check for points surrounded by fiber
    if( length(oPts)==0 ),
      idx = sub2ind(sizes,y(n),x(n),z(n));
      newTractIndices = [newTractIndices; idx];
      heap(idx) = 1;
      likelihoods(y(n),x(n),z(n)) = 1;
      posteriors(y(n),x(n),z(n)) = 1;
      break;
    end
    
    % Compute FAo/FAi
    %% Point
    FAp = fa(y(n),x(n),z(n));
    %% Out
    FAo = mean( nhoodFA(oPts) );
    FApo = mean( [FAp; nhoodFA(oPts)] );
    %% In
    FAi = mean( nhoodFA(iPts) );
    FApi = mean( [FAp; nhoodFA(iPts)] );
    
    % Create the matrix of principle eigenvectors
    PEvect = zeros( [sizesSmall, 3] );
    for iA = 1:length(aPts),
      [yPt,xPt,zPt] = ind2sub(sizesSmall,aPts(iA));
      tensorVect = squeeze(nhoodTensors(:,yPt,xPt,zPt));
      tensorTemp = [ tensorVect(1) tensorVect(2) tensorVect(3); ...
                     tensorVect(2) tensorVect(4) tensorVect(5); ...
                     tensorVect(3) tensorVect(5) tensorVect(6) ];
      [V,D] = eig( tensorTemp );
      [maxVal,maxIdx] = max([D(1) D(5) D(9)]);
      PEvect(yPt,xPt,zPt,:) = maxVal*V(:,maxIdx);
    end

    % Compute MEo/MEi
    PEvectY = PEvect(:,:,:,1);
    PEvectX = PEvect(:,:,:,2);
    PEvectZ = PEvect(:,:,:,3);
    %% Point
    MEp = squeeze(PEvect( centerPt(1),centerPt(2),centerPt(3),: ));
    %% Out
    MEo = [0; 0; 0];
    MEo(1) = mean( PEvectY(oPts) );
    MEo(2) = mean( PEvectX(oPts) );
    MEo(3) = mean( PEvectZ(oPts) );
    MEo = MEo ./ sqrt(dot(MEo,MEo));
    MEpo = [0; 0; 0];
    MEpo(1) = mean( [MEp(1);PEvectY(oPts)] );
    MEpo(2) = mean( [MEp(2);PEvectX(oPts)] );
    MEpo(3) = mean( [MEp(3);PEvectZ(oPts)] );
    MEpo = MEpo ./ sqrt(dot(MEpo,MEpo));
    %% In
    MEi = [0; 0; 0];
    MEi(1) = mean( PEvectY(iPts) );
    MEi(2) = mean( PEvectX(iPts) );
    MEi(3) = mean( PEvectZ(iPts) );
    MEi = MEi ./ sqrt(dot(MEi,MEi));
    MEpi = [0; 0; 0];
    MEpi(1) = mean( [MEp(1);PEvectY(iPts)] );
    MEpi(2) = mean( [MEp(2);PEvectX(iPts)] );
    MEpi(3) = mean( [MEp(3);PEvectZ(iPts)] );
    MEpi = MEpi ./ sqrt(dot(MEpi,MEpi));

    % Normalize PEvectY,PEvectX,PEvectZ
    PEdenom = sqrt( PEvectY.*PEvectY+PEvectX.*PEvectX+PEvectZ.*PEvectZ );
    maskIdx = find( nhoodHeap==-2 );
    PEdenom(maskIdx) = 1;
    PEvectY = PEvectY ./ PEdenom;
    PEvectX = PEvectX ./ PEdenom;
    PEvectZ = PEvectZ ./ PEdenom;

    % Setup the likelihood energy computations
    eFAi = abs( nhoodFA - FAi );
    eFApi = abs( nhoodFA - FApi );
    eFAo = abs( nhoodFA - FAo );
    eFApo = abs( nhoodFA - FApo );
    eMEi = sqrt(nhoodFA*FAi) .* ...
					 abs( PEvectY * MEi(1) + ...
								PEvectX * MEi(2) + ...
								PEvectZ * MEi(3) );
    eMEpi = sqrt(nhoodFA*FApi) .*...
						abs( PEvectY * MEpi(1) + ...
								 PEvectX * MEpi(2) + ...
								 PEvectZ * MEpi(3) );
    eMEo = sqrt(nhoodFA*FAo) .* ...
					 abs( PEvectY * MEo(1) + ...
								PEvectX * MEo(2) + ...
								PEvectZ * MEo(3) );
    eMEpo = sqrt(nhoodFA*FApo) .* ...
						abs( PEvectY * MEpo(1) + ...
								 PEvectX * MEpo(2) + ...
								 PEvectZ * MEpo(3) );

    % Compute the energy of the point belonging to the fiber
    eLFApii = sum( eFApi(piPts) ) / (2*vol);
    eLMEpii = sum( 1 - eMEpi(piPts) ) / (2*vol);
    eLFAoi = sum( eFAo(oPts) ) / (2*vol);
    eLMEoi = sum( 1 - eMEo(oPts) ) / (2*vol);
    eLi = eLFApii + eLMEpii + eLFAoi + eLMEoi;
    ePi = sum( 1 - nhoodPriors(piPts) ) / vol + ...
          sum( nhoodPriors(oPts) ) / vol;
    ei = eLi + ePi;
    ei = ei/2;
    
    % Compute the energy the point not belonging to the fiber
    eLo = sum( 1 + eFAi(iPts) - eMEi(iPts) ) / (2*vol) + ...
          sum( 1 + eFApo(poPts) - eMEpo(poPts) ) / (2*vol);
    ePo = sum( 1 - nhoodPriors(iPts) ) / vol + ...
          sum( nhoodPriors(poPts) ) / vol;
    eo = eLo + ePo;
    eo = eo/2;
    
    % Test for connectivity
    if( ei<=eo ),
      idx = sub2ind(sizes,y(n),x(n),z(n));
      newTractIndices = [newTractIndices; idx];
      heap(idx) = 1;
    end

    % Save the results
    terms.eLFApii(y(n),x(n),z(n)) = eLFApii;
		terms.eLMEpii(y(n),x(n),z(n)) = eLMEpii;
		terms.eLFAoi(y(n),x(n),z(n)) = eLFAoi;
		terms.eLMEoi(y(n),x(n),z(n)) = eLMEoi;
    likelihoods(y(n),x(n),z(n)) = eLo-eLi;
    posteriors(y(n),x(n),z(n)) = eo-ei;
  end
end
