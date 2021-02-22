% Check the range function with a negative float
function test_val = test_range6
range = -2.f:3.f;
range2 = [-2.f,-1.f,0.f,1.f,2.f,3.f];
test_val = test(range == range2);
