% Test the ndim contents-based assign for multiple values - with
% too many outputs
function test_val = test_subset16
a = {0,0;0,0};
test_val = 0;
try
  [a{1:2,1:2}] = test_subset16_assist
catch
  test_val = 1;
end

function [a,b,c] = test_subset16_assist
  a = 1;
  b = 2;
  c = 3;
