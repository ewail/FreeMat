% Torture test for subset indexing
function test_val = test_subset19
a1 = 5;
A{3}(2).foo(2) = a1;
test_val = test(strcmp(typeof(A),'cell')) & ...
    test(strcmp(typeof(A{3}),'struct')) & ...
    test(strcmp(typeof(A{3}(2).foo),'double'));
