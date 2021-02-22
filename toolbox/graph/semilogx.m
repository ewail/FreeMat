% DOCBLOCK handle_semilogx

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = semilogx(varargin)
    handle = plot(varargin{:});
    set(gca,'xscale','log');
    if (nargout > 0)
        ohandle = handle;
    end
    
