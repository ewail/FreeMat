function test_val = test_permute1
z = rand(3,5,2,4,7);
perm = [3,5,1,4,2];
sizez = size(z);
y = permute(z,perm);
sizey = size(y);
test_val = all(sizey == sizez(perm));
