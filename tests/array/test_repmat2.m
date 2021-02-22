function test_val = test_repmat2
  s = ones(2,2);
  p = repmat(s,[2 2 1]);
  test_val = all(p == ones(4));
