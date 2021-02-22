% DOCBLOCK elementary_all

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = all(A,dim)
  if (nargin == 0)
    error 'all function requires at least one argument'
  end
  if (nargin == 1)
    if  ~isempty(A) 
      y = min(logical(A));
    else
      if (ndims(A) < 3)
        y = true;
      else
        v=size(A);
        v(min(find(v > 1))) = 1;
        y=logical(ones(v));
      end
    end
  else
    if ~isempty(A)
      y = min(logical(A),[],dim);
    else
      v=size(A);
      v(dim)=1;
      y=logical(ones(v));
    end
  end
