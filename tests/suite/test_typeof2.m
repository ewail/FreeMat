% Check the typeof function for double types
function test_val = test_typeof2
a = 2.0;
test_val = test(strcmp(typeof(a),'double'));
