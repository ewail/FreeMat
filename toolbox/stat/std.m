% DOCBLOCK elementary_std

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = std(varargin)
  y = sqrt(var(varargin{:}));
