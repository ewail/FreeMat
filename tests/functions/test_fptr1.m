% Check function pointers that are members of structs (b3542238)
function test_val = test_fptr1
a.b = @cos;
test_val = issame(a.b(2.0),cos(2.0));
