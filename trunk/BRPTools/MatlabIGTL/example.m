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

IMGDATA.Type = 'IMAGE';
IMGDATA.Name = 'MatlabImage';
IMGDATA.Image = I;
IMGDATA.Trans = M;
r = igtlsend(sd, IMGDATA);

IMGDATA.Type = 'TRANSFORM';
IMGDATA.Name = 'MatlabTrans';
IMGDATA.Trans = M;
r = igtlsend(sd, IMGDATA);


igtlclose(sd);