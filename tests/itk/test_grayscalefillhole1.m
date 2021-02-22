function t = test_grayscalefillhole1
  a = zeros(5);
  a(2:4,2:4) = 0.3;
  b = grayscalefillhole(a);
  c = zeros(5);
  c(2:4,2:4) = 0.3;
  t = issame(b,c);
