% DOCBLOCK array_nonzeros

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = nonzeros(y)
[i,j,x] = find(y);
x = cast(reshape(x,[numel(x),1]),class(y));
