function test_val = test_assign20
  p = [];
  p(:,1) = [1;3;5];
  p(:,2) = [2;4;6];
  q = [1,2;3,4;5,6];
  test_val = testeq(p,q);
