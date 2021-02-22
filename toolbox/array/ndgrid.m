% DOCBLOCK array_ndgrid

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function varargout = ndgrid(varargin)
  if (nargin == 0)
    error('ndgrid requires at least one argument');
  end
  if (nargin == 1)
    varargin = repmat(varargin,[1 max(nargout,2)]);
    nargin = length(varargin);
  end
  if (nargout > nargin)
    error('ndgrid expects number of outputs to be less than or equal to the number of inputs');
  end
  
  % Convert the input vectors into column vectors
  for i=1:length(varargin)
    x = full(varargin{i});
    varargin{i} = x(:);
    dims(i) = numel(varargin{i});
  end
  
  % dims is the dimension of the output arrays.
  % We now look over the output arrays, reshaping
  % them to be single dimension arrays along their
  % corresponding dimension
  varargout = cell(1,nargout);
  for i=1:nargout
    rdims = dims*0+1;
    rdims(i) = dims(i);
    x = reshape(varargin{i},rdims);
    rdims = dims;
    rdims(i) = 1;
    varargout{i} = repmat(x,rdims);
  end
