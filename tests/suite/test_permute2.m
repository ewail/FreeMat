function test_val = test_permute2
z = rand(3,5,2,4,7);
perm = [3,5,1,4,2];
y = ipermute(permute(z,perm),perm);
test_val = all(y == z);
