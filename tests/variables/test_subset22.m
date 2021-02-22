function x = test_subset22
  a = rand(10,2);
  p = a(:,2);
  c = p(:,1);
  x = issame(p,c);
