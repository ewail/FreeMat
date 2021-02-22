% Test for itk erode in 2D
function test_val = test_erode1
  a = zeros(5);
  a(2:4,2:4) = 1;
  b = erode(a);
  c = zeros(5,'logical');
  c(3,3) = 1;
  test_val = issame(c,b);
