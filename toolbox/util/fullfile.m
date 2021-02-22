% DOCBLOCK os_fullfile

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = fullfile(varargin)
    if (nargin == 0)
        x = [];
        return;
    end
    x = [];
    for i=1:(nargin-1)
        x = [x,varargin{i},dirsep];
    end
    x = [x,varargin{end}];
    
