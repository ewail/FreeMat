function test_val = test_cell1
  A = cell(1,2,4);
  B = [1 2 4];
  test_val = issame(size(A),B);
