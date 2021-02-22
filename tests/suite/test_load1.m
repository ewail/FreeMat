function test_val = test_load1
  v = [1,2,3,4;5,6,7,8;9,6,2,3];
  save('test_load1.txt','-ascii','v');
  u = load('test_load1.txt','-ascii');
  test_val = issame(v,u);
