% Test the field assignment with a vector struct (should cause an error)
function test_val = test_assign6
a = struct('foo',{5,7});
test_val = 0;
try
a.foo = -1;
catch
test_val = 1;
end
