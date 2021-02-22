% Test for assignment of empties to an empty
function test_val = test_assign13
    A = [];
    A([],[],[]) = [];
    test_val = 1;
