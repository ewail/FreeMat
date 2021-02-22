% Test the foo class -- it replicates bug 2949163 -- helpgen broken
function testval = test_foo_1
  testval = false;
  try
    a = foo;
    hop(a);
    testval = true;
  catch
  end
  