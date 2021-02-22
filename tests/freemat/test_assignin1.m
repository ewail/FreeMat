function test_val = test_assignin1
  test_val = false;
  do_test_assignin1_subfunc;
end

function do_test_assignin1_subfunc
  assignin('caller','test_val',true);
end
