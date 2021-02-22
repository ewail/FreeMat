% Check to RHS function calls with multiple return values (b989865)
function test_val = test_fcall1
try
b = 1:10;
B = addthem(min(b),max(b));
test_val = test(B == 11);
catch
test_val = 0;
end

function c = addthem(a,b)
c = a + b;
