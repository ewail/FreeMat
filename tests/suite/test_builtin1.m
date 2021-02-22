function test_val = test_builtin1
  a = -1;
  b = builtin('abs',a);
  test_val = (b == 1);
end 
function y = abs(x)
  y = x;
end
