% Check the range function with a negative step size
function test_val = test_range8
range = 3f:-1f:-2f;
range2 = [3f,2f,1f,0f,-1f,-2f];
test_val = test(range == range2);
