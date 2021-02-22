function test_val = test_evalin1
   test_val = false;
   do_test_evalin1_subfunc;
end

function do_test_evalin1_subfunc
   evalin('caller','test_val = true');
   evalin('caller','test_val = true','test_val=false');
end
