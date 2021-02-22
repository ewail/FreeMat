0
w = sqrt(sum(A'.^2));
sA = diag(1./w)*A;
plot(A);
mprint('colormap6');
