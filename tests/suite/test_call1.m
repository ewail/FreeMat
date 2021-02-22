% test the multiple left-hand side call with a pre-existing cell-array
function test_val = test_call1
d1 = {1,2};
d2 = {5};
d3 = {6};
[d1,d2,d3] = test_call1_assist;
test_val = test(d1 == 1) & test(d2 == 4) & test(d3 == 3);

function [a,b,c] = test_call1_assist
a = 1;
b = 4;
c = 3;
