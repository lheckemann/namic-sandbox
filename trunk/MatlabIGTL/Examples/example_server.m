%
% Example to use igtlwaitconn()
%

%%% Create server and get server file descriptor
sfd = igtlopen('', 18945);

cfd = igtlwaitcon(sfd, 1000);
if cfd > 0
  disp ('Client connected.')
  while cfd
    [S, DATA] = igtlreceive(cfd);
    if S==0 % connection lost
      close(cfd)
      cfd = 0;
    end
  end
end


igtlclose(sfd);


