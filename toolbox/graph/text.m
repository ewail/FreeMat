% DOCBLOCK handle_text

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function handles = text(varargin)
if (nargin < 3)
  error 'text requires at least three arguments, the x and y location vectors and the strings'
end
xvec = varargin{1};
yvec = varargin{2};
labels = varargin{3};
varargin(1:3) = [];
if (length(xvec) ~= length(yvec))
  error 'vectors x and y must be the same length'
end

if (isa(labels,'char'))
  labelarray = repmat({labels},[length(xvec),1]);
elseif (iscellstr(labels))
  labelarray = labels;
  if (length(labelarray) ~= length(xvec))
    error 'number of labels much match the length of the x and y vectors'
  end
else
  error 'labels must be either a single string or a cell array of strings.'
end

if (nargout > 0)
  handles = [];
end

for (i=1:numel(xvec))
  h = htext('position',[xvec(i),yvec(i)],'string',labelarray{i},varargin{:});
  if (nargout > 0)
    handles = [handles,h];
  end
end

