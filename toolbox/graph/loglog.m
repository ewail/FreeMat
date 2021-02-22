% DOCBLOCK handle_loglog

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = loglog(varargin)
    handle = plot(varargin{:});
    set(gca,'xscale','log');
    set(gca,'yscale','log');
    if (nargout > 0)
        ohandle = handle;
    end
    
