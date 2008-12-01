function [c,indx] = matchpursuit(A,f,L)
%use this to find the best match in the basis

f=f(:);
n=length(f);
if (nargin<3);
   L=n;
else
   L=min(L,n);
end

Rf = f;

for k=1:L,
   [tmp,indx(k)]=max(abs(A'*Rf));
   g(:,k)=A(:,indx(k));

   if (k==1),
       b=[];
       u(:,k)=g(:,k);
   else
       b=(u'*g(:,k))./normu(1:k-1)';
       u(:,k)=g(:,k)-u*b;
   end

   normu(k)=sum(u(:,k).*u(:,k));
   c(k)=(u(:,k)'*Rf)./normu(k);
   Rf=Rf-c(k)*u(:,k);
   Rf(Rf<0)=0;
   B(1:k,k)=[b; 1];
end
c=B\c(:);
