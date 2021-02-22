function t = test_morphologicalgradient1
  a = zeros(5);
  a(2:4,2:4) = .3;
  b = morphologicalgradient(a);
  c = grayscaledilate(a) - grayscaleerode(a);
  t = issame(b,c);

