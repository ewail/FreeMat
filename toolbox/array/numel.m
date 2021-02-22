% DOCBLOCK inspection_numel

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function len = numel(x,varargin)
if (nargin==1)
  len = prod(size(x));
else
  len = 1;
  for k=1:length(varargin)
    len = len * numel(varargin{k});
  end
end
