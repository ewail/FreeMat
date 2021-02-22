% Test the vector contents-based assign for multiple values
function test_val = test_subset14
a = {0,0,0,0};
[a{1:4}] = test_subset14_assist;
test_val = test(strcmp(typeof(a),'cell')) & ...
    (a{1} == 1) & (a{2} == 2) & (a{3} == 3) & (a{4} == 4);

function [a,b,c,d] = test_subset14_assist
  a = 1;
  b = 2;
  c = 3;
  d = 4;
