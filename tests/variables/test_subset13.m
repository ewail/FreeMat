% Test the ndim subset contents-based assign function
function test_val = test_subset13
a = {1,2,3;4,5,6};
a{4} = 'goo';
test_val = test(strcmp(a{2,2},'goo'));
