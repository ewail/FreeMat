function test_val = test_zeros1
  a = zeros(2,3,'single');
  test_val = (size(a) == [2,3]) && strcmp(typeof(a),'single');
