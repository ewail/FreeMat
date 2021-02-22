% DOCBLOCK elementary_ind2sub
% Copyright (C) 2001, 2003, 2004, 2005, 2006, 2007 Paul Kienzle
% This file is part of Octave.
% Licensed under the GPL

function varargout = ind2sub (dims, ind)

  if (nargin == 2)
    if (isvector (dims) && all (round (dims) == dims))
      if (isnumeric (ind) && all (round (ind) == ind))
	ntot = prod (dims);
	if (all (ind > 0 & ind <= ntot))
	  nd = length (dims);
	  if (nargout > 0)
	    vlen = nargout;
	  else
	    vlen = 1;
	  end
	  if (nd > vlen);
	    dims(vlen) = prod (dims(vlen:nd));
	    dims(vlen+1:nd) = [];
	  end
	  nd = length (dims);
	  scale = [1; cumprod(dims(:))];
	  for i = nd:-1:2
	    k = (ind >= scale(i));
	    r = ones (size (ind));
	    t = zeros (size (ind));
	    t(k) = floor ((ind(k) - 1) / scale(i));
	    r(k) = t(k) + 1;
	    varargout{i} = r;
	    ind(k) = ind(k) - t(k) * scale(i);
	  end
	  varargout{1} = ind;
	  for i = nd+1:vlen
	    varargout{i} = ones (size (ind));
	  end
	else
	  error ('ind2sub: index out of range');
	end
      else
	error ('ind2sub: expecting integer-valued index argument');
      end
    else
      error ('ind2sub: expecting dims to be an integer vector');
    end
  else
     error('ind2sub requires 2 arguments');
  end


