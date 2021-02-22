% Test the fitfun bug (bug 1514605)
function test_val = test_fitfun1
O = 3/4*pi;
y0 = cos(O);
[x y] = fitfun('cos',3.5/4*pi,y0,1,0.0001);
test_val = abs((x-O)/O*100)<.1;
