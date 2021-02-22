% Check the range function with negative integer
function test_val = test_range5
range = -2:2;
range2 = [-2,-1,0,1,2];
test_val = test(range == range2);
