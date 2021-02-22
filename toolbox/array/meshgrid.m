% DOCBLOCK array_meshgrid

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function [X,Y,Z] = meshgrid(x,y,z)
  if (nargin == 0)
    error('meshgrid requires at least one input argument');
  end
  if (nargin == 1)
    X = repmat(x(:)',[numel(x),1]);
    Y = X';
  elseif (nargout == 2)
    X = repmat(x(:)',[numel(y),1]);
    Y = repmat(y(:),[1,numel(x)]);
  else
    X = repmat(x(:)',[numel(y),1,numel(z)]);
    Y = repmat(y(:),[1,numel(x),numel(z)]);
    Z = repmat(reshape(z,[1,1,numel(z)]),[numel(y),numel(x),1]);
  end
  
  
    
    
