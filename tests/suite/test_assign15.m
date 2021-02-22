% Test for error on illegal (incomplete) assign to empty variables
function test_val = test_assign15
   a = [2,3;4,5];
   test_val = 0;
   try
     c(2,:) = a;
   catch
     test_val = 1;
   end
