% Test for auto sizing of assignment to undefined variables
function test_val = test_assign14
   r = [2,3;3,4];
   a(2,:,:) = r;
   b = zeros(2,2,2);
   b(2,:,:) = r;
   test_val = all(a(:) == b(:)) && all(size(a) == size(b));
