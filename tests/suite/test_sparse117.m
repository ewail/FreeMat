function x = test_sparse117
% large sparse matrices
n=250000;
h1=1:n;
h2=2:n;
a=1.5; %an arbitrary factor
g1=ones(1,n);
g2=ones(1,n-1);
m1=sparse(h1,h1,a*g1,n,n); %works fine
m2=a*sparse(h1,h1,g1,n,n); %fails under freemat 4.0
%Error message: Cannot allocate enough memory to store an array of size 2370457856
m3=sparse(h2-1,h2,g2,n,n);
m4=m3+m3'; %works
x = 1;
