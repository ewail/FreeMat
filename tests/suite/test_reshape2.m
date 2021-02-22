function test_val = test_reshape2
  a = rand(3,4);
  b = reshape(a,6,2);
  c = reshape(a,6,[]);
  d = reshape(a,[],2);
  test_val = issame(b,c) && issame(b,d);
