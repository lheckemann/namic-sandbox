clear all
close all

nIter = 300;
dt = 0.10;
lambda = 2;

% Original Image
I = cast(imread('triangle.png'),'double');
dimY = size(I,1); dimX = size(I,2);

% Smooth Original Image
I_smooth3 = kappa3(I,nIter,dt);
I_smooth = kappa(I,nIter,dt);
imwrite(I_smooth3,'I_smooth3.png');
imwrite(I_smooth,'I_smooth.png');
% I_smooth3 = imread('I_smooth3.png');
% I_smooth = imread('I_smooth.png');

% Shear Original Image
A = [lambda  0;
     0 1/lambda];
for x = 1:dimX,
  for y = lambda:lambda:dimY,
    New = A*[x;y];
    xNew = New(1);
    yNew = New(2);

    % Shear the Original Image
    iValSum = 0;
    for n = 0:lambda-1,
        iValSum = iValSum+I(y-n,x);
    end
    iVal = iValSum/lambda;
    for n = 0:lambda-1,
        I_shear(yNew,xNew-n) = iVal;
    end

    % Shear the Smooth3 Image
    iValSum = 0;
    for n = 0:lambda-1,
        iValSum = iValSum+I_smooth3(y-n,x);
    end
    iVal = iValSum/lambda;
    for n = 0:lambda-1,
        I_smooth3_shear(yNew,xNew-n) = iVal;
    end

    % Shear the Smooth Image
    iValSum = 0;
    for n = 0:lambda-1,
        iValSum = iValSum+I_smooth(y-n,x);
    end
    iVal = iValSum/lambda;
    for n = 0:lambda-1,
        I_smooth_shear(yNew,xNew-n) = iVal;
    end
  end
end
imwrite(cast(I_shear,'uint8'),'I_shear.png');
imwrite(cast(I_smooth3_shear,'uint8'),'I_smooth3_shear.png');
imwrite(cast(I_smooth_shear,'uint8'),'I_smooth_shear.png');

% Smooth Sheared Image
I_shear_smooth3 = kappa3(I_shear,nIter,dt);
I_shear_smooth = kappa(I_shear,nIter,dt);
imwrite(cast(I_shear_smooth3,'uint8'),'I_shear_smooth3.png');
imwrite(cast(I_shear_smooth,'uint8'),'I_shear_smooth.png');
% I_shear_smooth3 = imread('I_shear_smooth3.png');
% I_shear_smooth = imread('I_shear_smooth.png');

% Unshear Images
dimY = size(I_shear,1);
dimX = size(I_shear,2);
B = [1/lambda  0;
     0 lambda];
for x = lambda:lambda:dimX,
  for y = 1:dimY,
    New = B*[x;y];
    xNew = New(1);
    yNew = New(2);

    % Unshear the Shear Image
    iValSum = 0;
    for n = 0:lambda-1,
        iValSum = iValSum+I_shear(y,x-n);
    end
    iVal = iValSum/lambda;
    for n = 0:lambda-1,
        I_shear_unshear(yNew-n,xNew) = iVal;
    end

    % Unshear the Smooth3 Image
    iValSum = 0;
    for n = 0:lambda-1,
        iValSum = iValSum+I_shear_smooth3(y,x-n);
    end
    iVal = iValSum/lambda;
    for n = 0:lambda-1,
        I_shear_smooth3_unshear(yNew-n,xNew) = iVal;
    end

    % Unshear the Smooth Image
    iValSum = 0;
    for n = 0:lambda-1,
        iValSum = iValSum+I_shear_smooth(y,x-n);
    end
    iVal = iValSum/lambda;
    for n = 0:lambda-1,
        I_shear_smooth_unshear(yNew-n,xNew) = iVal;
    end
  end
end
imwrite(cast(I_shear_unshear,'uint8'),'I_shear_unshear.png');
imwrite(cast(I_shear_smooth3_unshear,'uint8'),'I_shear_smooth3_unshear.png');
imwrite(cast(I_shear_smooth_unshear,'uint8'),'I_shear_smooth_unshear.png');

% Display Sheared Results
figure
subplot(3,2,1), imshow(I,[]), xlabel('Original Image');
subplot(3,2,2), imshow(I_shear,[]), xlabel('Sheared Original Image');
subplot(3,2,3), imshow(I_smooth3_shear,[]), xlabel('AI Smooth Shear');
subplot(3,2,4), imshow(I_smooth_shear,[]), xlabel('NAI Smooth Shear');
subplot(3,2,5), imshow(I_shear_smooth3,[]), xlabel('AI Shear Smooth');
subplot(3,2,6), imshow(I_shear_smooth,[]), xlabel('NAI Shear Smooth');

% Display Unsheared Results
figure
subplot(3,2,1), imshow(I,[]), xlabel('Original Image');
subplot(3,2,2), imshow(I_shear_unshear,[]), xlabel('Sheared Unsheared Image');
subplot(3,2,3), imshow(I_smooth3,[]), xlabel('AI Smooth');
subplot(3,2,4), imshow(I_smooth,[]), xlabel('NAI Smooth');
subplot(3,2,5), imshow(I_shear_smooth3_unshear,[]), xlabel('AI Shear Smooth Unshear');
subplot(3,2,6), imshow(I_shear_smooth_unshear,[]), xlabel('NAI Shear Smooth Unshear');

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

