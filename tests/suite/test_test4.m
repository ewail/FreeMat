% Check that test function works properly for a vector with one zero
function test_val = test_test4

test_val = test([1,0,1,1]) == 0;
