% DOCBLOCK signal_conv

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function z = conv(x,y)
% is x a column vector
% col * scalar = col
% col * row = row
% ndim * row = row
% col * row = col
xiscol = size(x,1) > size(x,2);
yiscol = size(y,1) > size(y,2);
if (xiscol | yiscol)
  z = conv2(x(:),y(:));
else
  z = conv2(x(:).',y(:).');
end
if (numel(x) > numel(y))
  maxdims = maxdim(x);
else
  maxdims = maxdim(y);
end
if (maxdims > 1)
  z = reshape(z,[ones(1,maxdims-1),numel(z)]);
end

