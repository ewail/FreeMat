% DOCBLOCK inspection_maxdim

function n = maxdim(x);
  n = min(find(size(x) == max(size(x))));
