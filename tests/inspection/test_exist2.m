function x = test_exist2
persistent y
x = 1;
if (~exist('y'))
  x = 0;
end
