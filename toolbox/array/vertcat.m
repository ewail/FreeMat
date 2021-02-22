% DOCBLOCK class_vertcat

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = vertcat(varargin)
  if (nargin == 0)
    y = [];
  else
    y = varargin{1};
    for i=2:nargin
      y = [y;varargin{i}];
    end
  end
