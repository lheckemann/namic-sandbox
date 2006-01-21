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
% I_smooth3 = kappa3(I,nIter,dt);
% I_smooth = kappa(I,nIter,dt);
% imwrite(I_smooth3,'I_smooth3.png');
% imwrite(I_smooth,'I_smooth.png');
I_smooth3 = imread('I_smooth3.png');
I_smooth = imread('I_smooth.png');

% Shear Original Image
A = [2  0;
     0 .5];
% this only works for A = [2 0;0 0.5]
for x = 1:dimX,
  for y = 2:2:dimY,
    New = A*[x;y];
    xNew = New(1);
    yNew = New(2);

    % Shear the Original Image
    iVal = mean([I(y,x) I(y-1,x)]);
    I_shear(yNew,xNew) = iVal;
    I_shear(yNew,xNew-1) = iVal;

    % Shear the Smooth3 Image
    iVal = mean([I_smooth3(y,x) I_smooth3(y-1,x)]);
    I_smooth3_shear(yNew,xNew) = iVal;
    I_smooth3_shear(yNew,xNew-1) = iVal;

    % Shear the Smooth Image
    iVal = mean([I_smooth(y,x) I_smooth(y-1,x)]);
    I_smooth_shear(yNew,xNew) = iVal;
    I_smooth_shear(yNew,xNew-1) = iVal;
  end
end
imwrite(I_shear,'I_shear.png');
imwrite(I_smooth3_shear,'I_smooth3_shear.png');
imwrite(I_smooth_shear,'I_smooth_shear.png');

% Smooth Sheared Image
% I_shear_smooth3 = kappa3(I_shear,nIter,dt);
% I_shear_smooth = kappa(I_shear,nIter,dt);
% imwrite(I_shear_smooth3,'I_shear_smooth3.png');
% imwrite(I_shear_smooth,'I_shear_smooth.png');
I_shear_smooth3 = imread('I_shear_smooth3.png');
I_shear_smooth = imread('I_shear_smooth.png');

% Display Results
figure
subplot(3,2,1), imshow(I,[]), xlabel('Original Image');
subplot(3,2,2), imshow(I_shear,[]), xlabel('Sheared Original Image');
subplot(3,2,3), imshow(I_smooth3_shear,[]), xlabel('AI Smooth Shear');
subplot(3,2,4), imshow(I_smooth_shear,[]), xlabel('NAI Smooth Shear');
subplot(3,2,5), imshow(I_shear_smooth3,[]), xlabel('AI Shear Smooth');
subplot(3,2,6), imshow(I_shear_smooth,[]), xlabel('NAI Shear Smooth');


% LEGACY
% Crop Images and Write to File
% N = size(I_shear,1);
% a = (N-dimY)/2 + 1;
% b = a+dimY-1;
% I_shear = I_shear(a:b,:);
% I_shear_smooth3 = I_shear_smooth3(a:b,:);
% I_shear_smooth = I_shear_smooth(a:b,:);
% I_smooth3_shear = I_smooth3_shear(a:b,:);
% I_smooth_shear = I_smooth_shear(a:b,:);

