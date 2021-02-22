% DOCBLOCK freemat_addpath

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function addpath(varargin)
    if (nargin == 0) return; end
    atbegin = 1;
    if (any(strcmp(varargin{end},{'-0','-begin'})))
        varargin(end) = [];
    elseif (any(strcmp(varargin{end},{'-1','-end'})))
        atbegin = 0;
        varargin(end) = [];
    end
    if (atbegin)
        g = [];
        for i=1:numel(varargin)
            g = [g,varargin{i},pathsep];
        end
        g = [g,getpath];
        setpath(g);
    else
        g = getpath;
        for i=1:numel(varargin)
            g = [g,pathsep,varargin{i}];
        end
        setpath(g);
    end
