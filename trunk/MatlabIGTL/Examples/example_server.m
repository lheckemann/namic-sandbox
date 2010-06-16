function DATA = example_server(timeout)
%
% Example to use igtlwaitconn()
%

%%% Create server and get server file descriptor
sfd = igtlopen('', 18944);
disp(sprintf('File descriptor = %d', sfd))

while sfd > 0
  cfd = igtlwaitcon(sfd, timeout);
  if cfd > 0
    disp ('Client connected.')
    while cfd
      [S, DATA] = igtlreceive(cfd);
      if S==0 % connection lost
	igtlclose(cfd)
	cfd = 0;
      else
        %figure; image(DATA.Image(:,:,60))
	DATA.Trans
        DATA.Image = convn (DATA.Image, ones (5, 5, 5) / (5*5*5), 'same');
	DATA.Name = 'MatlabImage';
	r = igtlsend(cfd, DATA);
      end
    end
  else
    igtlclose(sfd);
    sfd = 0;
  end
end



