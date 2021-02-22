% Check the typeof function for integer types
function test_val = test_typeof1
a = 2;
test_val = test(strcmp(typeof(a),'double'));
