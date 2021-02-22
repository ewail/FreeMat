function c = wbtest_near_permute(a,b)
  a = sort(a(:));
  b = sort(b(:));
  c = wbtest_near(a,b);
  