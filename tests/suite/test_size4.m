% Check the size function with two arguments and two outputs
function test_val = test_size4
a = [1,2,3;4,5,6];
test_val = 0;
[c,d] = size(a,2);
test_val = (c == 3);
