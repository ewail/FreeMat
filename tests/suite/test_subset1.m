% Test the vector subset operator
function test_val = test_subset1
a = [1,2,3,4;5,6,7,8];
b = [3;4;5];
c = a(b);
test_val = test(c==[2;6;3]);
