% DOCBLOCK elementary_sub2ind

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function n = sub2ind(sizevec,varargin)
  if (length(varargin) == 0)
    n = [];
    return;
  end
  % Make sure sizevec is large enough
  sizevec = sizevec(:)';
  if (length(sizevec) < length(varargin))
    sizevec = [sizevec,ones(1,length(varargin)-length(sizevec))];
  end
  % Test each indexing component - make sure they are the
  % same length, and that all are in the correct range
  indvecs = {};
  nomlength = length(varargin{1}(:));
  for i=1:length(varargin)
    indvecs{i} = varargin{i}(:);
    if (length(indvecs{i}) ~= nomlength)
      error 'all indexing arguments to sub2ind must be the same length'
    end
    if (min(indvecs{i}) < 1)
      error 'indexing arguments are out of range of an array of the given size'
    end
  end
  % Everything is OK, so combine these into a single indexing vector
  outvec = zeros(nomlength,1);
  slicesize = 1;
  for i=1:length(varargin)
    outvec = outvec + slicesize*(indvecs{i}-1);
    slicesize = slicesize*sizevec(i);
  end
  n = (outvec + 1)';
  n = reshape(n,size(varargin{1}));

