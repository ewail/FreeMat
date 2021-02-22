% DOCBLOCK elementary_diff

% Copyright (C) 1995, 1996, 1999, 2000, 2002, 2004, 2005, 2006, 2007
%               Kurt Hornik
%%
%% Copyright (C) 2008 Samit Basu, Eugene Ingerman
%%
%% This file is adopted in FreeMat from Octave under the conditions of the GNU 
%% General Public License
%
%
% Octave is free software; you can redistribute it and/or modify it
% under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 3 of the License, or (at
% your option) any later version.
%
% Octave is distributed in the hope that it will be useful, but
% WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
% General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with Octave; see the file COPYING.  If not, see
% <http://www.gnu.org/licenses/>.

% Author: KH <Kurt.Hornik@wu-wien.ac.at>
% Created: 2 February 1995
% Adapted-By: jwe

function x = diff (x, k, dim)

  if (nargin < 1 || nargin > 3)
    error('incorrect number of parameters');
  end

  if (nargin < 2 || isempty(k))
    k = 1;
  else
    if ( ~ (isscalar (k) && k == round (k) && k >= 0))
      error ('diff: k must be a nonnegative integer');
    elseif (k == 0)
      return;
    end
  end

  nd = ndims (x);
  sz = size (x);
  if (nargin ~= 3)
    %% Find the first non-singleton dimension
    dim  = 1;
    while (dim < nd + 1 && sz (dim) == 1)
      dim = dim + 1;
    end
    if (dim > nd)
      dim = 1;
    end
  else
    if (~ (isscalar (dim) && dim == round (dim)) && dim > 0 && dim < (nd + 1))
      error ('diff: dim must be an integer and valid dimension');
    end
  end

%  if (ischar (x))
%    error ('diff: symbolic differentiation not (yet) supported');
%  end


  if (nargin == 3)
    if (sz (dim) <= k)
      sz(dim) = 0;
      if (isa(x,'single'))
        x = zeros (sz,'single');
      else
        x = zeros(sz);
      end
    else
      n = sz (dim);
      idx1 = cell ();
      for i = 1:nd
		idx1{i} = 1:sz(i);
      end
      idx2 = idx1;
      for i = 1 : k;
		idx1{dim} = 2 : (n - i + 1);	
		idx2{dim} = 1 : (n - i);	
		x = x(idx1{:}) - x(idx2{:});
      end
    end
  else
    if (sum (sz - 1) < k)
      if (isa(x,'single'))
        x = single([]);
      else
        x = [];
      end;
    else
      idx1 = cell ();
      for i = 1:nd
		idx1{i} = 1:sz(i);
      end
      idx2 = idx1;
      while (k)
		n = sz (dim);
		for i = 1 : min (k, n - 1)
		  idx1{dim} = 2 : (n - i + 1);	
		  idx2{dim} = 1 : (n - i);	
		  x = x(idx1{:}) - x(idx2{:});
		end
		idx1{dim} = 1;
		idx2{dim} = 1;
		k = k - min (k, n - 1);
		dim = dim + 1;
      end
    end
  end

