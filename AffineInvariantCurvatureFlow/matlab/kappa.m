%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


% geometric diffusion
% ------------------------
% I_in is the input image (in double precision format)
% dt is the time step (should be less then .25 for stability)
% steps is the number of diffusion iterations to perform
% I_out is the diffused output image (in double precision format)

function I_out = kappa(I_in,steps,dt)

%Set input image as the initial value of the diffused image

I_out=I_in;

% Perform diffusion iterations

for n=1:steps,

  disp(['... diffusion iteration ' num2str(n)])

  %Get neighbor values (in matrix form with Neumann boundary conditions)

  I_p0 = [I_out(2:end,:); I_out(end,:)];
  I_m0 = [I_out(1,:); I_out(1:end-1,:)];
  I_0p = [I_out(:,2:end) I_out(:,end)];
  I_0m = [I_out(:,1) I_out(:,1:end-1)];

  I_pp = [I_out(2:end,2:end) I_out(2:end,end); I_out(end,2:end) I_out(end,end)];
  I_mm = [I_out(1,1) I_out(1,1:end-1); I_out(1:end-1,1) I_out(1:end-1,1:end-1)];
  
  I_pm = [I_out(2:end,1) I_out(2:end,1:end-1); I_out(end,1) I_out(end,1:end-1)];
  
  I_mp = [I_out(1,2:end) I_out(1,end); I_out(1:end-1,2:end) I_out(1:end-1,end)];
  
  
  %Compute derivatives (discrete central difference approximations)

  Dxx = I_p0 - 2*I_out + I_m0;
  Dyy = I_0p - 2*I_out + I_0m;

  Dx = (I_p0 - I_m0)/2;
  Dy = (I_0p - I_0m)/2;
  Dxy = (I_pp + I_mm - I_pm - I_mp)/4;
  
  nom = Dxx.*Dy.^2 - 2*Dx.*Dy.*Dxy + Dyy.*Dx.^2;
  denom = Dx.^2 + Dy.^2;
  
  % search for zeros of the gradient
  
  indx = find(abs(denom)<=eps);
  nom(indx) = Dxx(indx)+Dyy(indx);
  denom(indx) = 1;
    
  %Compute diffusion

  diffusion = nom./denom;

  %Perform single forward-Euler iteration of diffusion PDE

  I_out = I_out + dt*diffusion;

end 

