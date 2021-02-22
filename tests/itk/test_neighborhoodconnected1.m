function test_val = test_neighborhoodconnected1
A = zeros(5);
A(2:4,2:4) = 1;
B = neighborhoodconnected(A,[3;3],.5,2,2);
C = zeros(5);
C(3,3) = 2;
test_val = issame(B,C);
