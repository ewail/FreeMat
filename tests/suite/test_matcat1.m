% Check the matcat function
function test_val = test_matcat1
a = [1;2];
b = 3;
d = 4;
c = [a,[b;d]];
test_val = test(c==[1,3;2,4]);
