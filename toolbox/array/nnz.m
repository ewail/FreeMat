% DOCBLOCK inspection_nnz
function y = nnz(x)
   p = find(x);
   y = numel(p);
