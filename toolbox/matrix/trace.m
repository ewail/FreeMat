% DOCBLOCK array_trace

% Copyright (c) 2008 Samit Basu
% Licensed under the GPL

function ret = trace(x)
   
   [m n] = size(x);
   
   if m ~= n
        error('Matrix must be square!\n');
   end
   
   ret = sum(diag(x));
   
   end

