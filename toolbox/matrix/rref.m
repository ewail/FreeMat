% DOCBLOCK array_rref
function [A,k] = rref(A, tolerance)
% Original notice for rref from 
% http://users.powernet.co.uk/kienzle/octave/matcompat/rref.m
% Copyright (C) 2000 Paul Kienzle
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
% rref   Reduced row echelon form
%      R = rref (A, tol) returns the reduced row echelon form of a.
%      tol defaults to eps * max (size (A)) * norm (A, inf)
%
%      [R, k] = rref (...) returns the vector of "bound variables",
%      which are those columns on which elimination has been performed.
% Author: Paul Kienzle (based on a anonymous source from the public
% domain)  
  
  [rows,cols] = size (A);
  if (nargin < 2)
    tolerance = eps * max (rows, cols) * norm (A, inf);
  end
  used = zeros(1,cols);
  r = 1;
  for c=1:cols
    % Find the pivot row
    [m, pivot] = max (abs (A (r:rows, c)));
    pivot = r + pivot - 1;
    if (m <= tolerance)
      % Skip column c, making sure the approximately zero terms are
      % actually zero.
      A (r:rows, c) = zeros (rows-r+1, 1);
    else
      % keep track of bound variables
      used (1, c) = 1;
      % Swap current row and pivot row
      A ([pivot, r], c:cols) = A ([r, pivot], c:cols);
      % Normalize pivot row
      A (r, c:cols) = A (r, c:cols) / A (r, c);
      % Eliminate the current column
      ridx = [1:r-1, r+1:rows];
      A (ridx, c:cols) = A (ridx, c:cols) - A (ridx, c) * A(r, c:cols);
      % Check if done
      if (r == rows) break; end
      r = r + 1;
    end
  end
  k = find(used);
end

