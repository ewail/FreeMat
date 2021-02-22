% DOCBLOCK elementary_cov
function y = cov(x,z,d)
  if (nargin == 1) && isvector(x)
    y = var(x);
  elseif (nargin == 1)
    y = cov_matrix(x,size(x,1));
  elseif (nargin == 2 && all(size(x)==size(z)))
    y = cov([x(:),z(:)]);
  elseif (nargin == 2 && (isscalar(z)))
    if (z == 1)
      y = cov_matrix(x,size(x,1));
    elseif (z == 0)
      y = cov_matrix(x,max(1,size(x,1)-1));
    end
  elseif (nargin == 3 && all(size(x)==size(z)) && isscalar(d))
    y = cov([x(:),z(:)],d);
  else
    error('Unrecognized form for call to cov');
  end
end

function y = cov_matrix(x,norm_factor)
  x = x - repmat(mean(x),[size(x,1),1]);
  y = (x'*x)/norm_factor;
end