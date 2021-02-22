function t = test_simplecontourextractor1
A = zeros(5);
A(2:4,2:4) = 1;
B = simplecontourextractor(A,[1,1],1,0,.3,0);
C = zeros(5);
C(2:4,2:4) = 0.3;
C(3,3) = 0;
t = issame(B,C);
