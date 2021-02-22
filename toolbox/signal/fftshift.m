% DOCBLOCK transforms_fftshift

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = fftshift(x,dim)
if (nargin > 1)
  if (numel(dim) ~= 1 | dim ~= floor(dim(1)) | dim < 1)
    error('dim must be a positive scalar integer');
  end
  shiftvec = zeros(1,dim);
  shiftvec(dim) = floor(size(x,dim)/2);
else
  shiftvec = floor(size(x)/2);
end
y = circshift(x,shiftvec);

