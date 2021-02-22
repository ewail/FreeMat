function t = test_derivativefilter1
  a = zeros(5);
  a(3,3) = 1;
  b = derivativefilter(a,1,1);
  c = zeros(5);
  c(2,3) = .5;
  c(4,3) = -.5;
  t = issame(b,c);
