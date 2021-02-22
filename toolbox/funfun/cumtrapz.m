% DOCBLOCK num_cumtrapz

% Copyright (c) 2008 Timothy Cyders
% Licensed under the GPL

function ret = cumtrapz(x,y)
   
   ret=0;
	
   if nargin == 1
      
      if isvector(x) %%x is vector, unit integration
         ret(1) = 0;
         for i = 2:length(x)
            ret(i) = ret(i-1) + (x(i)+x(i-1))/2;
         end

      else
         for i = 1:size(x,2) %% i counts columns
            retj = 0;
            for j =2:size(x,1) %% j counts rows
               retj(j) = retj(j-1) + (x(j,i)+x(j-1,i))/2;
            end
            ret(i,1:size(x,1)) = retj';
         end
         ret=ret';
      end
      
   elseif nargin == 2;
      if isvector(y)
         ret = 0;
         for i = 2:length(x)
            ret(i) = ret(i-1) + (x(i) - x(i-1))*(y(i)+y(i-1))*.5;
         end
      else
         for i = 1:size(y,2) %% i counts columns
            retj = 0;
            for j = 2:length(x) %% j counts rows
               retj(j) = retj(j-1) + (x(j)-x(j-1))*(y(j,i)+y(j-1,i))*.5;
            end
            ret(i,1:length(x)) = retj';
         end
         ret = ret';         
   end
end

      
