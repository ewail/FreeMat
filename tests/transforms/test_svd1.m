% Test the svd with finite and infinite args (functional only)
function test_val = test_svd1
  a = [1,3;4,2];
  [u,s,v] = svd(a);
  res1 = 1;
  res2 = 0;
  res3 = 0;
  res4 = 0;
  try 
    c = a;
    c(2,2) = inf;
    svd(c);
  catch
    res2 = 1;
  end
  try
    c = a;
    c(2,2) = -inf;
    svd(c);
  catch
    res3 = 1;
  end
  try
    c = a;
    c(2,2) = nan;
    svd(c);
  catch
    res4 = 1;
  end
  
  test_val = res1 & res2 & res3 & res4;
