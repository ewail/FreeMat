function t = test_grayscaleerode1
  a = zeros(5);
  a(2:4,2:4) = 0.3;
  b = grayscaleerode(a,'ball',1);
  c = zeros(5);
  c(3,3) = 0.3;
  t = issame(b,c);
