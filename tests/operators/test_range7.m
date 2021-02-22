% Check the range function with a negative step size
function test_val = test_range7
range = 3:-1:-2;
range2 = [3,2,1,0,-1,-2];
test_val = test(range == range2);
