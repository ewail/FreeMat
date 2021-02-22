% Test the zeros function
function x = test_sparse69
xi = int32(sparse(100,200));
yi = int32(zeros(100,200));
xf = float(sparse(100,200));
yf = float(zeros(100,200));
xd = double(sparse(100,200));
yd = double(zeros(100,200));
xc = complex(sparse(100,200));
yc = complex(zeros(100,200));
xz = dcomplex(sparse(100,200));
yz = dcomplex(zeros(100,200));
x = testeq(xi,yi) & testeq(xf,yf) & testeq(xd,yd) & testeq(xc,yc) & testeq(xz,yz);
