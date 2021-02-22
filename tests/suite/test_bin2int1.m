function test_val = bin2int1
   A = floor(rand(4,4,3)*10);
   B = int2bin(A,4);
   C = bin2int(B);
   test_val = issame(A,C);
