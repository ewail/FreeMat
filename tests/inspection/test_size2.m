% Check the size function with an n-dim argument & multiple outputs
function test_val = test_size2
a = [1,2,3;4,5,6];
[c1,c2,c3] = size(a);
test_val = test(c1 == 2) & test(c2 == 3) & test(c3 == 1);
