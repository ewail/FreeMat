function x = test_exist1
x = test_exist1_assist;
x = test_exist1_assist;

function y = test_exist1_assist
persistent x
if (exist('x'))
  y = 1;
else
  y = 0;
  x = 1;
end
