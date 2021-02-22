0
a = sparse([1,0,0,4;3,2,0,0;0,0,0,1;4,3,2,4])
[l,u,p,q,r] = lu(a)
full(l*a)
b = r*a
full(b(p,q))
