0
x = repmat(linspace(-1,1),[100,1]); y = x';
z = exp(-x.^2-y.^2);
image(z);
min(z(:))
max(z(:))
mprint clim1
