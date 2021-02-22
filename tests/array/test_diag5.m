% Test the diagonal creation function with no arguments (bug 1620051)
function test_val = test_diag5
test_val = 1;
try
  b = diag;
catch
  test_val = 1;
end
