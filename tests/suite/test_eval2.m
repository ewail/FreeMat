function test_val = test_eval2
  a = rand(10);
  [s1,v1,d1] = svd(a);
  [s2,v2,d2] = eval('svd(a)');
  test_val = issame(s1,s2) && issame(v1,v2) && issame(d1,d2);
