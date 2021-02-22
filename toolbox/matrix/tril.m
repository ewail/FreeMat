% DOCBLOCK array_tril

% Copyright (c) 2008 Samit Basu
% Licensed under the GPL

function ret = tril(x,n)
   
   [m n] = size(x);
   
   if m ~= n
        error('Matrix must be square!\n');
   end
	
   if nargin == 1
      for i = 1:size(x,1)
         for j = i+1:size(x,2)
            x(i,j) = 0;
         end
      end
  
   elseif nargin == 2     

      if n < 0
         t1 = zeros(size(x));
         for i = 1 + abs(n):size(x,1)
            for j = 1:i+n
               t1(i,j) = 1;
            end
         end
      elseif n > 0
         t1 = ones(size(x));
         for i = 1:size(x,1)-n
            for j = i+n+1:size(x,2)
               t1(i,j) = 0;
            end
         end
      elseif n == 0
          for i = 1:size(x,1)
            for j = i+1:size(x,2)
                x(i,j) = 0;
            end
        end
      end
      x = x.*t1;
   end
      ret = x;
end

