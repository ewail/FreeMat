function test_val = test_grayscaledilate1
  a = zeros(5);
  a(3,3) = .3;
  b = grayscaledilate(a,'ball',1);
  c = zeros(5);
  c(2:4,2:4) = .3;
  test_val = issame(c,b);
