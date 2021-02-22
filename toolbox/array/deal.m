% DOCBLOCK elementary_deal

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function varargout = deal(varargin)
  if (nargin ~= nargout) && (nargin ~= 1)
    error('number of outputs must match number of inputs');
  end
  if (nargin == 1)
    varargout = varargin(ones(1,max(1,nargout)));
  else
    varargout = varargin;
  end

    
