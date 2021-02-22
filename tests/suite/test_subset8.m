% Test the ndim subset with a cell-array vector
function test_val = test_subset8
a = {1,2,3;4,5,6;7,8,9};
c = struct('foo',a);
b = c(2,2:3);
test_val = test(strcmp(typeof(b),'struct')) & ...
               test((b(1).foo == 5) & (b(2).foo == 6));
