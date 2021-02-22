function test_val = test_seed1
seed(32,41);
  a1 = rand(1,5);
  seed(32,41);
  a2 = rand(1,5);
  test_val = issame(a1,a2);
