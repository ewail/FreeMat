% Test the nested fitfun bug (bug 1741350)
function test_val = test_fitfun2
a = 2;
b = 0;
init = [a,b];
y = [1:100];
weights = y*0+1;
tol = 1.e-08;
junk = rand(100);
[xopt,yopt] = fitfun('fitfunc_func1',init,y,weights,tol,junk);
test_val = abs(xopt(1)-1) < 1e-6;
