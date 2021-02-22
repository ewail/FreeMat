% DOCBLOCK io_csvread

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = csvread(filename,firstrow,firstcol,range)
    if (nargin == 1)
	x = dlmread(filename,',');
    elseif (nargin == 3)
        x = dlmread(filename,',',firstrow,firstcol);
    elseif (nargin == 4)
        x = dlmread(filename,',',range);
    end
        
