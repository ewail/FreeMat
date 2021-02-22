% DOCBLOCK handle_semilogy

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = semilogy(varargin)
    handle = plot(varargin{:});
    set(gca,'yscale','log');
    if (nargout > 0)
        ohandle = handle;
    end
    
