% Check function declaration with two return args
function test_val = test_fcall2
try
   [a,b] = fc2fuc(1,3,2);
   test_val = test(a == 4);
catch
   test_val = 0;
end

function [a b] = fc2fuc(w,x,y)
  a = w+x;
  b = y;
