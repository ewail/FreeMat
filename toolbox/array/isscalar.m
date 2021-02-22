% DOCBLOCK inspection_isscalar
%   M version contributor: M.W. Vogel 01-22-06
% Licensed under the GPL
function v = isscalar(a)
  v = (numel(a) == 1);
  
