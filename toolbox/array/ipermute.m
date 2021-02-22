% DOCBLOCK array_ipermute

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = ipermute(x,p)
  iperm = zeros(1,length(p));
  iperm(p) = 1:length(p);
  y = permute(x,iperm);
