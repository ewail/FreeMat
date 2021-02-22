% Test the arithmetic operators with empty arguments
function test_val = test_empty
test_val = isempty([]+[]);
test_val = test_val & isempty([]-[]);
test_val = test_val & isempty(-[]);
test_val = test_val & isempty([]*[]);
test_val = test_val & isempty([]/[]);
test_val = test_val & isempty([]\[]);
test_val = test_val & isempty([].*[]);
test_val = test_val & isempty([]./[]);
test_val = test_val & isempty([].\[]);
test_val = test_val & isempty([]^[]);
test_val = test_val & isempty([].^[]);
test_val = test_val & isempty([]>[]);
test_val = test_val & isempty([]>=[]);
test_val = test_val & isempty([]<[]);
test_val = test_val & isempty([]<=[]);
