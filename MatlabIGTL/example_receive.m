%
% Example to use igtlreceive()
%

%%% read image data
sd = igtlopen('localhost', 18944);

if sd == -1
  error('Could not connect to the server.');
end

for n=1:10
  DATA = igtlreceive(sd);
end

igtlclose(sd);
