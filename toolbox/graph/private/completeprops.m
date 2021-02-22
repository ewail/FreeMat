% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function q = completeprops(cs,ms,ps,p)
   if (strcmp(cs,'none'))
     q = {'marker',ms,'linestyle',ps,p{:}};
   else
     q = {'color',cs,'marker',ms,'linestyle',ps,'markeredgecolor',cs,p{:}};
   end
