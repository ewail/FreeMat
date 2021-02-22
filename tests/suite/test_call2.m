% test the multiple left-hand side call with a pre-existing cell-array
function test_val = test_call2
[d1{1:3}] = test_call2_assist;
test_val = test(d1{1} == 1) & test(d1{2} == 4) & test(d1{3} == 3);

function [a,b,c] = test_call2_assist
a = 1;
b = 4;
c = 3;
