% DOCBLOCK handle_datacursormode

% Copyright (c) 2010 Eugene Ingerman
% Licensed under the GPL

function varargout=datacursormode(varargin)
    if nargin == 1
        if strcmp(varargin{1}, 'on') 
            varargout{1} = datacursormanager();
        end
    end
