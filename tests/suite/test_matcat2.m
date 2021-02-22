% Check the matcat function with n-dimensional arrays
function test_val = test_matcat2
a = 1;
a(1,1,1) = 1;
a(1,2,1) = 2;
a(1,1,2) = 5;
a(1,2,2) = 6;
c = [a;a];
c1 = c(:,:,1);
c2 = c(:,:,2);
test_val = test(c1==[1,2;1,2]) & test(c2==[5,6;5,6]);
