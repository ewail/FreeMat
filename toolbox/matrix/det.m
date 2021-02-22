% DOCBLOCK array_det

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = det(A)
    if (~isnumeric(A))
        error('Argument to det must be numeric');
    end
    if (ndims(A) ~= 2)
        error('argument to det must be 2D matrix');
    end
    if (size(A,1) ~= size(A,2))
        error('argument to det must be square');
    end
    if (isscalar(A))
      y = A;
      return;
    end
    N = size(A,1);
    [L,U,P] = lu(A);
    o = (1:N)';
    p = P*o;
    exchange_count = det_exchange_count(p);
    y = prod(diag(U));
    if (mod(exchange_count,2) ~= 0)
        y = -y;
    end
    
function exchange_count = det_exchange_count(pivot)
    exchange_count = 0;
    for i=1:numel(pivot)
        if (pivot(i) ~= i)
            k = find(pivot == i);
            p = pivot(i);
            pivot(i) = i;
            pivot(k) = p;
            exchange_count = exchange_count + 1;
        end
    end
