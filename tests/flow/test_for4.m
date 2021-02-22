% Test a simple 'for' statement with a return clause
function test_val = test_for4
tval = test_for4_assist;
test_val = test(tval == 15);

function sum = test_for4_assist
sum = 0;
for i = 1:10;
 sum = sum + i;
 if (i==5), return; end;
end
