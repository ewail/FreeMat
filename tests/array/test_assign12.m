% Test for column assignment bug with complex arrays
function test_val = test_assign12
    A = [1+i 3-i 5+3i
         7-i 8+9i 6+i
         9   8-i   5i];
    B = A;
    A(:,2) = 0;
    B(1:3,2) = 0;
    test_val = all(A==B);
