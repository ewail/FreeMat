% Check the typeof function for dcomplex types
function test_val = test_typeof5
a = 2.0 + i;
test_val = test(strcmp(typeof(a),'double'));
