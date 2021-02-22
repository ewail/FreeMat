% Check the typeof function for complex types
function test_val = test_typeof4
a = 2.0f + i;
test_val = test(strcmp(typeof(a),'single'));
