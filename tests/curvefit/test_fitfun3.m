% Test the fitfun with a local function
function test_val = test_fitfun3
O = 3/4*pi;
y0 = cos(O);
[x y] = fitfun(@locos,3.5/4*pi,y0,1,0.0001);
test_val = abs((x-O)/O*100)<.1;

function y = locos(x)
  y = cos(x);
