function t = test_recursivegaussian1
  a = zeros(5);
  a(3,3) = 1;
  b = recursivegaussian(a,1,1,1,0);
  v1 = b(3,:);
  p = -2:2;
  v2 = exp(-p.^2/2)/sqrt(2*pi).*(-p);
  t = norm(v1-v2,'fro') < 0.01;

