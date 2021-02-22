% Check the size function with an n-dim argument & one output
function test_val = test_size1
a = [];
a(3,7,2) = 1.0;
c = size(a);
test_val = test(c(1) == 3) & test(c(2) == 7) & test(c(3) == 2);
