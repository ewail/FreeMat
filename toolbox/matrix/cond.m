% DOCBLOCK array_cond

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cond(A,p)
    if (nargin < 2)
        p = 2;
    end
    if (nargin < 1)
        error('expect at least 1 input to cond function');
    end
    if (isempty(A)) y = 0; return; end;
    if (p ~= 2)
        y = norm(A,p)*norm(inv(A),p);
    else
        if (isscalar(A) && (A == 0))
          y = inf;
          return;
        end
        s = svd(A);
        y = s(1)/s(end);
    end
    
