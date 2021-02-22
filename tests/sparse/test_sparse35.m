function x = test_sparse35
s = [0,3,4,5;0,0,0,0;0,2,3,0];
a = sparse(s);
x = testeq(full(a),s) & testeq(full(float(a)),s) & testeq(full(double(a)),s) & testeq(full(complex(a)),s) & testeq(full(dcomplex(a)),s);
