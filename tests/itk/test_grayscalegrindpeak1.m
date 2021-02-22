function t = test_grayscalegrindpeak1
  a = 0.3*ones(5);
  a(3,3) = 1;
  b = grayscalegrindpeak(a,'ball',1);
  c = 0.3*ones(5);
  t = issame(b,c);
