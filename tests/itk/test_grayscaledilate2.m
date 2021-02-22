function test_val = test_grayscaledilate2
  a = zeros(5);
  a(3,3) = .3;
  b = grayscaledilate(a,'cross');
  c = zeros(5);
  c(2:4,3) = .3;
  c(3,2:4) = .3;
  test_val = issame(c,b);
