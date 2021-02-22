function x = test_subset21
  a = zeros(4,4,5);
  p = randn(4,4,1);
  a(:,:,end) = p;
  x = 1;
