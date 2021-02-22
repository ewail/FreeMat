function t = test_grayscalemorphologicalclosing1
  a = zeros(5);
  a(2:4,2:4) = .3;
  a(3,3) = 0;
  b = grayscalemorphologicalclosing(a,'ball',1);
  c = zeros(5);
  c(2:4,2:4) = .3;
  t = issame(b,c);
