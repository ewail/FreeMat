function test_val = test_repmat1
  s = ones(2,2,1);
  p = repmat(s,[2 2]);
  test_val = all(p == ones(4));
