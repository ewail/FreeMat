% Test for multiple assign with structures
function test_val = test_assign10
a = zeros(3,4);
[m.x m.y] = size(a);
test_val = (m.x == 3) & (m.y == 4);
