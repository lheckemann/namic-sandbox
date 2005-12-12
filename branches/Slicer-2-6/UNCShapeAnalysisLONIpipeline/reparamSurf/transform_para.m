function [shape_corrs,lookup] = transform_corrs(sph_corrs,sX,T,X,lookup);
% 
% Transform correspondence points from the sphere to the shape 
%
% In:
%   sph_corrs: correspondence points on the sphere
%   sX: vertices of the shape mapped onto the sphere
%   T, X: Faces and Verts of the original shape 
%           (faces not used until we do proper barycentric coordinates)
%
% Out:
%   sphere_corrs: correspondence points mapped back to the shape
%

nT = size(T,1);

if nargin<5 | isempty(lookup)
    % Create a lookup table of probable faces on which the correspondences lie
    % Calculte the centroid of each sphere face
    [Tx,Ty,Tz] = fv2tri(sX,T);
    Scent = [mean(Tx,2),mean(Ty,2),mean(Tz,2)];
    lookup = mex_nearest_pt(sph_corrs, Scent);
    
end

% Given that we have a good guess of which face each point resides
% on, calculate the barycentric coordinates for each correspondence point

% Compute the parameters for each sphere triangle
P1 = sX(T(:,1),:);
P2 = sX(T(:,2),:);
P3 = sX(T(:,3),:);
N12 = cross(P1,P2,2);
N23 = cross(P2,P3,2);
N31 = cross(P3,P1,2);
D = dot(P1,N23,2);

% Assign the probable triangles to the lookups
N12lk = N12(lookup,:);
N23lk = N23(lookup,:);
N31lk = N31(lookup,:);
Dlk = D(lookup,:);

% Find and remove the zero denominators
zero_D = find(Dlk==0);
Dlk(zero_D) = 1;

% Compute the barycentric coordinates
br(:,1) = dot(sph_corrs,N23lk,2)./Dlk;
br(:,2) = dot(sph_corrs,N31lk,2)./Dlk;
br(:,3) = dot(sph_corrs,N12lk,2)./Dlk;

% Replace the zero denominators
br(zero_D,1) = NaN;
br(zero_D,2) = NaN;
br(zero_D,3) = NaN;

% Check to see whether these are valid and within the
% specified face
% Do it this way or else we load the correspondences whose barycentric
% coordinates have been assigned NaN
valid = find(br(:,1)>=0 & br(:,1)<=1 & br(:,2)>=0 & br(:,2)<=1 & br(:,3)>=0 & br(:,3)<=1);
invalid = setdiff([1:size(sph_corrs,1)],valid);

incident = mex_incident_tri(sph_corrs(invalid,:), N12, N23, N31, D); 

not_found = find(incident==-1);
if any(not_found)
    V_nrm = sph_corrs(invalid(not_found),:);
    V_orig = zeros(size(V_nrm));

    % mex function which find the hit face (and point)
    [dist,hit_face,hit_pt] = hit_point(V_orig,V_nrm,T,sX);
    incident(not_found) = hit_face;
    
    % If we still can't find it, revert to the nearest face
    not_found = find(incident==-1);
    if any(not_found)
        [Tx,Ty,Tz] = fv2tri(sX,T);
        Scent = [mean(Tx,2),mean(Ty,2),mean(Tz,2)];
        incident(not_found) = mex_nearest_pt(sph_corrs(invalid(not_found),:), Scent);     
    end
    
end
% Replace wring faces reference
lookup(invalid) = incident;

% Re-calculate the baricentric coordinates for these newly found triangles
% Assign the probable triangles to the lookups
N12lk = N12(lookup,:);
N23lk = N23(lookup,:);
N31lk = N31(lookup,:);
Dlk = D(lookup,:);

% Find and remove the zero denominators
zero_D = find(Dlk==0);
Dlk(zero_D) = 1;

% Compute the barycentric coordinates
br(invalid,1) = dot(sph_corrs(invalid,:),N23(incident,:),2)./D(incident);
br(invalid,2) = dot(sph_corrs(invalid,:),N31(incident,:),2)./D(incident);
br(invalid,3) = dot(sph_corrs(invalid,:),N12(incident,:),2)./D(incident);

% Check that we have sensible Baricentic Coordinate Values
S = sum(br,2);
strange = find(S==0);
if ~isempty(strange)
    fprintf('We have some strange Baricentric Coordinate values in transform_corrs\n');
end
% Ensure that the barycentric coordinates are normalised
% so that the points end up on the triangel faces
br = br ./ repmat(S,1,3);

% Calculate the barycentric points on the surface
P1 = X(T(lookup,1),:);
P2 = X(T(lookup,2),:);
P3 = X(T(lookup,3),:);
shape_corrs = repmat(br(:,1),1,3).*P1 + repmat(br(:,2),1,3).*P2 + repmat(br(:,3),1,3).*P3;



    
    

