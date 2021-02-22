% DOCBLOCK handle_hist

%% Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2002, 2003,
%%               2004, 2005, 2006, 2007 John W. Eaton
%%
%% Copyright (C) 2008 Samit Basu, Eugene Ingerman
%%
%% This file is adopted in FreeMat from Octave under the conditions of the GNU 
%% General Public License
%%
%% Octave is free software; you can redistribute it and/or modify it
%% under the terms of the GNU General Public License as published by
%% the Free Software Foundation; either version 3 of the License, or (at
%% your option) any later version.
%%
%% Octave is distributed in the hope that it will be useful, but
%% WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
%% General Public License for more details.
%%
%% You should have received a copy of the GNU General Public License
%% along with Octave; see the file COPYING.  If not, see
%% <http://www.gnu.org/licenses/>.
%% Author: jwe

function [nn, xx] = hist (y, varargin)

  if (nargin < 1)
    error('Not enough input parameters');
  end

  arg_is_vector = isvector (y);

  if (size(y,1) == 1)
    y = y(:);
  end

  if (isreal (y))
    max_val = max (y(:));
    min_val = min (y(:));
  else
    error ('hist: first argument must be a vector');
  end

  iarg = 1;
  if (nargin == 1 || ischar (varargin{iarg}))
    n = 10;
    x = [0.5:n]'/n;
    x = x * (max_val - min_val) + ones(size(x)) * min_val;
  else
    %% nargin is either 2 or 3
    x = varargin{iarg};
    iarg=iarg+1;
    if (isscalar (x))
      n = x;
      if (n <= 0)
        error ('hist: number of bins must be positive');
      end
      x = [0.5:n]'/n;
      x = x * (max_val - min_val) + ones (size (x)) * min_val;
    elseif (isreal (x))
      if (isvector (x))
		x = x(:);
      end
      tmp = sort (x);
      if (any (tmp ~= x))
        warning ('hist: bin values not sorted on input');
        x = tmp;
      end
    else
      error ('hist: second argument must be a scalar or a vector');
    end
  end

  %% Avoid issues with integer types for x and y
  x = double (x);
  y = double (y);

  cutoff = (x(1:end-1,:) + x(2:end,:)) / 2;
  n = size(x,1);
  if (n < 30 && size(x,2) == 1)
    %% The following algorithm works fastest for n less than about 30.
    chist = zeros (n+1, size(y,2));
    for i = 1:n-1
      chist(i+1,:) = sum (y <= cutoff(i));
    end
    chist(n+1,:) = sum (~isnan (y));
  else
    %% The following algorithm works fastest for n greater than about 30.
    %% Put cutoff elements between boundaries, integrate over all
    %% elements, keep totals at boundaries.
    [s, idx] = sort ([y; cutoff]);
    len = size (y,1);
    chist = cumsum (idx <= len);
    t1 = zeros (1, size(y,2));
    t2 = reshape (chist(idx > len), size (cutoff));
    t3 = chist(end,:) - sum (isnan (y));
    chist = [t1; t2; t3];
  end

  freq = diff (chist);

  if (nargin > 2 && ~ischar(varargin{iarg}))
    %% Normalise the histogram.
    norm = varargin{iarg++};
    freq = freq / size(y,1) * norm;
  end

  if (nargout > 0)
    if (arg_is_vector)
      nn = freq';
      xx = x';
    else
      nn = freq;
      xx = x;
    end
  elseif (size (freq, 2) ~= 1)
    bar (x, freq, 0.8, varargin{iarg:end});
  else
    bar (x, freq, 1.0, varargin{iarg:end});
  end

 % function y=diff(x)
	% y=x(2:end)-x(1:end-1);
	
function h=bar( x, y, width, varargin )

	x=x(:);
	y=y(:);
	if (length(x) ~= length(y))
		error('cannot handle this case');
	end
		
	df = width*diff( x )/2;
   df(end+1) = df(end);
	x_r=x(1:end)-df;
	x_l=x(1:end)+df;
	
	n = length(x);
	vertices = [ x_r zeros(size(x_r)) ones(size(x_r)) ;...
				x_r y ones(size(x_r)) ;...
				x_l y ones(size(x_r)) ;...
				x_l zeros(size(x_l)) ones(size(x_r))];
	
	ind=(1:n)';
   faces = [ind ind+n ind+2*n ind+3*n];
   if ~ishold
      clf
   end
   h=patch('Faces',faces,'Vertices',vertices,'FaceColor',[0 0 1]);

%test
%  [nn,xx]=hist([1:4],3);
%  assert(xx, [1.5,2.5,3.5]);
%  assert(nn, [2,1,1]);
%test
%  [nn,xx]=hist([1:4]',3);
%  assert(xx, [1.5,2.5,3.5]);
%  assert(nn, [2,1,1]);
%test
%  [nn,xx]=hist([1 1 1 NaN NaN NaN 2 2 3],[1 2 3]);
%  assert(xx, [1,2,3]);
%  assert(nn, [3,2,1]);
%test
%  [nn,xx]=hist([[1:4]',[1:4]'],3);
%  assert(xx, [1.5;2.5;3.5]);
%  assert(nn, [[2,1,1]',[2,1,1]']);
%assert(hist(1,1),1);
%test
%  for n = [10, 30, 100, 1000]
%    assert( sum(hist([1:n], n)), n );
%    assert( sum(hist([1:n], [2:n-1])), n);
%    assert( sum(hist([1:n], [1:n])), n );
%    assert( sum(hist([1:n], 29)), n);
%    assert( sum(hist([1:n], 30)), n);
%  endfor
