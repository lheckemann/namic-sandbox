clear all
close all

nIter = 1000;
dt = 0.10;

% Original Image
I = cast(imread('triangle.png'),'double');
dimY = size(I,1); dimX = size(I,2);

% Rescale
oneIdx = find(I);
I(oneIdx) = 255;

% Smooth Original Image
I_smooth3 = kappa3(I,nIter,dt);
imwrite(I_smooth3,'I_smooth3.png');
I_smooth = kappa(I,nIter,dt);
imwrite(I_smooth,'I_smooth.png');

% Shear Original Image
A = [1 2 0;
     0 1 0;
     0 0 1];
tform1 = maketform('affine',A);
I_shear = imtransform(I,tform1);
tform2 = maketform('affine',inv(A));
I_unshear_orig = imtransform(I_shear,tform2);

% Smooth Sheared Image
I_shear_smooth3 = kappa3(I_shear,nIter,dt);
I_shear_smooth = kappa(I_shear,nIter,dt);

% Unshear Smoothed Image
I_unshear3 = imtransform(I_shear_smooth3,tform2);
I_unshear = imtransform(I_shear_smooth,tform2);

% Crop Images and Write to File
N = size(I_shear,1);
a = (N-dimY)/2 + 1;
b = a+dimY-1;
I_shear = I_shear(a:b,:);
imwrite(I_shear,'I_shear.png');
I_shear_smooth3 = I_shear_smooth3(a:b,:);
imwrite(I_shear_smooth3,'I_shear_smooth3.png');
I_shear_smooth = I_shear_smooth(a:b,:);
imwrite(I_shear_smooth,'I_shear_smooth.png');

N = size(I_unshear_orig,1);
a = (N-dimY)/2 + 1;
b = a+dimY-1;
I_unshear_orig = I_unshear_orig(a:b,:);
imwrite(I_unshear_orig,'I_unshear_orig.png');
I_unshear3 = I_unshear3(a:b,:);
imwrite(I_unshear3,'I_unshear3.png');
I_unshear = I_unshear(a:b,:);
imwrite(I_unshear,'I_unshear.png');

% Display Results
figure
subplot(4,2,1), imshow(I,[]), xlabel('Original Image');
subplot(4,2,2), imshow(I_shear,[]), xlabel('Sheared Original Image');
subplot(4,2,3), imshow(I_smooth3,[]), xlabel('AI Smooth');
subplot(4,2,4), imshow(I_smooth,[]), xlabel('NAI Smooth');
subplot(4,2,5), imshow(I_shear_smooth3,[]), xlabel('AI Sheared Smooth');
subplot(4,2,6), imshow(I_shear_smooth,[]), xlabel('NAI Sheared Smooth');
subplot(4,2,7), imshow(I_unshear3,[]), xlabel('AI Unsheared');
subplot(4,2,8), imshow(I_unshear,[]), xlabel('NAI Unsheared');
