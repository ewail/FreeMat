function test_val = test_evalin2
   evalin('base','qv32 = true;');
   if (exist('qv32'))
     test_val = false;
     return;
   end;
   test_val = evalin('base','qv32');
