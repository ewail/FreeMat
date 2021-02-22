% Test for bug 1808557 - incorrect subset assignment with complex arrays
function test_val = test_assign16
   x = rand(4,2)+i*rand(4,2);
   y = rand(2,2)+i*rand(2,2);
   x(2:2:4,:)=y;
   z = x;
   for i=1:2;
     for j=1:2;
       z(2+(i-1)*2,j) = y(i,j);
     end
   end
  q = x;
  q(:,1) = y(:);
  p = [x,x];
  p(1,:) = y(:);
  test_val = all(vec(z==x)) && all(q(:,1) == y(:)) && all(p(1,:).' == y(:));
