0
A = randi(1,3*ones(15,3))
unique(A,'rows')
[b,m,n] = unique(A,'rows');
b
A(m,:)
b(n,:)
