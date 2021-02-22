% Check the type promotion for the matrix cat function
function test_val = test_matcat6
a = [1,2;3.0f,4.0f+i];
test_val = test(strcmp(typeof(a),'single'));
