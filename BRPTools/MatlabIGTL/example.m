%
% Example to use igtlsend()
%


%%% read image data
fid = fopen('igtlTestImage1.raw', 'r');
I = fread(fid, [256 256], 'uint8')';
fclose(fid);

%%% affine transform matrix
M = [1.0, 0.0, 0.0, 0.0;
     0.0,-1.0, 0.0, 0.0;
     0.0, 0.0, 1.0, 0.0;
     0.0, 0.0, 0.0, 1.0];

sd = igtlopen('localhost', 18944);
r = igtlsend(sd, 'MatlabImage', I, M);
igtlclose(sd);


