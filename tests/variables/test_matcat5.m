% Check the type promotion for the matrix cat function
function test_val = test_matcat5
a = [1,2;3.0,4.0+i];
test_val = test(strcmp(typeof(a),'double'));
