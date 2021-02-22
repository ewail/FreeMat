% DOCBLOCK thread_threadcall

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function varargout = threadcall(id,timeout,funcname,varargin)
  threadstart(id,funcname,nargout,varargin{:});
  if (~threadwait(id,timeout))
    error(sprintf('Timeout on call to %s',funcname));
  end
  varargout = cell(1,nargout);
  varargout{:} = threadvalue(id);
  
