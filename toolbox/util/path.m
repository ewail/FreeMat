% DOCBLOCK freemat_path

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = path(a,b)
if (strcmp(computer,'PCWIN'))
  pathdiv = ';';
else
  pathdiv = ':';
end
if ((nargout == 0) && (nargin == 0))
    a = getpath;
    b = strfind(a,pathsep);
    n = 1;
    for i=1:numel(b)
        printf('%s\n',a(n:(b(i)-1)));
        n = b(i)+1;
    end
        if ((~isempty( a )) && (~isempty(b)))
            printf('%s\n',a((b(end)+1):end));
        else
            printf('\n');
        end
    return;
end
if (nargout == 1)
  x = getpath;
else
  x = [];
end
if (nargin == 1)
  setpath(a);
elseif (nargin == 2)
  setpath([a,pathsep,b]);  
end
