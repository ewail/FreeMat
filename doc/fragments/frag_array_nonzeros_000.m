0
a = rand(8); a(a>0.2) = 0;
A = sparse(a)
nonzeros(A)
