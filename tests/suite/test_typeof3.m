% Check the typeof function for float types
function test_val = test_typeof3
a = 2.0f;
test_val = test(strcmp(typeof(a),'single'));
