% Check the range function for integer types positive with 3 args & horizontal concatenation
function test_val = test_range2
range = (0:1:3)';
range2 = [0;1;2;3];
test_val = test(range == range2);
