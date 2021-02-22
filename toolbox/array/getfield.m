% DOCBLOCK elementary_getfield

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = getfield(x,varargin)
if (isempty(varargin))
  error('Need at least one additional input to getfield');
end

% Check for the simple case of a single field index
field = varargin{1};
if (length(varargin)==1 && isa(field,'char'))
  y = x.(field);
  return;
end
% Loop through the references
y = x;
for i=1:length(varargin)
  ndx = varargin{i};
  if (isa(ndx,'cell'))
    y = y(ndx{:});
  elseif (isa(ndx,'char'))
    % Dereference the field - if a contents list is returned
    % handle appropriately using a deal call
    if (length(y) == 1)
      y = y.(ndx);
    else
      tmp = cell(1,length(y));
      [tmp{:}] = deal(y.(ndx));
      y = tmp{1};
    end
  else
    error('unrecognized indexing type');
  end
end

