0
A = float(rand(4))
[u,s,v] = svd(A)    % Compute the full decomposition
sigmas = svd(A)     % Only want the singular values
