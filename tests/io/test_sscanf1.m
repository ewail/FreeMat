function test_val = test_sscanf1
A = sscanf('1.34 54 5.67','%g');
B = [1.34;54;5.67];
test_val = issame(A,B);
