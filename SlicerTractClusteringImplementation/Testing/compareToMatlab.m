% This script loads the input matrix from the testCluster program:
%
% inputMatrix.txt
%
% This script loads these output files from itkSpectralClustering:
%
% embed.txt
% eigenvectors.txt
% eigenvalues.txt
%
% It then computes the eigensystem and embedding using matlab
% and compares the results.
%
% Note the eigenvectors are generally the same until near-zero
% eigenvalues are reached, and the differences in those "smaller"
% eigenvectors are not meaningful.
%

disp( 'Loading input matrix to compare matlab and C++ results');
load inputMatrix.txt
[r,c] = size(inputMatrix);
% row sum normalization
normalize=repmat(sum(inputMatrix,1),r,1);
normalize2=repmat(sum(inputMatrix,2),1,c);
normalize=sqrt(normalize.*normalize2);

normalizedInputMatrix = inputMatrix./normalize;
[v , d] = eig( normalizedInputMatrix );

% compare embedding to the embedding we get
% note: turn off embedding normalization in the class for this test
load embed.txt
[r,c] = size(embed);
% get the last c columns in reverse order to correspond to 
% the embedding we load from disk
% skip the last one which is constant as long as the
% input matrix is not perfect 1s and 0s block diagonal
disp( 'Matlab then C++ embeddings:');
embedMatlab = v(:,end-1:-1:end-c)
embed

load eigenvectors.txt
load eigenvalues.txt

% re-create the matrix:
origMatrix1=eigenvectors*diag(eigenvalues)*inv(eigenvectors);
origMatrix2=v*d*inv(v);

% look at it
figure; plot(eigenvalues); title('Eigenvalues from c++');
figure; plot(diag(d)); title('Eigenvalues from matlab');
figure; imagesc(origMatrix1); title('V*D*inv(V) from c++'); colorbar;
figure; imagesc(origMatrix2); title('V*D*inv(V) from c++'); colorbar;
figure; imagesc(embed); title('Embedding from c++'); colorbar;
figure; imagesc(embedMatlab); title('Embedding from matlab'); colorbar;


% plot in 2D 
figure;plot(embed(:,1),embed(:,2),'*'); 
title('Embedding from c++');
figure;plot(embedMatlab(:,1),embedMatlab(:,2),'*'); 
title('Embedding from matlab');

