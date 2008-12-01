function [basis, srt, gradsOut] = findBasisAndRadon( output )
% function [basis, srt, gradsOut] = findBasisAndRadon( output )
% This function computes the basis and radon from gradient
% directions.
% Input Parameters:
%   output is a struct containing all of the output information
% Return Value:
%   basis is the signal matrix
%   srt is the spherical radon transform
%   gradsOut are the corresponding directions (3 x nDir)

%% --- ORIGINAL COMMENTS
%Input
%u = all the gradient directions

%Output
%The basis (signal) matrix, the ODF matrix and the corresponding
%directions (3 x grad_dir) matrix

% Set stuff up
gradsIn = output.grads;
gradsIn = cat( 1, gradsIn, -gradsIn );
basic_dir = [];
gradsOut = [];

%% Let's first find the anti-podal directions
U=(abs(gradsIn*gradsIn')>(1-1e-9));
U=U-eye(size(U));
ct = 1;

for i=1:size(gradsIn,1)
  [tmp,id] = find(U(i,:)==1);
  if(~isempty(id))
    basic_dir(ct) = i;
    antipodal(ct) = id;
    U(i,id) = 0;
    U(id,i) = 0;
    ct= ct+1;
  end
end

ct = 1;

% We no longer use this since we threshold on FA
% % Isotropic case
% k = 0.1; % Our scaling parameter

% for i = 1:length(basic_dir)
%   eta = acos(gradsIn(basic_dir(i),:)*gradsIn');
  
%   sig  =  exp(-k * cos(eta).^2);
%   basis(:,ct) = sig/norm(sig);
  
%   %% Compute the corresponding spherical radon transform
%   srt(:,ct) = exp(-k * sin(eta).^2);
%   srt(:,ct) = srt(:,ct)/norm(srt(:,ct));
  
%   gradsOut(:,ct) = gradsIn(basic_dir(i),:);
%   ct = ct + 1;
% end

% Anisotropic case
k = output.processVars.ODFEstimKParam; % Our scaling parameter

for i = 1:length(basic_dir)
  eta = acos(gradsIn(basic_dir(i),:)*gradsIn');
  
  sig  =  exp(-k * cos(eta).^2);
  basis(:,ct) = sig/norm(sig);
  
  %% Compute the corresponding spherical radon transform
  srt(:,ct) = exp(-k * sin(eta).^2);
  srt(:,ct) = srt(:,ct)/norm(srt(:,ct));
  
  gradsOut(:,ct) = gradsIn(basic_dir(i),:);
  ct = ct + 1;
end
