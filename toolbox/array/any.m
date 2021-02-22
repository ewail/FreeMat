% DOCBLOCK elementary_any

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = any(A,dim)
  if (nargin == 0)
    error 'any function requires at least one argument'
  end
  A(isnan(A))=0;
  if (nargin == 1)
    if (isempty(A))
      if (ndims(A) < 3)
        y = false;
      else
        v = size(A);
        v(min(find(v > 1))) = 1;
        y = logical(ones(v));
      end
    else
      y = max(logical(A));
    end
 else
    if ~isempty( A )
       y = max(logical(A),[],dim);
    else
       v=size(A);
       v(dim)=1;
       y=zeros(v,'logical');
    end
  end
