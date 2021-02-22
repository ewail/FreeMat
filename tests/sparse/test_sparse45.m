function x = test_sparse45
a = rand(10);
A = sparse(a);
p = eigs(A,4,0.634);
d = eig(a);
e = min(abs(d*ones(1,4) - ones(10,1)*p.'));
x = max(e) < 100*eps;
