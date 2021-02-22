% DOCBLOCK num_trapz

% Copyright (c) 2008 Timothy Cyders
% Licensed under the GPL

function ret = trapz(x,y)
   
   ret=0;
   
   if nargin == 1
      
      if isvector(x) %%x is vector, unit integration
         for i = 2:length(x)
            ret = ret + (x(i)+x(i-1))/2;
         end
      
      else
         for i = 1:size(x)(2) %% i counts columns
            retj = 0;
            for j =2:size(x)(1) %% j counts rows
               retj = retj +(x(j,i)+x(j-1,i))/2;
            end
            ret(i) = retj;
         end
      end
      
   elseif nargin == 2;
      if isvector(y)
         for i = 2:length(x)
            ret = ret + (x(i) - x(i-1))*(y(i)+y(i-1))*.5;
         end
      else
         for i = 1:size(y)(2) %% i counts columns
            retj = 0;
            for j = 2:length(x) %% j counts rows
               retj = retj + (x(j)-x(j-1))*(y(j,i)+y(j-1,i))*.5;
            end
            ret(i) = retj;
         end
         
   end
end

      