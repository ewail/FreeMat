function test_val = test_reshape1
  a = [1,6,7;3,4,2]; 
  b = [1,4;3,7;6,2];
  c = reshape(a,3,2);
  test_val = issame(b,c);
