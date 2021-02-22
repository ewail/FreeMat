% DOCBLOCK typecast_char

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = char(varargin)
  if ((nargin == 1) && ~iscell(varargin{1}))
    y = string(varargin{1});
  elseif ((nargin == 1))
    y = char_block(varargin{1});
  else
    y = char_block(varargin);
  end

      
function y = char_block(x)
  maxwidth = 0;
  for (i=1:numel(x))
    p = char_flatten(string(x{i}));
    x{i} = p;
    maxwidth = max(maxwidth,size(p,2));
  end;
  y = [];
  for (i=1:numel(x))
    k = x{i}; p = size(k,2);
    y = [y;[k,' '+zeros(max(1,size(k,1)),maxwidth-p)]];
  end
  
function y = char_flatten(x)
  y = reshape(x,size(x,1),numel(x)/max(1,size(x,1)));
  